/// @file
/// @brief schema and dut classes for PostgreSQL

#ifndef POSTGRES_HH
#define POSTGRES_HH

#include "dut.hh"
#include "relmodel.hh"
#include "schema.hh"

#include <pqxx/pqxx>

extern "C" {
#include <postgresql/libpq-fe.h>
}

#define OID long

struct pg_type : sqltype {
  OID oid_;
  char typdelim_;
  OID typrelid_;
  OID typelem_;
  OID typarray_;
  char typtype_;
  pg_type(string name,
	  OID oid,
	  char typdelim,
	  OID typrelid,
	  OID typelem,
	  OID typarray,
	  char typtype)
    : sqltype(name), oid_(oid), typdelim_(typdelim), typrelid_(typrelid),
      typelem_(typelem), typarray_(typarray), typtype_(typtype) { }

  virtual bool consistent(struct sqltype *rvalue);
  bool consistent_(sqltype *rvalue);
};


struct schema_pqxx : public schema {
  pqxx::connection c;
  map<OID, pg_type*> oid2type;
  map<string, pg_type*> name2type;

  virtual std::string quote_name(const std::string &id) {
    return c.quote_name(id);
  }
  schema_pqxx(std::string &conninfo, bool no_catalog);
};

struct dut_pqxx : dut_base {
  pqxx::connection c;
  virtual void test(const std::string &stmt);
  dut_pqxx(std::string conninfo);
};

struct dut_libpq : dut_base {
     PGconn *conn = 0;
     std::string conninfo_;
     virtual void test(const std::string &stmt);
     void command(const std::string &stmt);
     void connect(std::string &conninfo);
     dut_libpq(std::string conninfo);
};

#endif
