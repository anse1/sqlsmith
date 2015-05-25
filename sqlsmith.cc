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
	query_spec gen(scope);
	cout << gen.to_str() << endl;
	result r = w.exec(gen.to_str() + ";");
	w.commit();
      }
    }
  catch (const std::exception &e)
    {
      cerr << e.what() << endl;
      return 1;
    }
}
