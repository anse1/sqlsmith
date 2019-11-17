/// @file
/// @brief schema and dut classes for Redshift (based on PostgreSQL v8.0.2)

#ifndef REDSHIFT_HH
#define REDSHIFT_HH

#include "dut.hh"
#include "relmodel.hh"
#include "schema.hh"

#include <pqxx/pqxx>

extern "C" {
#include <libpq-fe.h>
}

#define OID long

struct rs_type : sqltype {
  OID oid_;
  char typdelim_;
  OID typrelid_;
  OID typelem_;
  OID typarray_;
  char typtype_;
  rs_type(string name,
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


struct schema_redshift : public schema {
  pqxx::connection c;
  map<OID, rs_type*> oid2type;
  map<string, rs_type*> name2type;

  virtual std::string quote_name(const std::string &id) {
    return c.quote_name(id);
  }
  schema_redshift(std::string &conninfo, bool no_catalog);
};

struct dut_rspqxx : dut_base {
  pqxx::connection c;
  virtual void test(const std::string &stmt);
  dut_rspqxx(std::string conninfo);
};

struct dut_redshift : dut_base {
     PGconn *conn = 0;
     std::string conninfo_;
     virtual void test(const std::string &stmt);
     void command(const std::string &stmt);
     void connect(std::string &conninfo);
     dut_redshift(std::string conninfo);
};
#endif
