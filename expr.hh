/// @file
/// @brief grammar: Value expression productions

#ifndef EXPR_HH
#define EXPR_HH

#include "prod.hh"
#include <string>

using std::shared_ptr;
using std::vector;
using std::string;

struct value_expr: prod {
  sqltype *type;
  virtual void out(std::ostream &out) = 0;
  virtual ~value_expr() { }
  value_expr(prod *p) : prod(p) { }
  static shared_ptr<value_expr> factory(prod *p, sqltype *type_constraint = 0);
};

struct case_expr : value_expr {
  shared_ptr<value_expr> condition;
  shared_ptr<value_expr> true_expr;
  shared_ptr<value_expr> false_expr;
  case_expr(prod *p, sqltype *type_constraint = 0);
  void out(std::ostream &out) override;
  void accept(prod_visitor *v) override;
};

struct funcall : value_expr {
  routine *proc;
  bool is_aggregate;
  vector<shared_ptr<value_expr> > parms;
  void out(std::ostream &out) override;
  ~funcall() override { }
  funcall(prod *p, sqltype *type_constraint = 0, bool agg = 0);
  void accept(prod_visitor *v) override {
    v->visit(this);
    for (auto p : parms)
      p->accept(v);
  }
};

struct atomic_subselect : value_expr {
  table *tab;
  column *col;
  int offset;
  routine *agg;
  atomic_subselect(prod *p, sqltype *type_constraint = 0);
  void out(std::ostream &out) override;
};

struct const_expr: value_expr {
  std::string expr;
  const_expr(prod *p, sqltype *type_constraint = 0);
  void out(std::ostream &out) override { out << expr; }
  ~const_expr() override { }
};

struct column_reference: value_expr {
  column_reference(prod *p, sqltype *type_constraint = 0);
  void out(std::ostream &out) override { out << reference; }
  std::string reference;
  ~column_reference() override { }
};

struct coalesce : value_expr {
  const char *abbrev_;
  vector<shared_ptr<value_expr> > value_exprs;
  ~coalesce() override { };
  coalesce(prod *p, sqltype *type_constraint = 0, const char *abbrev = "coalesce");
  void out(std::ostream &out) override;
  void accept(prod_visitor *v) override {
    v->visit(this);
    for (auto p : value_exprs)
      p->accept(v);
  }
};

struct nullif : coalesce {
 ~nullif() override { };
     nullif(prod *p, sqltype *type_constraint = 0)
	  : coalesce(p, type_constraint, "nullif")
	  { };
};

struct bool_expr : value_expr {
  ~bool_expr() override { }
  bool_expr(prod *p) : value_expr(p) { type = scope->schema->booltype; }
  static shared_ptr<bool_expr> factory(prod *p);
};

struct truth_value : bool_expr {
  ~truth_value() override { }
  const char *op;
  void out(std::ostream &out) override { out << op; }
  truth_value(prod *p) : bool_expr(p) {
    op = ( (d6() < 4) ? scope->schema->true_literal : scope->schema->false_literal);
  }
};

struct null_predicate : bool_expr {
  ~null_predicate() override { }
  const char *negate;
  shared_ptr<value_expr> expr;
  null_predicate(prod *p) : bool_expr(p) {
    negate = ((d6()<4) ? "not " : "");
    expr = value_expr::factory(this);
  }
  void out(std::ostream &out) override {
    out << *expr << " is " << negate << "NULL";
  }
  void accept(prod_visitor *v) override {
    v->visit(this);
    expr->accept(v);
  }
};

struct exists_predicate : bool_expr {
  shared_ptr<struct query_spec> subquery;
  ~exists_predicate() override { }
  exists_predicate(prod *p);
  virtual void out(std::ostream &out);
  virtual void accept(prod_visitor *v);
};

struct bool_binop : bool_expr {
  shared_ptr<value_expr> lhs, rhs;
  bool_binop(prod *p) : bool_expr(p) { }
  virtual void out(std::ostream &out) = 0;
  void accept(prod_visitor *v) override {
    v->visit(this);
    lhs->accept(v);
    rhs->accept(v);
  }
};

struct bool_term : bool_binop {
  virtual ~bool_term() { }
  const char *op;
  void out(std::ostream &out) override {
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
  ~distinct_pred() override { };
  void out(std::ostream &o) override {
    o << *lhs << " is distinct from " << *rhs;
  }
};

struct comparison_op : bool_binop {
  op *oper;
  comparison_op(prod *p);
  ~comparison_op() override { };
  virtual void out(std::ostream &o) {
    o << *lhs << " " << oper->name << " " << *rhs;
  }
};

struct window_function : value_expr {
  virtual void out(std::ostream &out);
  ~window_function() override { }
  window_function(prod *p, sqltype *type_constraint);
  vector<shared_ptr<column_reference> > partition_by;
  vector<shared_ptr<column_reference> > order_by;
  shared_ptr<funcall> aggregate;
  static bool allowed(prod *pprod);
  void accept(prod_visitor *v) override {
    v->visit(this);
    aggregate->accept(v);
    for (auto p : partition_by)
      p->accept(v);
    for (auto p : order_by)
      p->accept(v);
  }
};

#endif
