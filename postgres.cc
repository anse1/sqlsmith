#include "postgres.hh"
#include "config.h"
#include <iostream>

#ifndef HAVE_BOOST_REGEX
#include <regex>
#else
#include <boost/regex.hpp>
using boost::regex;
using boost::smatch;
using boost::regex_match;
#endif

using namespace std;

static regex e_timeout("ERROR:  canceling statement due to statement timeout(\n|.)*");
static regex e_syntax("ERROR:  syntax error at or near(\n|.)*");

dut_pqxx::dut_pqxx(std::string conninfo)
  : c(conninfo)
{
  
}

void dut_pqxx::test(const std::string &stmt)
{
  try {

    if (reset_gucs) {

      if(!c.is_open())
	c.activate();

      pqxx::work w(c);

      w.exec("set statement_timeout to '1s';"
	     "set client_min_messages to 'ERROR';"
	     "set application_name to '" PACKAGE "::dut';");
      w.commit();
      reset_gucs = false;
    }

    pqxx::work w(c);
    w.exec(stmt.c_str());
  } catch (const pqxx::failure &e) {
    if ((dynamic_cast<const pqxx::broken_connection *>(&e))) {
      /* re-throw to outer loop to recover session. */
      reset_gucs = true;
      throw dut::broken(e.what());
    }

    if (regex_match(e.what(), e_timeout))
      throw dut::timeout(e.what());
    else if (regex_match(e.what(), e_syntax))
      throw dut::syntax(e.what());
    else
      throw dut::failure(e.what());
  }
}


schema_pqxx::schema_pqxx(std::string &conninfo) : c(conninfo)
{
  pqxx::work w(c);

  w.exec("set application_name to '" PACKAGE "::schema';");
    
  pqxx::result r = w.exec("select version()");
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
    q += " and relnamespace = " " (select oid from pg_namespace where nspname = " + w.quote(t->schema) + ")";
    q += " and relname = " + w.quote(t->name);

    for (auto row : w.exec(q)) {
      t->constraints.push_back(row[0].as<string>());
    }
    
  }
  cerr << "done." << endl;

  cerr << "Loading operators...";

  r = w.exec("select oprname, oprleft::regtype,"
		    "oprright::regtype, oprresult::regtype "
		    "from pg_catalog.pg_operator "
                    "where 0 not in (oprresult, oprright, oprleft) ");
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
  r = w.exec("select (select nspname from pg_namespace where oid = pronamespace), oid, prorettype::regtype, proname "
	     "from pg_proc "
	     "where prorettype::regtype::text not in ('event_trigger', 'trigger', 'opaque', 'internal') "
	     "and proname <> 'pg_event_trigger_table_rewrite_reason' "
	     "and proname <> 'pg_event_trigger_table_rewrite_oid' "
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
  r = w.exec("select (select nspname from pg_namespace where oid = pronamespace), oid, prorettype::regtype, proname "
	     "from pg_proc "
	     "where prorettype::regtype::text not in ('event_trigger', 'trigger', 'opaque', 'internal') "
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
  c.disconnect();
  generate_indexes();
}
