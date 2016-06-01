#include <stdexcept>
#include <string>
#include <iostream>

#ifndef PROD_HH
#define PROD_HH

struct prod_visitor {
  virtual void visit(struct prod *p) = 0;
  virtual ~prod_visitor() { }
};

struct prod {
  struct prod *pprod;
  struct scope *scope;
  int level;
  long retries = 0;
  prod(prod *parent) : pprod(parent) {
    if (parent) {
      level = parent->level + 1;
      scope = parent->scope;
    } else {
      scope = 0;
      level = 0;
    }
  }
  virtual void indent(std::ostream &out) {
    out << std::endl;
    for (int i = 0; i < level; i++)
      out << "  ";
  }
  virtual void out(std::ostream &out) = 0;
  virtual void accept(prod_visitor *v) { v->visit(this); }
  void retry() {
    if (retries++ > 100000)
      throw std::runtime_error(std::string("excessive retries in ")
			       + typeid(this).name());
  };
  
};

inline std::ostream& operator<<(std::ostream& s, prod& p)
{
  p.out(s); return s;
}


#endif
