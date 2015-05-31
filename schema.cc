#include "schema.hh"
#include "relmodel.hh"
#include <pqxx/pqxx>

using namespace std;
using namespace pqxx;

schema_pqxx::schema_pqxx() {
  connection c;
  work w(c);
  result r = w.exec("select table_catalog, "
		    "table_name, "
		    "table_schema from information_schema.tables;");

  for (auto row = r.begin(); row != r.end(); ++row) {
    string schema(row[2].as<string>());
    //       if (schema == "pg_catalog")
    // 	continue;
    //       if (schema == "information_schema")
    // 	continue;
      
    tables.push_back(table(row[0].as<string>(),
			   row[1].as<string>(),
			   schema));
  }

  for (auto t = tables.begin(); t != tables.end(); ++t) {
    string q("select column_name, "
	     "udt_name"
	     " from information_schema.columns where");
    q += " table_catalog = " + w.quote(t->catalog);
    q += " and table_schema = " + w.quote(t->schema);
    q += " and table_name = " + w.quote(t->name);
    q += ";";

    r = w.exec(q);
    for (auto row : r) {
      column c(row[0].as<string>());
      c.type = row[1].as<string>();
      t->columns.push_back(c);
    }
  }

  r = w.exec("select oprname, oprleft::regtype,"
		    "oprright::regtype, oprresult::regtype "
		    "from pg_catalog.pg_operator;");
  for (auto row : r) {
    op o(row[0].as<string>(),
	 row[1].as<string>(),
	 row[2].as<string>(),
	 row[3].as<string>());
    register_operator(o);
  }
}

schema_pqxx schema;
