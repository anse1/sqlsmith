#ifndef GRAMMAR_HH
#define GRAMMAR_HH

#include <ostream>
#include "relmodel.hh"
#include <memory>
#include "schema.hh"

#include "prod.hh"
#include "expr.hh"

using std::shared_ptr;

struct table_ref : prod {
  vector<shared_ptr<named_relation> > refs;
  static shared_ptr<table_ref> factory(prod *p);
  table_ref(prod *p) : prod(p) { }
  virtual ~table_ref() { }
};

struct table_or_query_name : table_ref {
  virtual void out(std::ostream &out);
  table_or_query_name(prod *p);
  virtual ~table_or_query_name() { }
private:
  named_relation *t;
};

struct table_subquery : table_ref {
  bool is_lateral;
  virtual void out(std::ostream &out);
  shared_ptr<struct query_spec> query;
  table_subquery(prod *p, bool lateral = false);
  static int instances;
  virtual ~table_subquery();
  virtual void accept(prod_visitor *v);
};

struct joined_table : table_ref {
  virtual void out(std::ostream &out);  
  joined_table(prod *p);
  std::string type;
  std::string condition;
  std::string alias;
  virtual std::string ident() { return alias; }
  shared_ptr<table_ref> lhs;
  shared_ptr<table_ref> rhs;
  virtual ~joined_table() {
  }
  virtual void accept(prod_visitor *v) {
    lhs->accept(v);
    rhs->accept(v);
    v->visit(this);
  }
};

struct from_clause : prod {
  std::vector<shared_ptr<table_ref> > reflist;
  virtual void out(std::ostream &out);
  from_clause(prod *p);
  ~from_clause() { }
  virtual void accept(prod_visitor *v) {
    v->visit(this);
    for (auto p : reflist)
      p->accept(v);
  }
};

struct select_list : prod {
  std::vector<shared_ptr<value_expr> > value_exprs;
  relation derived_table;
  int columns = 0;
  select_list(query_spec *q);
  virtual void out(std::ostream &out);
  ~select_list() { }
  virtual void accept(prod_visitor *v) {
    v->visit(this);
    for (auto p : value_exprs)
      p->accept(v);
  }
};

struct query_spec : prod {
  std::string set_quantifier;
  shared_ptr<struct from_clause> from_clause;
  shared_ptr<struct select_list> select_list;
  shared_ptr<bool_expr> search;
  std::string limit_clause;
  virtual void out(std::ostream &out);
  query_spec(prod *p, struct scope *s, bool lateral = 0);
  virtual ~query_spec() { delete scope; }
  virtual void accept(prod_visitor *v) {
    v->visit(this);
    select_list->accept(v);
    from_clause->accept(v);
    search->accept(v);
  }
};

struct prepare_stmt : prod {
  query_spec q;
  static long seq;
  long id;
  virtual void out(std::ostream &out) {
    out << "prepare prep" << id << " as " << q;
  }
  prepare_stmt(prod *p) : prod(p), q(p, scope) {
    id = seq++;
  }
  virtual void accept(prod_visitor *v) {
    v->visit(this);
    q.accept(v);
  }
};

#endif
