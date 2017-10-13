#include <stdexcept>
#include <cassert>
#include "monetdb.hh"
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

static regex e_syntax("near \".*\": syntax error");
static regex e_user_abort("callback requested query abort");

extern "C" {
#include <mapi.h>
#include <unistd.h>
}

// connect montetdb
monetdb_connection::monetdb_connection(std::string &conninfo)
{
	dbh = mapi_mapiuri(conninfo.c_str(), "monetdb", "monetdb", "sql");
	if (mapi_error(dbh)) {
		if (dbh != NULL) {
                         mapi_explain(dbh, stderr);
                         mapi_destroy(dbh);
                } else {
                         fprintf(stderr, "command failed\n");
                }
                exit(-1);
        }
	mapi_reconnect(dbh);
	if (mapi_error(dbh)) {
		mapi_explain(dbh, stderr);
		mapi_destroy(dbh);
                exit(-1);
        }
}

// execute queries on MonetDB
void monetdb_connection::q(const char* query)
{
	MapiHdl hdl = mapi_query(dbh, query);
	if (mapi_result_error(hdl) != NULL)
		mapi_explain_result(hdl, stderr);
	mapi_close_handle(hdl);
}

// disconnect MonetDB
monetdb_connection::~monetdb_connection()
{
	mapi_destroy(dbh);
}

//load schema from MonetDB
schema_monetdb::schema_monetdb(std::string &conninfo):monetdb_connection(conninfo)
{


	cerr << "init booltype, inttype, internaltype, arraytype here" << endl;
	booltype = sqltype::get("boolean");
	inttype = sqltype::get("int");
	internaltype = sqltype::get("internal");
	arraytype = sqltype::get("ARRAY");

	cerr << "Loading tables from database: " << conninfo << endl;
//	string qry = "select t.name, s.name, t.system, t.type from sys.tables t,  sys.schemas s where t.schema_id=s.id and t.system=false";
	string qry = "select t.name, s.name, t.system, t.type from sys.tables t,  sys.schemas s where t.schema_id=s.id ";
	MapiHdl hdl = mapi_query(dbh,qry.c_str());
	while (mapi_fetch_row(hdl)) {
		cerr << ".";
		tables.push_back(table(mapi_fetch_field(hdl,0),mapi_fetch_field(hdl,1),strcmp(mapi_fetch_field(hdl,2),"false")==0 ? true : false , atoi(mapi_fetch_field(hdl,3))==0 ? false : true));
	}
	mapi_close_handle(hdl);
	cerr << " done." << endl;

	cerr << "Loading columns and constraints...";
	for (auto t = tables.begin(); t!=tables.end(); t++) {
		string q("select col.name,"
			" col.type "
			" from sys.columns col, sys.tables tab"
			" where tab.name= '");
		q += t->name;
		q += "' and tab.id = col.table_id";
		cerr << ".";

		hdl = mapi_query(dbh,q.c_str());
		while (mapi_fetch_row(hdl)) {
			cerr << ".";
			column c(mapi_fetch_field(hdl,0), sqltype::get(mapi_fetch_field(hdl,1)));
			t->columns().push_back(c);
		}
		mapi_close_handle(hdl);
	}
	// TODO: confirm with Martin or Stefan about column
	// constraints in MonetDB
	cerr << " done." << endl;

	cerr << "Loading operators...";
	string opq("select f.func, a.type, b.type, c.type"
		" from sys.functions f, sys.args a, sys.args b, sys.args c"
                "  where f.id=a.func_id and f.id=b.func_id and f.id=c.func_id and a.name='arg_1' and b.name='arg_2' and c.number=0");
	hdl = mapi_query(dbh,opq.c_str());
	while (mapi_fetch_row(hdl)) {
		cerr << ".";
		op o(mapi_fetch_field(hdl,0),sqltype::get(mapi_fetch_field(hdl,1)),sqltype::get(mapi_fetch_field(hdl,2)),sqltype::get(mapi_fetch_field(hdl,3)));
		register_operator(o);
	}
	mapi_close_handle(hdl);
	cerr << " done." << endl;


	cerr << "Loading routines...";
	string routq("select s.name, f.id, a.type, f.name from sys.schemas s, sys.args a, sys.types t, sys.functions f where f.schema_id = s.id and f.id=a.func_id and a.number=0 and a.type = t.sqlname and f.mod<>'aggr'");
	hdl = mapi_query(dbh,routq.c_str());
	while (mapi_fetch_row(hdl)) {
		cerr << ".";
		routine proc(mapi_fetch_field(hdl,0),mapi_fetch_field(hdl,1),sqltype::get(mapi_fetch_field(hdl,2)),mapi_fetch_field(hdl,3));
		register_routine(proc);
	}
	mapi_close_handle(hdl);
	cerr << " done." << endl;

	cerr << "Loading routine parameters...";
	for (auto &proc : routines) {
		string routpq ("select a.type from sys.args a,"
			       " sys.functions f "
			       " where f.id = a.func_id and a.number <> 0 and f.id = '");
		routpq += proc.specific_name;
		routpq += "'";
		hdl = mapi_query(dbh,routpq.c_str());
		while (mapi_fetch_row(hdl)) {
			proc.argtypes.push_back(sqltype::get(mapi_fetch_field(hdl,0)));
			cerr<<".";
		}
		mapi_close_handle(hdl);
	}
	cerr << " done."<< endl;



	cerr << "Loading aggregates...";
	string aggq("select s.name, f.id, a.type, f.name from sys.schemas s, sys.args a, sys.types t, sys.functions f where f.schema_id = s.id and f.id=a.func_id and a.number=0 and a.type = t.sqlname and f.mod='aggr'");

	hdl = mapi_query(dbh,aggq.c_str());
	while (mapi_fetch_row(hdl)) {
		cerr << ".";
		routine proc(mapi_fetch_field(hdl,0),mapi_fetch_field(hdl,1),sqltype::get(mapi_fetch_field(hdl,2)),mapi_fetch_field(hdl,3));
		register_aggregate(proc);
	}
	mapi_close_handle(hdl);
	cerr << " done." << endl;

	cerr << "Loading aggregates parameters...";
	for (auto &proc: aggregates) {
		string aggpq ("select a.type from sys.args a, sys.functions f "
			      "where f.id = a.func_id and a.number <> 0 and f.id = '");
		aggpq += proc.specific_name;
		aggpq += "'";
		hdl = mapi_query(dbh,aggpq.c_str());
		while (mapi_fetch_row(hdl)) {
			proc.argtypes.push_back(sqltype::get(mapi_fetch_field(hdl,0)));
			cerr<<".";
		}
		mapi_close_handle(hdl);
	}
	cerr << " done."<< endl;

	mapi_destroy(dbh);
	generate_indexes();

//	cerr << "print loaded information to check correctness" << endl;
//	cerr << "Loaded tables.... " << endl;
/*  	for (auto item : tables) {
		cerr << item.name << "; " << item.schema << "; " << item.is_insertable << "; " << item.is_base_table << endl;
  	}
*/
//	cerr << "Loaded columns... " << endl;
/*  	for (auto tab : tables) {
		for (auto col: tab.columns())
			cerr << tab.name << "; " << col.name << "; "<<col.type->name << endl;
	}
*/
//	cerr << "Loaded aggregates and parameters... " << endl;
/* 	for (auto &proc : aggregates) {
		cerr << proc.specific_name << "; " << proc.schema << "; " << proc.name <<"; " << proc.restype->name ;
		for (auto item : proc.argtypes)
			cerr << "; " << item->name;
		cerr << endl;
 	}
*/
}

dut_monetdb::dut_monetdb(std::string &conninfo):monetdb_connection(conninfo)
{
	//build connection
}

void dut_monetdb::test(const std::string &stmt)
{
	MapiHdl hdl = mapi_query(dbh,"CALL sys.settimeout(1)");
	mapi_close_handle(hdl);
	cerr << stmt << endl;
	hdl = mapi_query(dbh,stmt.c_str());
	if (mapi_error(dbh)!=MOK) {
		try {
			if (mapi_error(dbh)==MERROR) {
			    mapi_explain_result(hdl, stdout);
			    mapi_close_handle(hdl);
			    throw dut::syntax("e");
			} else if (mapi_error(dbh)==MTIMEOUT) {
				mapi_explain_result(hdl, stdout);
				mapi_close_handle(hdl);
				throw dut::timeout("t");
			} else {
				mapi_explain_result(hdl, stdout);
				mapi_close_handle(hdl);
				throw dut::failure("er");
			}
		} catch (dut::failure &e) {
			throw dut::failure("et");
		}
        }
	mapi_close_handle(hdl);
}
