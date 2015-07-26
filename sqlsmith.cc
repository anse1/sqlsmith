#include <iostream>
#include <pqxx/pqxx>
#include <numeric>
#include <chrono>
#include <regex>

#include <thread>
#include <mutex>
#include <memory>

#include "random.hh"
#include "grammar.hh"
#include "relmodel.hh"
#include "schema.hh"

#include "config.h"

#include "log.hh"
#include "dump.hh"

using namespace std;
using namespace pqxx;

using namespace std::chrono;

extern "C" {
#include <unistd.h>
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
      "    --dry-run            print queries instead of executing" << endl <<
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

      if (options.count("verbose"))
	loggers.push_back(make_shared<cerr_logger>());

      if (options.count("dump-all-graphs"))
	loggers.push_back(make_shared<ast_logger>());
      
      smith::rng.seed(options.count("seed") ? stoi(options["seed"]) : getpid());

      milliseconds query_time(0);
      milliseconds gen_time(0);

      if (options.count("dry-run")) {
	while (1) {
	  query_spec gen = query_spec((struct prod *)0, &scope);
	  gen.out(cout);
	  for (auto l : loggers)
	    l->generated(gen);
	  cout << endl;
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
		 "set lc_messages to 'C';");
	  w.commit();

	  while (1) {
	    if (options.count("max-queries")
		&& (queries_generated++ > stol(options["max-queries"])))
	      return 0;
	    
	    work w(c);
	    auto g0 = high_resolution_clock::now();
	    query_spec gen = query_spec((struct prod *)0, &scope);
	    auto g1 = high_resolution_clock::now();
	    gen_time += duration_cast<milliseconds>(g1-g0);

	    for (auto l : loggers)
	      l->generated(gen);
	  
	    ostringstream s;
	    gen.out(s);

	    try {
	      auto q0 = high_resolution_clock::now();
	      result r = w.exec(s.str() + ";");
	      auto q1 = high_resolution_clock::now();
	      query_time =  duration_cast<milliseconds>(q1-q0);
	      for (auto l : loggers)
		l->executed(gen);
	      w.abort();
	    } catch (const pqxx::failure &e) {
	      for (auto l : loggers)
		try {
		  l->error(gen, e);
		} catch (runtime_error &e) {
		  cerr << endl << "log failed: " << typeid(*l).name() << ": "
		       << e.what() << endl;
		}
	      if ((dynamic_cast<const broken_connection *>(&e))) {
		throw;
	      }
	    }
	  }
	}
	catch (const broken_connection &e) {
	  this_thread::sleep_for(milliseconds(1000));
	}
      }
    }
  catch (const exception &e) {
    cerr << e.what() << endl;
    return 1;
  }
}
