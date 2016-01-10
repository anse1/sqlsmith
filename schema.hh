#ifndef SCHEMA_HH
#define SCHEMA_HH

#include <string>
#include <iostream>
#include <pqxx/pqxx>
#include <numeric>
#include <memory>

#include "relmodel.hh"
#include "random.hh"

struct schema {
  sqltype *booltype;
  sqltype *inttype;
  sqltype *internaltype;
  
  std::vector<table> tables;
  std::vector<op> operators;
  std::vector<routine> routines;

  typedef std::tuple<sqltype *,sqltype *,sqltype *> typekey;
  std::multimap<typekey, op> index;
  typedef std::multimap<typekey, op>::iterator op_iterator;

  std::multimap<sqltype*, routine*> routines_returning_type;
  std::multimap<sqltype*, routine*> parameterless_routines_returning_type;
  std::multimap<sqltype*, table*> tables_with_columns_of_type;

  string version;

  virtual std::string quote_name(const std::string &id) = 0;
  
  void summary() {
    std::cout << "Found " << tables.size() <<
      " user table(s) in information schema." << std::endl;
  }
  void fill_scope(struct scope &s) {
    for (auto &t : tables)
      s.tables.push_back(&t);
    s.schema = this;
  }
  virtual void register_operator(op& o) {
    operators.push_back(o);
    typekey t(o.left, o.right, o.result);
    index.insert(std::pair<typekey,op>(t,o));
  }
  virtual void register_routine(routine& r) {
    routines.push_back(r);
  }
  virtual op_iterator find_operator(sqltype *left, sqltype *right, sqltype *res) {
    typekey t(left, right, res);
    auto cons = index.equal_range(t);
    if (cons.first == cons.second)
      return index.end();
    else
      return random_pick<>(cons.first, cons.second);
  }
  schema() { }
  void generate_indexes();
};

struct schema_pqxx : public schema {
  pqxx::connection c;
  virtual std::string quote_name(const std::string &id) {
    return c.quote_name(id);
  }
  schema_pqxx(std::string &conninfo);
};
  
extern schema_pqxx schema;

#endif

