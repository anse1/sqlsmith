#ifndef EXPR_HH
#define EXPR_HH

#include "prod.hh"

using std::shared_ptr;
using std::vector;

struct value_expr: prod {
  sqltype *type;
  virtual void out(std::ostream &out) = 0;
  virtual ~value_expr() { }
  value_expr(prod *p) : prod(p) { }
  static shared_ptr<value_expr> factory(prod *p, sqltype *type_constraint = 0);
};

struct const_expr: value_expr {
  const_expr(prod *p) : value_expr(p) { type = scope->schema->inttype; }
  virtual void out(std::ostream &out) { out << d42(); }
  virtual ~const_expr() { }
};

struct column_reference: value_expr {
  column_reference(prod *p, sqltype *type_constraint = 0);
  virtual void out(std::ostream &out) { out << reference; }
  std::string reference;
  virtual ~column_reference() { }
};

struct coalesce : value_expr {
  vector<shared_ptr<value_expr> > value_exprs;
  virtual ~coalesce() { };
  coalesce(prod *p, sqltype *type_constraint = 0);
  virtual void out(std::ostream &out);
  virtual void accept(prod_visitor *v) {
    v->visit(this);
    for (auto p : value_exprs)
      p->accept(v);
  }
};

struct bool_expr : value_expr {
  virtual ~bool_expr() { }
  bool_expr(prod *p) : value_expr(p) { type = scope->schema->booltype; }
  static shared_ptr<bool_expr> factory(prod *p);
};

struct truth_value : bool_expr {
  virtual ~truth_value() { }
  const char *op;
  virtual void out(std::ostream &out) { out << op; }
  truth_value(prod *p) : bool_expr(p) {
    op = ( (d6() < 4) ? "true" : "false");
  }
};

struct null_predicate : bool_expr {
  virtual ~null_predicate() { }
  const char *negate;
  shared_ptr<value_expr> expr;
  null_predicate(prod *p) : bool_expr(p) {
    negate = ((d6()<4) ? "not " : "");
    expr = value_expr::factory(this);
  }
  virtual void out(std::ostream &out) {
    out << *expr << " is " << negate << "NULL";
  }
  virtual void accept(prod_visitor *v) {
    v->visit(this);
    expr->accept(v);
  }
};

struct exists_predicate : bool_expr {
  shared_ptr<struct query_spec> subquery;
  virtual ~exists_predicate() { }
  exists_predicate(prod *p);
  virtual void out(std::ostream &out);
  virtual void accept(prod_visitor *v);
};

struct bool_binop : bool_expr {
  shared_ptr<value_expr> lhs, rhs;
  bool_binop(prod *p) : bool_expr(p) { }
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
    out << "(" << *lhs << ") ";
    indent(out);
    out << op << " (" << *rhs << ")";
  }
  bool_term(prod *p) : bool_binop(p)
  {
    op = ((d6()<4) ? "or" : "and");
    lhs = bool_expr::factory(this);
    rhs = bool_expr::factory(this);
  }
};

struct distinct_pred : bool_binop {
  distinct_pred(prod *p);
  virtual ~distinct_pred() { };
  virtual void out(std::ostream &o) {
    o << *lhs << " is distinct from " << *rhs;
  }
};

struct comparison_op : bool_binop {
  op *oper;
  comparison_op(prod *p);
  virtual ~comparison_op() { };
  virtual void out(std::ostream &o) {
    o << *lhs << " " << oper->name << " " << *rhs;
  }
};
  
#endif
