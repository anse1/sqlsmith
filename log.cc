#include <iostream>
#include <pqxx/pqxx>
#include <numeric>
#include <chrono>
#include <regex>
#include <string>

#include <thread>
#include <mutex>

#include "random.hh"
#include "grammar.hh"
#include "relmodel.hh"
#include "schema.hh"

#include "config.h"

#include "log.hh"

using namespace std;
using namespace pqxx;

struct stats_visitor : prod_visitor {
  int nodes = 0;
  int maxlevel = 0;
  map<const char*, long> production_stats;
  virtual void visit(struct prod *p) {
    nodes++;
    if (p->level > maxlevel)
      maxlevel = p->level;
    production_stats[typeid(*p).name()]++;
  }
  void report() {
    cerr << "production statistics" << endl;
    vector<pair<const char *, long> > report;
    for (auto p : production_stats)
      report.push_back(p);
    stable_sort(report.begin(), report.end(),
		[](const pair<std::string, long> &a,
		   const pair<std::string, long> &b)
		{ return a.second > b.second; });
    for (auto p : report) {
      cerr << p.second << "\t" << p.first << endl;
    }
  }
};

static regex e_timeout("ERROR:  canceling statement due to statement timeout(\n|.)*");
static regex e_syntax("ERROR:  syntax error at or near(\n|.)*");

void cerr_logger::report(prod &p)
{
  stats_visitor s;
  p.accept(&s);

  sum_nodes += s.nodes;
  sum_height += s.maxlevel;

  if ((10*columns-1) == queries%(10*columns)) {
    cerr << endl << "queries: " << queries << endl;
// 	 << " (" << 1000.0*query_count/gen_time.count() << " gen/s, "
// 	 << 1000.0*query_count/query_time.count() << " exec/s)" << endl;
    cerr << "AST stats (avg): height = " << sum_height/queries
	 << " nodes = " << sum_nodes/queries << endl;

    vector<pair<std::string, long> > report;
    for (auto e : errors) {
      report.push_back(e);
    }
    stable_sort(report.begin(), report.end(),
		[](const pair<std::string, long> &a,
		   const pair<std::string, long> &b)
		{ return a.second > b.second; });
    long err_count = 0;
    for (auto e : report) {
      err_count += e.second;
      cerr << e.second << "\t" << e.first << endl;
    }
    cerr << "error rate: " << (float)err_count/(queries) << endl;

  } else if (columns-1 == (queries%columns)) {
    cerr << endl;
  }
}


void cerr_logger::executed(prod &query)
{
  cerr << ".";
  report(query);
  queries++;
}

void cerr_logger::error(prod &query, const sql_error &e)
{
  istringstream err(e.what());
  string line;
  
  getline(err, line);
  errors[line]++;
  if (regex_match(e.what(), e_timeout))
    cerr << "t";
  else if (regex_match(e.what(), e_syntax))
    cerr << "s";
  else
    cerr << "e";

  report(query);
  queries++;
}


pqxx_logger::pqxx_logger(std::string conninfo)
{
  c = make_shared<pqxx::connection>(conninfo);
  c->prepare("error",
	     "insert into error (rev, msg, query) values ('" GITREV
	     "', $1, $2)");

}
void pqxx_logger::error(prod &query, const sql_error &e)
{
  work w(*c);
  std::ostringstream s;
  s << query;
  w.prepared("error")(e.what())(s.str()).exec();
  w.commit();
}
