#include "config.h"

#include <iostream>
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
#include <typeinfo>

#include "random.hh"
#include "grammar.hh"
#include "relmodel.hh"
#include "schema.hh"
#include "gitrev.h"

#include "log.hh"
#include "dump.hh"
#include "impedance.hh"
#include "dut.hh"

#ifdef HAVE_LIBSQLITE3
#include "sqlite.hh"
#endif

#ifdef HAVE_LIBMYSQLCLIENT
#include "mysql.hh"
#endif

#include "postgres.hh"

using namespace std;

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
  cerr << PACKAGE_NAME " " GITREV << endl;

  map<string,string> options;
<<<<<<< HEAD
  regex optregex("--(help|log-to|verbose|target|sqlite|mysql|version|dump-all-graphs|seed|dry-run|max-queries|exclude-catalog)(?:=((?:.|\n)*))?");
=======
  regex optregex("--(help|log-to|verbose|target|sqlite|mysql|version|dump-all-graphs|seed|dry-run|max-queries)(?:=((?:.|\n)*))?");
>>>>>>> fusion/mysql
  
  for(char **opt = argv+1 ;opt < argv+argc; opt++) {
    smatch match;
    string s(*opt);
    if (regex_match(s, match, optregex)) {
      options[string(match[1])] = match[2];
    } else {
      cerr << "Cannot parse option: " << *opt << endl;
      options["help"] = "";
    }
  }

  if (options.count("help")) {
    cerr <<
      "    --target=connstr     postgres database to send queries to" << endl <<
#ifdef HAVE_LIBSQLITE3
      "    --sqlite=URI         SQLite database to send queries to" << endl <<
#endif
#ifdef HAVE_LIBMYSQLCLIENT
      "    --mysql=conninfo     MySQL database to send queries to" << endl <<
#endif
      "    --log-to=connstr     log errors to postgres database" << endl <<
      "    --seed=int           seed RNG with specified int instead of PID" << endl <<
      "    --dump-all-graphs    dump generated ASTs" << endl <<
      "    --dry-run            print queries instead of executing them" << endl <<
      "    --exclude-catalog    don't generate queries using catalog relations" << endl <<
      "    --max-queries=long   terminate after generating this many queries" << endl <<
      "    --verbose            emit progress output" << endl <<
      "    --version            print version information and exit" << endl <<
      "    --help               print available command line options and exit" << endl;
    return 0;
  } else if (options.count("version")) {
    return 0;
  }

  try
    {
      shared_ptr<schema> schema;
      if (options.count("sqlite")) {

#ifdef HAVE_LIBSQLITE3
	schema = make_shared<schema_sqlite>(options["sqlite"], options.count("exclude-catalog"));
#else
	cerr << "Sorry, " PACKAGE_NAME " was compiled without SQLite support." << endl;
	return 1;
#endif

      } else if (options.count("mysql")) {

#ifdef HAVE_LIBMYSQLCLIENT
	schema = make_shared<schema_mysql>(options["mysql"]);
#else
	cerr << "Sorry, " PACKAGE_NAME " was compiled without MySQL support." << endl;
	return 1;
#endif

      } else
	schema = make_shared<schema_pqxx>(options["target"], options.count("exclude-catalog"));

      scope scope;
      long queries_generated = 0;
      schema->fill_scope(scope);
//       work w(c);
//       w.commit();

      vector<shared_ptr<logger> > loggers;

      loggers.push_back(make_shared<impedance_feedback>());

      if (options.count("log-to"))
	loggers.push_back(make_shared<pqxx_logger>(
	     options.count("sqlite") ? options["sqlite"] : options["target"],
	     options["log-to"], *schema));

      if (options.count("verbose")) {
	auto l = make_shared<cerr_logger>();
	global_cerr_logger = &*l;
	loggers.push_back(l);
	signal(SIGINT, cerr_log_handler);
      }
      
      if (options.count("dump-all-graphs"))
	loggers.push_back(make_shared<ast_logger>());
      
      smith::rng.seed(options.count("seed") ? stoi(options["seed"]) : getpid());

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

      shared_ptr<dut_base> dut;
      
      if (options.count("sqlite")) {
#ifdef HAVE_LIBSQLITE3
	dut = make_shared<dut_sqlite>(options["sqlite"]);
#else
	cerr << "Sorry, " PACKAGE_NAME " was compiled without SQLite support." << endl;
	return 1;
#endif
      }
      else if (options.count("mysql")) {
#ifdef HAVE_LIBMYSQLCLIENT
	dut = make_shared<dut_mysql>(options["sqlite"]);
#else
	cerr << "Sorry, " PACKAGE_NAME " was compiled without MySQL support." << endl;
	return 1;
#endif
      } else
	dut = make_shared<dut_pqxx>(options["target"]);

      while (1)
      {
	try {
	  while (1) {
	    if (options.count("max-queries")
		&& (++queries_generated > stol(options["max-queries"]))) {
	      if (global_cerr_logger)
		global_cerr_logger->report();
	      return 0;
	    }
	    
	    /* Invoke top-level production to generate AST */
	    shared_ptr<prod> gen = statement_factory(&scope);

	    for (auto l : loggers)
	      l->generated(*gen);
	  
	    /* Generate SQL from AST */
	    ostringstream s;
	    gen->out(s);

	    /* Try to execute it */
	    try {
	      dut->test(s.str());
	      for (auto l : loggers)
		l->executed(*gen);
	    } catch (const dut::failure &e) {
	      for (auto l : loggers)
		try {
		  l->error(*gen, e);
		} catch (runtime_error &e) {
		  cerr << endl << "log failed: " << typeid(*l).name() << ": "
		       << e.what() << endl;
		}
	      if ((dynamic_cast<const dut::broken *>(&e))) {
		/* re-throw to outer loop to recover session. */
		throw;
	      }
	    }
	  }
	}
	catch (const dut::broken &e) {
	  /* Give server some time to recover. */
	  this_thread::sleep_for(milliseconds(1000));
	}
      }
    }
  catch (const exception &e) {
    cerr << endl << "caught exception: " << typeid(e).name() << ": "
	 << e.what() << endl;
    return 1;
  }
}
