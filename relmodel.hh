#ifndef RELMODEL_HH
#define RELMODEL_HH
#include <string>
#include <iostream>
#include <pqxx/pqxx>
#include <cstdlib>
#include <numeric>

using std::string;

struct column {
  string name;
  string type;
  column(string name) : name(name) { }
  column(string name, string type) : name(name), type(type) { }
};

struct relation {
  std::vector<column> columns;
};

struct named_relation : public relation {
  string name;
  virtual string ident() { return name; }
  virtual ~named_relation() { };
};

struct table : public named_relation {
  string catalog;
  string name;
  string schema;
  table(string catalog, string name, string schema)
    : catalog(catalog), name(name), schema(schema) { }
  table() { }
  virtual string ident() { return schema + "." + name; }
  virtual ~table() { };
};

struct scope {
  struct scope *parent;
  std::vector<named_relation*> tables;
};

struct op {
  string name;
  string left;
  string right;
  string result;
  op(string n,string l,string r, string res)
    : name(n), left(l), right(r), result(res) { }
  op() { }
};

#endif
