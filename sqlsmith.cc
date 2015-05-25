#include <iostream>
#include <pqxx/pqxx>
#include <cstdlib>
#include <numeric>

#include "random.hh"
#include "relmodel.hh"

using namespace std;
using namespace pqxx;

struct prod {
  virtual string to_str() = 0;
};

struct from_clause : public prod {
  vector<table*> reflist;
  string to_str() {
    string r("");
    if (! reflist.size())
      return r;
    r += "\n    from " + reflist[0]->schema + "." + reflist[0]->name;
    return r;
  }
  from_clause(scope &s) {
    reflist.push_back(random_pick<table*>(s.tables));
  }
};

struct table_expression : public prod {
  from_clause fc;
  table_expression(scope &s) : fc(s) { } ; 
  string to_str() {
    return fc.to_str();
  }
};

struct query_spec : public prod {
  string set_quantifier;
  table_expression expr;
  table derived_table;
  vector<column> &sl = derived_table.columns;
  
  string to_str() {
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

  query_spec(scope &s) : expr(s){
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
};

int main()
{
  try
    {
      connection c;
      schema.summary();
      scope scope;
      schema.fill_scope(scope);
      work w(c);
      w.commit();
      
      while (1) {
	work w(c);
	query_spec gen(scope);
	cout << gen.to_str() << endl;
	result r = w.exec(gen.to_str());
	w.commit();
      }
    }
  catch (const std::exception &e)
    {
      cerr << e.what() << endl;
      return 1;
    }
}
