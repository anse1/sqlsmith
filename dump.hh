/// @file
/// @brief Dump syntax trees as GraphML
#ifndef DUMP_HH
#define DUMP_HH

#include <iostream>
#include <fstream>
#include <string>

#include "prod.hh"
#include "log.hh"

struct graphml_dumper : prod_visitor {
  std::ostream &o;
  virtual void visit(struct prod *p);
  graphml_dumper(std::ostream &out);
  static std::string id(prod *p);
  std::string type(struct prod *p);
  virtual ~graphml_dumper();
};

struct ast_logger : logger {
  int queries = 0;
  virtual void generated(prod &query);
};

#endif
