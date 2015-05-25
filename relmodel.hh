#ifndef RELMODEL_HH
#define RELMODEL_HH
#include <string>
#include <iostream>
#include <pqxx/pqxx>
#include <cstdlib>
#include <numeric>

struct column {
  std::string name;
  std::string type;
  column(std::string name) : name(name) { }
  column(std::string name, std::string type) : name(name), type(type) { }
};

struct relation {
  std::vector<column> columns;
};

struct named_relation : public relation {
  std::string name;
  virtual std::string ident() { return name; }
  virtual ~named_relation() { };
};

struct table : public named_relation {
  std::string catalog;
  std::string name;
  std::string schema;
  table(std::string catalog, std::string name, std::string schema)
    : catalog(catalog), name(name), schema(schema) { }
  table() { }
  virtual std::string ident() { return schema + "." + name; }
  virtual ~table() { };
};

struct scope {
  struct scope *parent;
  std::vector<named_relation*> tables;
};

struct schema {
  std::vector<table> tables;
  void summary() {
    std::cout << "Found " << tables.size() <<
      " user table(s) in information schema." << std::endl;
  };
  void fill_scope(struct scope &s) {
    for (auto &t : tables)
      s.tables.push_back(&t);
  }
};

struct schema_pqxx : public schema {
  schema_pqxx();
};
  
extern schema_pqxx schema;

#endif
