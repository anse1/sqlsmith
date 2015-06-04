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
  struct scope *scope;
  int level;
  prod(prod *parent) : pprod(parent) {
    if (parent) {
      level = parent->level + 1;
      scope = parent->scope;
    } else {
      scope = 0;
      level = 0;
    }
  }
  virtual void out(std::ostream &out) = 0;
  virtual void accept(prod_visitor *v) { v->visit(this); }
};

std::ostream& operator<<(std::ostream& s, struct prod& p);

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
  static int sequence;
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

struct value_expr: prod {
  sqltype *type;
  virtual void out(std::ostream &out) = 0;
  virtual ~value_expr() { }
  value_expr(prod *p) : prod(p) { }
  static shared_ptr<value_expr> factory(prod *p);
};

struct const_expr: value_expr {
  const_expr(prod *p) : value_expr(p) { type = sqltype::getbyname("integer"); }
  virtual void out(std::ostream &out) { out << d42(); }
  virtual ~const_expr() { }
};

struct column_reference: value_expr {
  column_reference(prod *p);
  virtual void out(std::ostream &out) { out << reference; }
  std::string reference;
  virtual ~column_reference() { }
};

struct bool_expr : value_expr {
  virtual ~bool_expr() { }
  bool_expr(prod *p) : value_expr(p) { type = sqltype::getbyname("bool"); }
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
    out << "( " << *lhs << " ) " << op << " ( " << *rhs << " )";
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
