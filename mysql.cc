#include <string>
#include <iostream>
#include <stdexcept>
#include <regex>

#include "mysql.hh"

using namespace std;

static regex e_syntax("You have an error in your SQL syntax.*");

mysql_connection::mysql_connection(const std::string &conninfo)
{
  (void) conninfo;
  con = mysql_init(NULL);
  if (!mysql_real_connect(con, "localhost", "root", "qernqmysql", 
			 "regression", 0, NULL, 0)) {
    throw runtime_error(mysql_error(con));
  }
}

void mysql_connection::q(std::string s)
{
  if (mysql_query(con, s.c_str()))
    error();
} 

void mysql_connection::error()
{
  throw runtime_error(mysql_error(con));
} 

mysql_connection::~mysql_connection()
{
  mysql_close(con);
}

schema_mysql::schema_mysql(const std::string &conninfo)
  : mysql_connection(conninfo)
{
  cerr << "MySQL client version: " << mysql_get_client_info() << endl;


  {
    q("select version();");
    MYSQL_RES *result = mysql_store_result(con);
    if (!result)
      error();
    int num_fields = mysql_num_fields(result);
    MYSQL_ROW row = mysql_fetch_row(result);

    if (num_fields != 1 || !row)
      throw runtime_error("unexpected version() result");
    version = row[0];
    mysql_free_result(result);

    cerr << "MySQL server version: " << version << endl;

  }

  {
    cerr << "Loading tables...";
    q("select table_name, table_schema, table_type"
      " from information_schema.tables;");

    MYSQL_RES *result = mysql_store_result(con);
    MYSQL_ROW row;
    string basetab("BASE TABLE");
  
    while ((row = mysql_fetch_row(result))) {
      if (!row[0] || !row[1] || !row[2])
	throw std::runtime_error("broken schema");

      table tab(row[0], row[1],
		false /* insertable */,
		basetab == row[2] /* base_table */);

      tables.push_back(tab);
    }
    mysql_free_result(result);
    cerr << "done." << endl;
  }

  {
    cerr << "Loading columns...";

    for (auto t = tables.begin(); t != tables.end(); ++t) {
      string qs("select column_name, data_type"
		" from information_schema.columns where"
		" table_catalog = 'def'");
      qs += " and table_schema = '" + t->schema + "'";
      qs += " and table_name = '" + t->name + "'";
      qs += ";";

      q(qs);

      MYSQL_RES *result = mysql_store_result(con);
      MYSQL_ROW row;
  
      while ((row = mysql_fetch_row(result))) {
	if (!row[0] || !row[1])
	  throw std::runtime_error("broken schema");

	column col(row[0], sqltype::get(row[1]));
	t->columns().push_back(col);
      }
      mysql_free_result(result);

    }
    cerr << "done." << endl;
  }


  for (auto t : sqltype::typemap) {
    cerr << "type found: " << t.second->name << endl;
  }

  booltype = sqltype::get("tinyint");
  inttype = sqltype::get("int");

  internaltype = sqltype::get("internal");
  arraytype = sqltype::get("ARRAY");


}
dut_mysql::dut_mysql(const std::string &conninfo)
  : mysql_connection(conninfo)
{

}

void dut_mysql::test(const std::string &stmt)
{
  if (mysql_query(con, stmt.c_str())) {
    const char *msg = mysql_error(con);
    int myerrno = mysql_errno(con);

    switch(myerrno) {
    case 1149:
    case 1064:
      throw dut::syntax("Syntax error");
      break;
    case 2006:
      throw dut::broken(mysql_error(con));
    default:
      if (regex_match(msg, e_syntax))
	throw dut::syntax("Syntax error");
      else
	throw dut::failure(mysql_error(con));
    }
  }
  MYSQL_RES *result = mysql_store_result(con);
  mysql_free_result(result);    
}
