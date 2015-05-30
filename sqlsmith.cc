#include <iostream>
#include <pqxx/pqxx>
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
  cerr << "sqlsmith " << GITREV << endl;

  smith::rng.seed(getpid());
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
	w.exec("set statement_timeout to '1s';"
	       "set client_min_messages to 'ERROR';"
	       "set lc_messages to 'C';");
	w.commit();
      }
      int query_count = 0;
      milliseconds query_time(0);
      milliseconds gen_time(0);

      std::map<std::string, long> errors;
      while (1) {
	  work w(c);

	  auto g0 = high_resolution_clock::now();
	  //	  prepare_stmt gen(scope);
	  query_spec gen(scope);
	  std::ostringstream s;
	  gen.out(s);

	  auto g1 = high_resolution_clock::now();
	  gen_time += duration_cast<milliseconds>(g1-g0);
	  
	  cout << s.str() << endl;
	  query_count++;
	  if (79 == query_count%80)
	    cerr << endl;
	  try {
	    auto q0 = high_resolution_clock::now();
	    result r = w.exec(s.str() + ";");
	    auto q1 = high_resolution_clock::now();
	    query_time +=  duration_cast<milliseconds>(q1-q0);
	    w.abort();
	    cerr << ".";
	  } catch (const pqxx::sql_error &e) {
	    istringstream err(e.what());
	    string line;
	    getline(err, line);
	    errors[line]++;
	    cerr << (regex_match(e.what(), timeout) ? "t" : "e");
	  }
	  if (0 == query_count%1000) {
	    cerr << endl << "queries: " << query_count;
 	    cerr << " (" << 1000.0*query_count/gen_time.count() << " gen/s, ";
 	    cerr << 1000.0*query_count/query_time.count() << " exec/s)" << endl;
	    int error_count = 0;
	    vector<pair<std::string, long> > report;
	    for (auto e : errors) {
	      report.push_back(e);
	    }
	    stable_sort(report.begin(), report.end(),
			[](const pair<std::string, long> &a,
			   const pair<std::string, long> &b)
			{ return a.second > b.second; });
	    for (auto e : report) {
	      error_count += e.second;
	      cerr << e.second << "\t" << e.first << endl;
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
