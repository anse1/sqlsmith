#ifndef SCHEMA_HH
#define SCHEMA_HH

#include <string>
#include <iostream>
#include <pqxx/pqxx>
#include <numeric>

#include "relmodel.hh"
#include "random.hh"

struct schema {
  sqltype *booltype;
  sqltype *inttype;
  
  std::vector<table> tables;
  std::vector<op> operators;
  typedef std::tuple<sqltype *,sqltype *,sqltype *> typekey;
  std::multimap<typekey, op> index;
  typedef std::multimap<typekey, op>::iterator op_iterator;

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
  virtual op_iterator find_operator(sqltype *left, sqltype *right, sqltype *res) {
    auto cons = index.equal_range(typekey(left, right, res));
    if (cons.first == cons.second)
      return index.end();
    else
      return random_pick<>(cons.first, cons.second);
  }
};

struct schema_pqxx : public schema {
  schema_pqxx(std::string &conninfo);
};
  
extern schema_pqxx schema;

#endif

