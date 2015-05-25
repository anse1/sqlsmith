#include <pqxx/pqxx>
#include <cstdlib>
#include <numeric>

#include "random.hh"
#include "relmodel.hh"
#include "grammar.hh"

using namespace std;
using namespace pqxx;

string from_clause::to_str() {
  string r("");
  if (! reflist.size())
    return r;
  r += "\n    from " + reflist[0]->schema + "." + reflist[0]->name;
  return r;
}

from_clause::from_clause(scope &s) {
  reflist.push_back(random_pick<table*>(s.tables));
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
    table *t = random_pick<table*>(expr.fc.reflist);
    sl.push_back(random_pick<column>(t->columns));
  } while(random()%3);

  set_quantifier = (random() % 5) ? "" : "distinct ";
  for(auto &c : sl) {
    if (c.type == "anyarray")
      set_quantifier = "";
  }
}
