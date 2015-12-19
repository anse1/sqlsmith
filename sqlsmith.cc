#include "config.h"

#include <iostream>
#include <pqxx/pqxx>
#include <chrono>

#ifndef HAVE_BOOST_REGEX
#include <regex>
#else
#include <boost/regex.hpp>
using boost::regex;
using boost::smatch;
using boost::regex_match;
#endif

#include <thread>

#include "random.hh"
#include "grammar.hh"
#include "relmodel.hh"
#include "schema.hh"

#include "gitrev.h"

#include "log.hh"
#include "dump.hh"

using namespace std;
using namespace pqxx;

using namespace std::chrono;

extern "C" {
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
}

/* make the cerr logger globally accessible so we can emit one last
   report on SIGINT */
cerr_logger *global_cerr_logger;

extern "C" void cerr_log_handler(int)
{
  if (global_cerr_logger)
    global_cerr_logger->report();
  exit(1);
}

int main(int argc, char *argv[])
{
  cerr << "sqlsmith " << GITREV << endl;

  map<string,string> options;
  regex optregex("--(help|log-to|verbose|target|version|dump-all-graphs|seed|dry-run|max-queries)(?:=((?:.|\n)*))?");
  
  for(char **opt = argv+1 ;opt < argv+argc; opt++) {
    smatch match;
    string s(*opt);
    if (regex_match(s, match, optregex)) {
      options[string(match[1])] = match[2];
    } else {
      cerr << "Cannot parse option: " << *opt << endl;
      return 1;
    }
  }

  if (options.count("help")) {
    cerr <<
      "    --log-to=connstr     log errors to database" << endl <<
      "    --target=connstr     database to send queries to" << endl <<
      "    --seed=int           seed RNG with specified int instead of PID" << endl <<
      "    --dump-all-graphs    dump generated ASTs" << endl <<
      "    --dry-run            print queries instead of executing them" << endl <<
      "    --max-queries=long   terminate after generating this many queries" << endl <<
      "    --verbose            emit progress output" << endl <<
      "    --version            print version information and exit" << endl <<
      "    --help               print available command line options and exit" << endl;
    return 0;
  } else if (options.count("version")) {
    cerr << GITREV << endl;
    return 0;
  }

  try
    {
      schema_pqxx schema(options["target"]);
      scope scope;
      long queries_generated = 0;
      schema.fill_scope(scope);
//       work w(c);
//       w.commit();

      vector<shared_ptr<logger> > loggers;

      if (options.count("log-to"))
	loggers.push_back(make_shared<pqxx_logger>(options["target"], options["log-to"], schema));

      if (options.count("verbose")) {
	auto l = make_shared<cerr_logger>();
	global_cerr_logger = &*l;
	loggers.push_back(l);
	signal(SIGINT, cerr_log_handler);
      }
      
      if (options.count("dump-all-graphs"))
	loggers.push_back(make_shared<ast_logger>());
      
      smith::rng.seed(options.count("seed") ? stoi(options["seed"]) : getpid());

      milliseconds query_time(0);
      milliseconds gen_time(0);

      if (options.count("dry-run")) {
	while (1) {
	  shared_ptr<prod> gen = statement_factory(&scope);
	  gen->out(cout);
	  for (auto l : loggers)
	    l->generated(*gen);
	  cout << ";" << endl;
	  queries_generated++;

	  if (options.count("max-queries")
	      && (queries_generated >= stol(options["max-queries"])))
	      return 0;
	}
      }
      
      connection c(options["target"]);

      while (1)
      {
	try {
	  work w(c);
	  w.exec("set statement_timeout to '1s';"
		 "set client_min_messages to 'ERROR';"
		 "set application_name to 'sqlsmith';");
	  w.commit();

	  while (1) {
	    if (options.count("max-queries")
		&& (++queries_generated > stol(options["max-queries"]))) {
	      if (global_cerr_logger)
		global_cerr_logger->report();
	      return 0;
	    }
	    
	    work w(c);

	    /* Invoke top-level production to generate AST */
	    auto g0 = high_resolution_clock::now();
	    shared_ptr<prod> gen = statement_factory(&scope);
	    auto g1 = high_resolution_clock::now();
	    gen_time += duration_cast<milliseconds>(g1-g0);

	    for (auto l : loggers)
	      l->generated(*gen);
	  
	    /* Generate SQL from AST */
	    ostringstream s;
	    gen->out(s);

	    /* Try to execute it */
	    try {
	      auto q0 = high_resolution_clock::now();
	      result r = w.exec(s.str() + ";");
	      auto q1 = high_resolution_clock::now();
	      query_time =  duration_cast<milliseconds>(q1-q0);
	      for (auto l : loggers)
		l->executed(*gen);
	      w.abort();
	    } catch (const pqxx::failure &e) {
	      for (auto l : loggers)
		try {
		  l->error(*gen, e);
		} catch (runtime_error &e) {
		  cerr << endl << "log failed: " << typeid(*l).name() << ": "
		       << e.what() << endl;
		}
	      if ((dynamic_cast<const broken_connection *>(&e))) {
		/* re-throw to outer loop to recover session. */
		throw;
	      }
	    }
	  }
	}
	catch (const broken_connection &e) {
	  /* Give server some time to recover. */
	  this_thread::sleep_for(milliseconds(1000));
	}
      }
    }
  catch (const exception &e) {
    cerr << e.what() << endl;
    return 1;
  }
}
