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

struct table_sample : table_ref {
  virtual void out(std::ostream &out);
  table_sample(prod *p);
  virtual ~table_sample() { }
private:
  struct table *t;
  string method;
  double percent;
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
  select_list(prod *p);
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

struct modifying_stmt : prod {
  table *victim;
  modifying_stmt(prod *p, struct scope *s);
  virtual ~modifying_stmt() { delete scope; }
};

struct delete_stmt : modifying_stmt {
  shared_ptr<bool_expr> search;
  delete_stmt(prod *p, struct scope *s);
  virtual ~delete_stmt() { }
  virtual void out(std::ostream &out) {
    out << "delete from " << victim->ident() << std::endl;
    indent(out);
    out << "where " << std::endl << *search;
  }
  virtual void accept(prod_visitor *v) {
    v->visit(this);
    search->accept(v);
  }
};

struct delete_returning : delete_stmt {
  shared_ptr<struct select_list> select_list;
  delete_returning(prod *p, struct scope *s);
  virtual void out(std::ostream &out) {
    delete_stmt::out(out);
    out << std::endl << "returning " << *select_list;
  }
  virtual void accept(prod_visitor *v) {
    v->visit(this);
    search->accept(v);
    select_list->accept(v);
  }
};

struct insert_stmt : modifying_stmt {
  vector<shared_ptr<value_expr> > value_exprs;
  insert_stmt(prod *p, struct scope *s);
  virtual ~insert_stmt() {  }
  virtual void out(std::ostream &out);
  virtual void accept(prod_visitor *v) {
    v->visit(this);
    for (auto p : value_exprs) p->accept(v);
  }
};

struct set_list : prod {
  vector<shared_ptr<value_expr> > value_exprs;
  vector<string> names;
  set_list(modifying_stmt *pprod);
  virtual ~set_list() {  }
  virtual void out(std::ostream &out);
  virtual void accept(prod_visitor *v) {
    v->visit(this);
    for (auto p : value_exprs) p->accept(v);
  }
};

struct update_stmt : modifying_stmt {
  shared_ptr<bool_expr> search;
  shared_ptr<struct set_list> set_list;
  update_stmt(prod *p, struct scope *s);
  virtual ~update_stmt() {  }
  virtual void out(std::ostream &out);
  virtual void accept(prod_visitor *v) {
    v->visit(this);
    search->accept(v);
  }
};

struct update_returning : update_stmt {
  shared_ptr<struct select_list> select_list;
  update_returning(prod *p, struct scope *s);
  virtual void out(std::ostream &out) {
    update_stmt::out(out);
    out << std::endl << "returning " << *select_list;
  }
  virtual void accept(prod_visitor *v) {
    v->visit(this);
    search->accept(v);
    set_list->accept(v);
    select_list->accept(v);
  }
};

shared_ptr<prod> statement_factory(struct scope *s);

#endif
