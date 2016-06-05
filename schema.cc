#include "config.h"
#include "schema.hh"
#include "relmodel.hh"
#include <pqxx/pqxx>
#include "gitrev.h"

using namespace std;
using namespace pqxx;

schema_pqxx::schema_pqxx(std::string &conninfo) : c(conninfo)
{
  work w(c);

  w.exec("set application_name to '" PACKAGE "::schema';");
    
  result r = w.exec("select version()");
  version = r[0][0].as<string>();
  
  cerr << "Loading tables...";
  r = w.exec("select table_name, "
		    "table_schema, "
	            "is_insertable_into, "
	            "table_type "
	     "from information_schema.tables");
	     
  for (auto row = r.begin(); row != r.end(); ++row) {
    string schema(row[1].as<string>());
    string insertable(row[2].as<string>());
    string table_type(row[3].as<string>());
    //       if (schema == "pg_catalog")
    // 	continue;
    //       if (schema == "information_schema")
    // 	continue;
      
    tables.push_back(table(row[0].as<string>(),
			   schema,
			   ((insertable == "YES") ? true : false),
			   ((table_type == "BASE TABLE") ? true : false)));
  }
	     
  cerr << "done." << endl;

  cerr << "Loading columns and constraints...";

  for (auto t = tables.begin(); t != tables.end(); ++t) {
    string q("select column_name, "
	     "udt_name"
	     " from information_schema.columns where"
	     " table_catalog = current_catalog");
    q += " and table_schema = " + w.quote(t->schema);
    q += " and table_name = " + w.quote(t->name);
    q += ";";

    r = w.exec(q);
    for (auto row : r) {
      column c(row[0].as<string>(), row[1].as<string>());
      t->columns().push_back(c);
    }

    q = "select conname from pg_class t "
      "join pg_constraint c on (t.oid = c.conrelid) "
      "where contype in ('f', 'u', 'p') ";
    q += " and relnamespace = " + w.quote(t->schema) + "::regnamespace ";
    q += " and relname = " + w.quote(t->name);

    for (auto row : w.exec(q)) {
      t->constraints.push_back(row[0].as<string>());
    }
    
  }
  cerr << "done." << endl;

  cerr << "Loading operators...";

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

  cerr << "done." << endl;

  booltype = sqltype::get("boolean");
  inttype = sqltype::get("integer");

  internaltype = sqltype::get("internal");
  arraytype = sqltype::get("ARRAY");

  cerr << "Loading routines...";
  r = w.exec("select pronamespace::regnamespace, oid, prorettype::regtype, proname "
	     "from pg_proc "
	     "where prorettype::regtype not in ('event_trigger', 'trigger', 'opaque', 'internal') "
	     "and proname not in ('pg_event_trigger_table_rewrite_reason') "
	     "and proname !~ '^ri_fkey_' "
	     "and not (proretset or proisagg or proiswindow) ");

  for (auto row : r) {
    routine proc(row[0].as<string>(),
		 row[1].as<string>(),
		 sqltype::get(row[2].as<string>()),
		 row[3].as<string>());
    register_routine(proc);
  }

  cerr << "done." << endl;

  cerr << "Loading routine parameters...";

  for (auto &proc : routines) {
    string q("select unnest(proargtypes)::regtype "
	     "from pg_proc ");
    q += " where oid = " + w.quote(proc.specific_name);
      
    r = w.exec(q);
    for (auto row : r) {
      proc.argtypes.push_back(sqltype::get(row[0].as<string>()));
    }
  }
  cerr << "done." << endl;

  cerr << "Loading aggregates...";
  r = w.exec("select pronamespace::regnamespace, oid, prorettype::regtype, proname "
	     "from pg_proc "
	     "where prorettype::regtype not in ('event_trigger', 'trigger', 'opaque', 'internal') "
	     "and proname not in ('pg_event_trigger_table_rewrite_reason') "
	     "and proname not in ('percentile_cont', 'dense_rank', 'cume_dist', "
	     "'rank', 'test_rank', 'percent_rank', 'percentile_disc', 'mode', 'test_percentile_disc') "
	     "and proname !~ '^ri_fkey_' "
	     "and not (proretset or proiswindow) "
	     "and proisagg");

  for (auto row : r) {
    routine proc(row[0].as<string>(),
		 row[1].as<string>(),
		 sqltype::get(row[2].as<string>()),
		 row[3].as<string>());
    register_aggregate(proc);
  }

  cerr << "done." << endl;

  cerr << "Loading aggregate parameters...";

  for (auto &proc : aggregates) {
    string q("select unnest(proargtypes)::regtype "
	     "from pg_proc ");
    q += " where oid = " + w.quote(proc.specific_name);
      
    r = w.exec(q);
    for (auto row : r) {
      proc.argtypes.push_back(sqltype::get(row[0].as<string>()));
    }
  }
  cerr << "done." << endl;


  cerr << "Generating indexes...";
  generate_indexes();
  cerr << "done." << endl;

}


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
