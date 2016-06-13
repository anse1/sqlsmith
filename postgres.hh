/// @file
/// @brief schema and dut classes for PostgreSQL

#ifndef POSTGRES_HH
#define POSTGRES_HH

#include "dut.hh"
#include "relmodel.hh"
#include "schema.hh"

#include <pqxx/pqxx>

struct schema_pqxx : public schema {
  pqxx::connection c;
  virtual std::string quote_name(const std::string &id) {
    return c.quote_name(id);
  }
  schema_pqxx(std::string &conninfo);
};

struct dut_pqxx : dut_base {
  pqxx::connection c;
  bool reset_gucs = true;
  virtual void test(const std::string &stmt);
  dut_pqxx(std::string conninfo);
};

#endif
