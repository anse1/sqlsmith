#ifndef SQLITE_HH
#define SQLITE_HH

extern "C"  {
#include <sqlite3.h>
}

#include "schema.hh"
#include "relmodel.hh"

struct schema_sqlite : schema {
  sqlite3 *db;
  char *zErrMsg = 0;
  int rc;
  void q(const char *query);
  schema_sqlite(std::string &conninfo);
  ~schema_sqlite();
  virtual std::string quote_name(const std::string &id) {
    return id;
  }
};

#endif
