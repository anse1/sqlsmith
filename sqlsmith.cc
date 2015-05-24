#include <iostream>
#include <pqxx/pqxx>
#include <cstdlib>
#include <numeric>

#include <random>

using namespace std;
using namespace pqxx;

default_random_engine rng;

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
      if (schema == "pg_catalog")
	continue;
      if (schema == "information_schema")
	continue;
      
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
      for (auto row = r.begin(); row != r.end(); ++row) {
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

struct table_expression : public prod {
  vector<table> reference_list;
  
  string to_str() {
    string r("");
    if (! reference_list.size())
      return r;

    r += "from ";
    r += reference_list[0].name;

    return r;
  }
  table_expression() {
    uniform_int_distribution<int> dist(0,schema.tables.size()-1);
    reference_list.push_back(schema.tables[dist(rng)]);
  }
};

struct query_spec : public prod {
  string set_quantifier;
  vector<string> select_list;
  table_expression expr;
  
  string to_str() {
    string r("select ");
    r += set_quantifier + " ";

    r += accumulate(select_list.begin(),select_list.end(),string(),
		    [](string a,string b){ return a.length() ? a+","+b:b; }); 

    r += " " + expr.to_str();
    r += ";";
    return r;
  }

  query_spec() {
    set_quantifier = random() & 1 ? "all" : "distinct" ;
    select_list.push_back("1");
    select_list.push_back("version()");
  }
};

int main()
{
  try
    {
      connection c;

      schema.summary();

      for (int i; i<20; i++) {
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
