#ifndef GRAMMAR_HH
#define GRAMMAR_HH

#include <ostream>
#include "relmodel.hh"
#include <memory>

using std::shared_ptr;

struct prod_visitor {
  virtual void visit(struct prod *p) = 0;
  virtual ~prod_visitor() { }
};

struct prod {
  struct prod *pprod;
  int level;
  prod(prod *parent) : pprod(parent) {
    level = parent ? parent->level + 1 : 0;
  }
  virtual void out(std::ostream &out) = 0;
  virtual void accept(prod_visitor *v) { v->visit(this); }
};

std::ostream& operator<<(std::ostream& s, struct prod& p);

struct table_ref : prod {
  named_relation *t;
  static shared_ptr<table_ref> factory(prod *p, scope &s);
  table_ref(prod *p) : prod(p) { }
  virtual ~table_ref() { }
  virtual std::string ident() { return t->ident(); }
};

struct table_or_query_name : table_ref {
  virtual void out(std::ostream &out);
  table_or_query_name(prod *p, scope &s);
  virtual ~table_or_query_name() { }
  static int sequence;
  virtual std::string ident() { return alias; }
  std::string alias;
};

struct table_subquery : table_ref {
  virtual void out(std::ostream &out);
  shared_ptr<query_spec> query;
  table_subquery(prod *p, scope &s);
  static int instances;
  virtual ~table_subquery();
  virtual void accept(prod_visitor *v);
};

struct joined_table : table_ref {
  virtual void out(std::ostream &out);  
  joined_table(prod *p, scope &s);
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
  from_clause(prod *p, scope &s);
  ~from_clause() { }
  virtual void accept(prod_visitor *v) {
    v->visit(this);
    for (auto p : reflist)
      p->accept(v);
  }
};

struct value_expr: prod {
  std::string type;
  virtual void out(std::ostream &out) = 0;
  virtual ~value_expr() { }
  value_expr(prod *p) : prod(p) { }
  static shared_ptr<value_expr> factory(prod *p, query_spec *q);
};

struct const_expr: value_expr {
  const_expr(prod *p) : value_expr(p) { type = "integer"; }
  virtual void out(std::ostream &out) { out << d42(); }
  virtual ~const_expr() { }
};

struct column_reference: value_expr {
  column_reference(prod *p, query_spec *q);
  virtual void out(std::ostream &out) { out << reference; }
  std::string reference;
  virtual ~column_reference() { }
};

struct bool_expr : value_expr {
  virtual ~bool_expr() { }
  bool_expr(prod *p, query_spec *q) : value_expr(p) { type = "bool"; }
  static shared_ptr<bool_expr> factory(prod *p, query_spec *q);
};

struct truth_value : bool_expr {
  virtual ~truth_value() { }
  const char *op;
  virtual void out(std::ostream &out) { out << op; }
  truth_value(prod *p, query_spec *q) : bool_expr(p, q) {
    op = ( (d6() < 4) ? "true" : "false");
  }
};

struct null_predicate : bool_expr {
  virtual ~null_predicate() { }
  const char *negate;
  shared_ptr<value_expr> expr;
  null_predicate(prod *p, query_spec *q) : bool_expr(p, q) {
    negate = ((d6()<4) ? "not " : "");
    expr = value_expr::factory(this, q);
  }
  virtual void out(std::ostream &out) {
    out << *expr << " is " << negate << "NULL";
  }
  virtual void accept(prod_visitor *v) {
    v->visit(this);
    expr->accept(v);
  }
};

struct bool_binop : bool_expr {
  shared_ptr<value_expr> lhs, rhs;
  bool_binop(prod *p, query_spec *q) : bool_expr(p, q) { }
  virtual void out(std::ostream &out) = 0;
  virtual void accept(prod_visitor *v) {
    v->visit(this);
    lhs->accept(v);
    rhs->accept(v);
  }
};

struct bool_term : bool_binop {
  virtual ~bool_term() { }
  const char *op;
  virtual void out(std::ostream &out) {
    out << "( " << *lhs << " ) " << op << " ( " << *rhs << " )";
  }
  bool_term(prod *p, query_spec *q) : bool_binop(p, q)
  {
    op = ((d6()<4) ? "or" : "and");
    lhs = bool_expr::factory(this, q);
    rhs = bool_expr::factory(this, q);
  }
};

struct distinct_pred : bool_binop {
  distinct_pred(prod *p, query_spec *q);
  virtual ~distinct_pred() { };
  virtual void out(std::ostream &o) {
    o << *lhs << " is distinct from " << *rhs;
  }
};

struct comparison_op : bool_binop {
  op *oper;
  comparison_op(prod *p, query_spec *q);
  virtual ~comparison_op() { };
  virtual void out(std::ostream &o) {
    o << *lhs << oper->name << *rhs;
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
  from_clause fc;
  select_list sl;
  shared_ptr<bool_expr> search;
  std::string limit_clause;
  virtual void out(std::ostream &out);
  query_spec(prod *p, scope &s);
  virtual ~query_spec() { }
  virtual void accept(prod_visitor *v) {
    v->visit(this);
    sl.accept(v);
    fc.accept(v);
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
  prepare_stmt(prod *p, scope &s) : prod(p), q(p, s) {
    id = seq++;
  }
  virtual void accept(prod_visitor *v) {
    v->visit(this);
    q.accept(v);
  }
};

#endif
