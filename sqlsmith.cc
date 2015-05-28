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

      while (1) {
	  work w(c);
	  w.exec("set statement_timeout to '2s';");
	  query_spec gen(scope);
	  std::ostringstream s;
	  gen.out(s);
	  cout << s.str() << endl;
	  try {
	    result r = w.exec(s.str() + ";");
	    w.commit();
	  } catch (const pqxx::sql_error &e) {
	    cerr << e.what() << endl;
	    cerr << s.str() << endl;
	  }
      }
    }
  catch (const std::exception &e)
    {
      cerr << e.what() << endl;
      return 1;
    }
}
