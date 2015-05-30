#include <iostream>
#include <pqxx/pqxx>
#include <cstdlib>
#include <numeric>

#include "random.hh"
#include "grammar.hh"
#include "relmodel.hh"
#include "schema.hh"

using namespace std;
using namespace pqxx;

extern "C" {
#include <unistd.h>
}

int main()
{
  srand(getpid()*time(0));
  srandom(getpid()*time(0));
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
	w.exec("set statement_timeout to '2s';");
	w.exec("set client_min_messages to 'ERROR';");
	w.commit();
      }
      int query_count = 0;
      std::map<std::string, long> errors;
      while (1) {
	  work w(c);
	  cerr << ".";
	  query_spec gen(scope);
	  std::ostringstream s;
	  gen.out(s);
	  cout << s.str() << endl;
	  query_count++;
	  try {
	    result r = w.exec(s.str() + ";");
	    w.commit();
	    cerr << ".";
	  } catch (const pqxx::sql_error &e) {
	    errors[e.what()]++;
	    cerr << "e";
	  }

	  if (0 == query_count%1000) {
	    cerr << endl << "queries generated: " << query_count << endl;
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
