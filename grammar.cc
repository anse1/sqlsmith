#include <pqxx/pqxx>
#include <cstdlib>
#include <numeric>

#include "random.hh"
#include "relmodel.hh"
#include "grammar.hh"

using namespace std;
using namespace pqxx;


table_ref::table_ref(scope &s) {
  t = random_pick<table*>(s.tables);
}

string table_ref::to_str() {
  string r("");
  r += t->schema + "." + t->name;
  return r;
}

string from_clause::to_str() {
  string r("");
  if (! reflist.size())
    return r;
  r += "\n    from " + reflist[0]->to_str();
  return r;
}

from_clause::from_clause(scope &s) {
  reflist.push_back(new table_ref(s));
}

string query_spec::to_str() {
  string r("select ");
  r += set_quantifier ;

  for (auto col = sl.begin(); col != sl.end(); col++) {
    r += col->name;
    if (col+1 != sl.end())
      r += ", ";
  }

  r += " " + expr.to_str();
  r += ";";
  return r;
}

query_spec::query_spec(scope &s) : expr(s) {
  do {
    table *t = random_pick<table_ref*>(expr.fc.reflist)->t;
    sl.push_back(random_pick<column>(t->columns));
  } while(random()%3);

  set_quantifier = (random() % 5) ? "" : "distinct ";
  for(auto &c : sl) {
    if (c.type == "anyarray")
      set_quantifier = "";
  }
}
