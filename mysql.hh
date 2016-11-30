#ifndef MYSQL_HH
#define MYSQL_HH

#include <string>

#include "schema.hh"
#include "relmodel.hh"
#include "dut.hh"

extern "C" {
// #define bool_defined
// #include <my_global.h>
#include <mysql/mysql.h>
}

struct mysql_connection {
  MYSQL *con;
  mysql_connection(const std::string &conninfo);
  virtual ~mysql_connection();
  void q(std::string s);
  void error();
};

struct schema_mysql : schema, mysql_connection {
  schema_mysql(const std::string &conninfo);
  virtual std::string quote_name(const std::string &id) {
    return id;
  }
};

struct dut_mysql : dut_base, mysql_connection {
  virtual void test(const std::string &stmt);
  dut_mysql(const std::string &conninfo);
};

#endif
