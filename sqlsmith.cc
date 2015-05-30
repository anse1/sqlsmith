#include <iostream>
#include <pqxx/pqxx>
#include <cstdlib>
#include <numeric>
#include <chrono>
#include <regex>

#include "random.hh"
#include "grammar.hh"
#include "relmodel.hh"
#include "schema.hh"

#include "config.h"

using namespace std;
using namespace pqxx;

using namespace std::chrono;

extern "C" {
#include <unistd.h>
}

regex timeout("ERROR:  canceling statement due to statement timeout(\n|.)*");

int main()
{
  srand(getpid()*time(0));
  srandom(getpid()*time(0));

  cerr << "sqlsmith " << GITREV << endl;

  try
    {
      connection c;
      schema.summary();
      scope scope;
      schema.fill_scope(scope);
      work w(c);
      w.commit();

      {
	work w(c);
	w.exec("set statement_timeout to '1s';");
	w.exec("set client_min_messages to 'ERROR';");
	w.exec("set lc_messages to 'C';");
	w.commit();
      }
      int query_count = 0;
      milliseconds query_time(0);
      milliseconds gen_time(0);

      std::map<std::string, long> errors;
      while (1) {
	  work w(c);
	  cerr << ".";

	  auto g0 = high_resolution_clock::now();
	  //	  prepare_stmt gen(scope);
	  query_spec gen(scope);
	  std::ostringstream s;
	  gen.out(s);

	  auto g1 = high_resolution_clock::now();
	  gen_time += duration_cast<milliseconds>(g1-g0);
	  
	  cout << s.str() << endl;
	  query_count++;
	  try {
	    auto q0 = high_resolution_clock::now();
	    result r = w.exec(s.str() + ";");
	    auto q1 = high_resolution_clock::now();
	    query_time +=  duration_cast<milliseconds>(q1-q0);
	    w.commit();
	    cerr << ".";
	  } catch (const pqxx::sql_error &e) {
	    errors[e.what()]++;
	    cerr << (regex_match(e.what(), timeout) ? "t" : "e");
	  }

	  if (0 == query_count%1000) {
	    cerr << endl << "queries: " << query_count;
 	    cerr << " (" << 1000.0*query_count/gen_time.count() << " gen/s, ";
 	    cerr << 1000.0*query_count/query_time.count() << " exec/s)" << endl;
	    int error_count = 0;
	    for (auto e : errors) {
	      cerr << e.second << "\t" << e.first;
	      error_count += e.second;
	    }
	    cerr << "error rate: " << (float)error_count/query_count << endl;
	  }
      }
    }
  catch (const std::exception &e)
    {
      cerr << e.what() << endl;
      return 1;
    }
}
