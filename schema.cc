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

  for (auto &type: types) {
    assert(type);
    for(auto &r: aggregates) {
      if (type->consistent(r.restype))
	      aggregates_returning_type[type].push_back(&r);
    }

    for(auto &r: routines) {
      if (!type->consistent(r.restype))
	continue;
      routines_returning_type[type].push_back(&r);
      if(!r.argtypes.size())
	      parameterless_routines_returning_type[type].push_back(&r);
    }
    
    for (auto &t: tables) {
      for (auto &c: t.columns()) {
	if (type->consistent(c.type)) {
	  tables_with_columns_of_type[type].push_back(&t);
	  break;
	}
      }
    }

    for (auto &concrete: types) {
      if (type->consistent(concrete))
	      concrete_type[type].push_back(concrete);
    }

    for (auto &o: operators) {
      if (type->consistent(o.result))
	      operators_returning_type[type].push_back(&o);
    }
  }

  for (auto &t: tables) {
    if (t.is_base_table)
      base_tables.push_back(&t);
  }
  
  cerr << "done." << endl;

  assert(booltype);
  assert(inttype);
  assert(internaltype);
  assert(arraytype);

}
