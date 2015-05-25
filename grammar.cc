#include <pqxx/pqxx>
#include <cstdlib>
#include <numeric>

#include "random.hh"
#include "relmodel.hh"
#include "grammar.hh"

using namespace std;
using namespace pqxx;


table_ref *table_ref::factory(scope &s) {
  table_ref *r;
  random();
  if (random()%7)
    r = new table_or_query_name(s);
  else
    r = new table_subquery(s);
  return r;
}

table_or_query_name::table_or_query_name(scope &s) {
  t = random_pick<named_relation*>(s.tables);
}

string table_or_query_name::str() {
  string r("");
  r += t->ident();
  return r;
}

int table_subquery::instances;

table_subquery::table_subquery(scope &s) {
  query = new query_spec(s);
  t = new named_relation();
  t->columns = query->sl.derived_table.columns;
  ostringstream r;
  r << "subq_" << instances++;
  t->name = r.str();
}

string table_subquery::str() {
  ostringstream r;
  r << "(" << query->str() << ") as " << t->name;
  return r.str();
}

string from_clause::str() {
  string r("");
  if (! reflist.size())
    return r;
  r += "\n    from " + reflist[0]->str();
  return r;
}

from_clause::from_clause(scope &s) {
  reflist.push_back(table_ref::factory(s));
}

select_list::select_list(query_spec *q)
{
  query = q;
  derived_table.columns.push_back(column("some", "integer"));
}

std::string select_list::str()
{
  return string("1");
}

string query_spec::str() {
  string r("select ");
  r += set_quantifier + " ";

  r += sl.str();

  r += " " + expr.str();
  return r;
}

query_spec::query_spec(scope &s) : expr(s), sl(this) {
  set_quantifier = (random() % 5) ? "" : "distinct ";
}
