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
  struct table *table;
  column(std::string name) : name(name) { }
};

struct table {
  std::vector<column> columns;
  std::string catalog;
  std::string name;
  std::string schema;
  table(std::string catalog, std::string name, std::string schema)
    : catalog(catalog), name(name), schema(schema)
  { }
};

struct schema {
  std::vector<table> tables;
  void summary() {
    std::cout << "Found " << tables.size() <<
      " user table(s) in information schema." << std::endl;
  };
};

struct schema_pqxx : public schema {
  schema_pqxx();
};
  
extern schema_pqxx schema;

#endif
