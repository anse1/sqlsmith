/// @file
/// @brief Base class for grammar productions

#include <string>
#include <iostream>

#ifndef PROD_HH
#define PROD_HH

/// Base class for walking the AST
struct prod_visitor {
  virtual void visit(struct prod *p) = 0;
  virtual ~prod_visitor() { }
};

/// Base class for AST nodes
struct prod {
  struct prod *pprod;
  struct scope *scope;
  int level;
  long retries = 0;
  prod(prod *parent);
  virtual void indent(std::ostream &out);
  virtual void out(std::ostream &out) = 0;
  virtual void match();
  virtual void accept(prod_visitor *v) { v->visit(this); }
  virtual void fail(const char *reason);
  void retry();
};

inline std::ostream& operator<<(std::ostream& s, prod& p)
{
  p.out(s); return s;
}

#endif
