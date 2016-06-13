/// @file
/// @brief schema and dut classes for SQLite 3

#ifndef SQLITE_HH
#define SQLITE_HH

extern "C"  {
#include <sqlite3.h>
}

#include "schema.hh"
#include "relmodel.hh"
#include "dut.hh"

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

struct dut_sqlite : dut_base {
  sqlite3 *db;
  char *zErrMsg = 0;
  int rc;
  virtual void test(const std::string &stmt);
  dut_sqlite(std::string &conninfo);
  ~dut_sqlite();
};

#endif
