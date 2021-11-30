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

struct sqlite_connection {
  sqlite3 *db;
  char *zErrMsg = 0;
  int rc;
  void q(const char *query);
  sqlite_connection(std::string &conninfo);
  ~sqlite_connection();
};

struct schema_sqlite : schema, sqlite_connection {
  schema_sqlite(std::string &conninfo, bool no_catalog);
  std::string quote_name(const std::string &id) override {
    return id;
  }
};

struct dut_sqlite : dut_base, sqlite_connection {
  void test(const std::string &stmt) override;
  dut_sqlite(std::string &conninfo);
};

#endif
