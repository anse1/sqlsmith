#include <iostream>
#include <pqxx/pqxx>
#include <cstdlib>
#include <numeric>

#include <random>

using namespace std;
using namespace pqxx;

default_random_engine rng;

template<typename T> T& random_pick(vector<T>& container) {
  uniform_int_distribution<int> pick(0, container.size()-1);
  return container[pick(rng)];
}

struct column {
  string name;
  string type;
  struct table *table;
  column(string name) : name(name) { }
};

struct table {
  vector<column> columns;
  string catalog;
  string name;
  string schema;
  table(string catalog, string name, string schema)
    : catalog(catalog), name(name), schema(schema)
  { }
};

struct schema {
  vector<table> tables;
  void summary() {
    cout << "Found " << tables.size() <<
      " user table(s) in information schema." << endl;
  };
};

struct schema_pqxx : public schema {
  schema_pqxx() {
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
	       "data_type"
	       " from information_schema.columns where");
      q += " table_catalog = " + w.quote(t->catalog);
      q += " and table_schema = " + w.quote(t->schema);
      q += " and table_name = " + w.quote(t->name);
      q += ";";

      r = w.exec(q);
      for (auto row : r) {
	column c(row[0].as<string>());
	c.type = row[1].as<string>();
	c.table = &*t;
	t->columns.push_back(c);
      }
    }
  }
};

schema_pqxx schema;

struct prod {
  virtual string to_str() = 0;
};

struct table_scope {
  struct table_scope *parent;
  vector<table> tables;
};

struct column_scope {
  struct column_scope *parent;
  vector<column> columns;
};

struct from_clause : public prod {
  vector<table> reflist;
  string to_str() {
    string r("");
    if (! reflist.size())
      return r;
    r += "\n    from " + reflist[0].schema + "." + reflist[0].name;
    return r;
  }
  from_clause() {
    if (random()%5) {
      reflist.push_back(random_pick<table>(schema.tables));
    } else {
      reflist.push_back(random_pick<table>(schema.tables));
    }
  }
};

struct table_expression : public prod {
  from_clause fc;

  string to_str() {
    return fc.to_str();
  }
  
};

struct query_spec : public prod {
  string set_quantifier;
  vector<column> sl;
  table_expression expr;
  
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

  query_spec() {
    do {
      table t = random_pick<table>(expr.fc.reflist);
      sl.push_back(random_pick<column>(t.columns));
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
      work w(c);
      w.commit();
      while (1) {
	work w(c);
	query_spec gen;
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
