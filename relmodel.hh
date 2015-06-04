#ifndef RELMODEL_HH
#define RELMODEL_HH
#include <string>
#include <vector>
#include <map>

using std::string;
using std::vector;
using std::map;

struct sqltype {
  string name;
  static map<string, struct sqltype*> typemap;
  static struct sqltype *get(string s);
  sqltype(string n) : name(n) { }
};

struct column {
  string name;
  sqltype *type;
  column(string name) : name(name) { }
  column(string name, string t) : name(name) {
    type = sqltype::get(t);
  }
  column(string name, sqltype *t) : name(name), type(t) {  }
};

struct relation {
  vector<column> cols;
  virtual vector<column> &columns() { return cols; }
};

struct named_relation : relation {
  string name;
  virtual string ident() { return name; }
  virtual ~named_relation() { }
  named_relation(string n) : name(n) { }
};

struct aliased_relation : named_relation {
  relation *rel;
  virtual ~aliased_relation() { }
  aliased_relation(string n, relation* r) : named_relation(n), rel(r) { }
  virtual vector<column>& columns() { return rel->columns(); }
};

struct table : named_relation {
  string catalog;
  string schema;
  table(string catalog, string name, string schema)
    : named_relation(name), catalog(catalog), schema(schema) { }
  virtual string ident() { return schema + "." + name; }
  virtual ~table() { };
};

struct scope {
  struct scope *parent;
  vector<named_relation*> tables;  // available to table_ref productions
  vector<named_relation*> refs; // available to column_ref productions
  scope(struct scope *parent = 0) : parent(parent) { }
};

struct op {
  string name;
  sqltype *left;
  sqltype *right;
  sqltype *result;
  op(string n,sqltype *l,sqltype *r, sqltype *res)
    : name(n), left(l), right(r), result(res) { }
  op(string n, string l, string r, string res)
    : name(n) {
    left = sqltype::get(l);
    right = sqltype::get(r);
    result = sqltype::get(res);
  }
  op() { }
};

#endif
