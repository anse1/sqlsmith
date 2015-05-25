#include <iostream>
#include <pqxx/pqxx>
#include <cstdlib>
#include <numeric>

#include "random.hh"
#include "grammar.hh"
#include "relmodel.hh"

using namespace std;
using namespace pqxx;



int main()
{
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
	  cout << gen.str() << endl;
	  try {
	    result r = w.exec(gen.str() + ";");
	    w.commit();
	  } catch (const pqxx::sql_error &e) {
	    cerr << e.what() << endl;
	    cerr << gen.str() << endl;
	  }
      }
    }
  catch (const std::exception &e)
    {
      cerr << e.what() << endl;
      return 1;
    }
}
