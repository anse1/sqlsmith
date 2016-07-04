#include <typeinfo>
#include "config.h"
#include "schema.hh"
#include "relmodel.hh"
#include <pqxx/pqxx>
#include "gitrev.h"

using namespace std;
using namespace pqxx;

void schema::generate_indexes() {
  cerr << "Generating indexes...";

  for(auto &r: routines) {
    routines_returning_type.insert(pair<sqltype*, routine*>(r.restype, &r));
    if(!r.argtypes.size())
      parameterless_routines_returning_type.insert(pair<sqltype*, routine*>(r.restype, &r));
  }

  for(auto &r: aggregates) {
    aggregates_returning_type.insert(pair<sqltype*, routine*>(r.restype, &r));
  }

  for (auto &type: types) {
    for (auto &t: tables) {
      for (auto &c: t.columns()) {
	if (type->consistent(c.type)) 
	  tables_with_columns_of_type.insert(pair<sqltype*, table*>(type, &t));
      }
    }

    for (auto &concrete: types) {
      if (type->consistent(concrete))
	concrete_type.insert(pair<sqltype*, sqltype*>(type, concrete));
    }
  }

  for (auto &o: operators) {
    operators_returning_type.insert(pair<sqltype*, op*>(o.result, &o));
  }
  cerr << "done." << endl;

  assert(booltype);
  assert(inttype);
  assert(internaltype);
  assert(arraytype);
}
