#include "redshift.hh"
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

bool rs_type::consistent(sqltype *rvalue)
{
  rs_type *t = dynamic_cast<rs_type*>(rvalue);

  if (!t) {
    cerr << "unknown type: " << rvalue->name  << endl;
    return false;
  }

  switch(typtype_) {
  case 'b': /* base type */
  case 'c': /* composite type */
  case 'd': /* domain */
  case 'r': /* range */
  case 'e': /* enum */
    return this == t;

  case 'p':
    if (name == "anyarray") {
      return t->typelem_ != InvalidOid;
    } else if (name == "anynonarray") {
      return t->typelem_ == InvalidOid;
    } else if(name == "anyenum") {
      return t->typtype_ == 'e';
    } else if (name == "any") {
      return true;
    } else if (name == "anyelement") {
      return t->typelem_ == InvalidOid;
    } else if (name == "anyrange") {
      return t->typtype_ == 'r';
    } else if (name == "record") {
      return t->typtype_ == 'c';
    } else if (name == "cstring") {
      return this == t;
    } else {
      return false;
    }

  default:
    throw std::logic_error("unknown typtype");
  }
}

dut_rspqxx::dut_rspqxx(std::string conninfo)
  : c(conninfo)
{
     c.set_variable("statement_timeout", "'1s'");
     //c.set_variable("client_min_messages", "'ERROR'");
     c.set_variable("application_name", "'" PACKAGE "::dut'");
}

void dut_rspqxx::test(const std::string &stmt)
{
  try {
    if(!c.is_open())
       c.activate();

    pqxx::work w(c);
    w.exec(stmt.c_str());
    w.abort();
  } catch (const pqxx::failure &e) {
    if ((dynamic_cast<const pqxx::broken_connection *>(&e))) {
      /* re-throw to outer loop to recover session. */
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

schema_redshift::schema_redshift(std::string &conninfo, bool no_catalog) : c(conninfo)
{
  c.set_variable("application_name", "'" PACKAGE "::schema'");

  pqxx::work w(c);
  pqxx::result r = w.exec("select version()");
  version = r[0][0].as<string>();

  cerr << "Loading types...";

  r = w.exec("select quote_ident(typname), oid, typdelim, typrelid, typelem, "
	     "COALESCE((SELECT oid from pg_type where typname = '_' || t.typname),0) as typarray, "
             " typtype from pg_type t");

  for (auto row = r.begin(); row != r.end(); ++row) {
    string name(row[0].as<string>());
    OID oid(row[1].as<OID>());
    string typdelim(row[2].as<string>());
    OID typrelid(row[3].as<OID>());
    OID typelem(row[4].as<OID>());
    OID typarray(row[5].as<OID>());
    string typtype(row[6].as<string>());
    //       if (schema == "pg_catalog")
    // 	continue;
    //       if (schema == "information_schema")
    // 	continue;

    rs_type *t = new rs_type(name,oid,typdelim[0],typrelid, typelem, typarray, typtype[0]);
    oid2type[oid] = t;
    name2type[name] = t;
    types.push_back(t);
  }

  booltype = name2type["bool"];
  inttype = name2type["int4"];

  internaltype = name2type["internal"];
  arraytype = name2type["anyarray"];

  cerr << "done." << endl;

  cerr << "Loading tables...";
  r = w.exec("select table_name, "
		    "table_schema, "
	            "(CASE WHEN table_type = 'BASE TABLE' THEN 'YES' ELSE 'NO' END) is_insertable_into, "
	            "table_type "
	     "from information_schema.tables");

  for (auto row = r.begin(); row != r.end(); ++row) {
    string schema(row[1].as<string>());
    string insertable(row[2].as<string>());
    string table_type(row[3].as<string>());

	if (no_catalog && ((schema == "pg_catalog") || (schema == "information_schema")))
		continue;

    tables.push_back(table(row[0].as<string>(),
			   schema,
			   ((insertable == "YES") ? true : false),
			   ((table_type == "BASE TABLE") ? true : false)));
  }

  cerr << "done." << endl;

  cerr << "Loading columns and constraints...";

  for (auto t = tables.begin(); t != tables.end(); ++t) {
    string q("select attname, "
	     "atttypid "
	     "from pg_attribute join pg_class c on( c.oid = attrelid ) "
	     "join pg_namespace n on n.oid = relnamespace "
	     "where not attisdropped "
	     "and attname not in "
	     "('xmin', 'xmax', 'ctid', 'cmin', 'cmax', 'tableoid', 'oid') ");
    q += " and relname = " + w.quote(t->name);
    q += " and nspname = " + w.quote(t->schema);

    r = w.exec(q);
    for (auto row : r) {
      column c(row[0].as<string>(), oid2type[row[1].as<OID>()]);
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

  r = w.exec("select oprname, oprleft,"
		    "oprright, oprresult "
		    "from pg_catalog.pg_operator "
                    "where 0 not in (oprresult, oprright, oprleft) ");
  for (auto row : r) {
    op o(row[0].as<string>(),
	 oid2type[row[1].as<OID>()],
	 oid2type[row[2].as<OID>()],
	 oid2type[row[3].as<OID>()]);
    register_operator(o);
  }

  cerr << "done." << endl;

  cerr << "Loading routines...";
  r = w.exec("select (select nspname from pg_namespace where oid = pronamespace), oid, prorettype, proname "
	     "from pg_proc "
	     "where (select typname from pg_type where oid = prorettype) not in ('event_trigger', 'trigger', 'opaque', 'internal') "
	     "and proname <> 'pg_event_trigger_table_rewrite_reason' "
	     "and proname <> 'pg_event_trigger_table_rewrite_oid' "
	     "and proname !~ '^ri_fkey_' "
	     "and not (proretset or proisagg) ");

  for (auto row : r) {
    routine proc(row[0].as<string>(),
		 row[1].as<string>(),
		 oid2type[row[2].as<long>()],
		 row[3].as<string>());
    register_routine(proc);
  }

  cerr << "done." << endl;

  cerr << "Loading routine parameters...";

  for (auto &proc : routines) {
    string q(" WITH cnt AS ("
	" SELECT 1 AS n UNION ALL SELECT 2 UNION ALL SELECT 3 UNION ALL SELECT 4 UNION ALL "
	" SELECT 5 UNION ALL SELECT 6 UNION ALL SELECT 7 UNION ALL SELECT 8 UNION ALL "
	" SELECT 9 UNION ALL SELECT 10 "
" ), x AS ( SELECT oid, oidvectortypes(proargtypes) oi FROM pg_proc WHERE pronargs > 0 and oid = " + w.quote(proc.specific_name) + ")"
" SELECT"
"  (SELECT oid FROM pg_type WHERE format_type(oid, NULL) = TRIM(SPLIT_PART(oi, ',', cnt.n))) AS oid"
" FROM cnt"
"	INNER JOIN x ON cnt.n <= REGEXP_COUNT(oi, ',') + 1;");

/*"   TRIM(SPLIT_PART(oi, ',', cnt.n)) ," */

    r = w.exec(q);
    for (auto row : r) {
      sqltype *t = oid2type[row[0].as<OID>()];
      assert(t);
      proc.argtypes.push_back(t);
    }
  }
  cerr << "done." << endl;

  cerr << "Loading aggregates...";
  r = w.exec("select (select nspname from pg_namespace where oid = pronamespace), oid, prorettype, proname "
	     "from pg_proc "
	     "where (select typname from pg_type where oid = prorettype) not in ('event_trigger', 'trigger', 'opaque', 'internal') "
	     "and proname not in ('pg_event_trigger_table_rewrite_reason') "
	     "and proname not in ('percentile_cont', 'dense_rank', 'cume_dist', "
	     "'rank', 'test_rank', 'percent_rank', 'percentile_disc', 'mode', 'test_percentile_disc') "
	     "and proname !~ '^ri_fkey_' "
	     "and not proretset "
	     "and proisagg");

  for (auto row : r) {
    routine proc(row[0].as<string>(),
		 row[1].as<string>(),
		 oid2type[row[2].as<OID>()],
		 row[3].as<string>());
    register_aggregate(proc);
  }

  cerr << "done." << endl;

  cerr << "Loading aggregate parameters...";

  for (auto &proc : aggregates) {
    string q(" WITH cnt AS ("
	" SELECT 1 AS n UNION ALL SELECT 2 UNION ALL SELECT 3 UNION ALL SELECT 4 UNION ALL "
	" SELECT 5 UNION ALL SELECT 6 UNION ALL SELECT 7 UNION ALL SELECT 8 UNION ALL "
	" SELECT 9 UNION ALL SELECT 10 "
" ), x AS ( SELECT oid, oidvectortypes(proargtypes) oi FROM pg_proc WHERE pronargs > 0 and oid = " + w.quote(proc.specific_name) + ")"
" SELECT"
"  (SELECT oid FROM pg_type WHERE format_type(oid, NULL) = TRIM(SPLIT_PART(oi, ',', cnt.n))) AS oid"
" FROM cnt"
"	INNER JOIN x ON cnt.n <= REGEXP_COUNT(oi, ',') + 1;");

    r = w.exec(q);
    for (auto row : r) {
      sqltype *t = oid2type[row[0].as<OID>()];
      assert(t);
      proc.argtypes.push_back(t);
    }
  }
  cerr << "done." << endl;
  c.disconnect();
  generate_indexes();
}

extern "C" {
    void dut_libpq_rsnotice_rx(void *arg, const PGresult *res);
}

void dut_libpq_rsnotice_rx(void *arg, const PGresult *res)
{
    (void) arg;
    (void) res;
}

void dut_redshift::connect(std::string &conninfo)
{
    if (conn) {
	PQfinish(conn);
    }
    conn = PQconnectdb(conninfo.c_str());
    char *errmsg = PQerrorMessage(conn);
    if (strlen(errmsg))
	 throw dut::broken(errmsg, "08001");

    command("set statement_timeout to '10'");
//    command("set client_min_messages to 'ERROR';");
    command("set application_name to '" PACKAGE "::dut';");

    PQsetNoticeReceiver(conn, dut_libpq_rsnotice_rx, (void *) 0);
}

dut_redshift::dut_redshift(std::string conninfo)
    : conninfo_(conninfo)
{
    connect(conninfo);
}

void dut_redshift::command(const std::string &stmt)
{
    if (!conn)
	connect(conninfo_);
    PGresult *res = PQexec(conn, stmt.c_str());

    switch (PQresultStatus(res)) {

    case PGRES_FATAL_ERROR:
    default:
    {
	const char *errmsg = PQresultErrorMessage(res);
	if (!errmsg || !strlen(errmsg))
	     errmsg = PQerrorMessage(conn);

	const char *sqlstate = PQresultErrorField(res, PG_DIAG_SQLSTATE);
	if (!sqlstate || !strlen(sqlstate))
	     sqlstate =  (CONNECTION_OK != PQstatus(conn)) ? "08000" : "?????";

	std::string error_string(errmsg);
	std::string sqlstate_string(sqlstate);
	PQclear(res);

	if (CONNECTION_OK != PQstatus(conn)) {
            PQfinish(conn);
	    conn = 0;
	    throw dut::broken(error_string.c_str(), sqlstate_string.c_str());
	}
	if (sqlstate_string == "42601")
	     throw dut::syntax(error_string.c_str(), sqlstate_string.c_str());
	else
	     throw dut::failure(error_string.c_str(), sqlstate_string.c_str());
    }

    case PGRES_NONFATAL_ERROR:
    case PGRES_TUPLES_OK:
    case PGRES_SINGLE_TUPLE:
    case PGRES_COMMAND_OK:
	PQclear(res);
	return;
    }
}

void dut_redshift::test(const std::string &stmt)
{
    command("ROLLBACK;");
    command("BEGIN;");
    command(stmt.c_str());
    command("ROLLBACK;");
}
