#include "config.h"
#include "schema.hh"
#include "relmodel.hh"
#include <pqxx/pqxx>
#include "gitrev.h"

using namespace std;
using namespace pqxx;

void schema::generate_indexes() {
  for(auto &r: routines) {
    routines_returning_type.insert(pair<sqltype*, routine*>(r.restype, &r));
    if(!r.argtypes.size())
      parameterless_routines_returning_type.insert(pair<sqltype*, routine*>(r.restype, &r));
  }

  for(auto &r: aggregates) {
    aggregates_returning_type.insert(pair<sqltype*, routine*>(r.restype, &r));
  }

  for (auto &t: tables) {
    for (auto &c: t.columns()) {
      tables_with_columns_of_type.insert(pair<sqltype*, table*>(c.type, &t));
    }
  }
}
