#ifndef GRAMMAR_HH
#define GRAMMAR_HH

#include <ostream>
#include "relmodel.hh"
#include <memory>

using std::shared_ptr;

struct prod {
  struct prod *parent;
  virtual void out(std::ostream &out) = 0;
};

std::ostream& operator<<(std::ostream& s, struct prod& p);

struct table_ref : public prod {
  named_relation *t;
  static shared_ptr<table_ref> factory(scope &s);
  virtual ~table_ref() { }
  virtual std::string ident() { return t->ident(); }
};

struct table_or_query_name : public table_ref {
  virtual void out(std::ostream &out);
  table_or_query_name(scope &s);
  virtual ~table_or_query_name() { }
  static int sequence;
  virtual std::string ident() { return alias; }
  std::string alias;
};

struct table_subquery : public table_ref {
  virtual void out(std::ostream &out);
  struct query_spec *query;
  table_subquery(scope &s);
  static int instances;
  virtual ~table_subquery();
};

struct joined_table : table_ref {
  virtual void out(std::ostream &out);  
  joined_table(scope &s);
  std::string type;
  std::string condition;
  std::string alias;
  virtual std::string ident() { return alias; }
  shared_ptr<table_ref> lhs;
  shared_ptr<table_ref> rhs;
  virtual ~joined_table() {
  }
};

struct from_clause : public prod {
  std::vector<shared_ptr<table_ref> > reflist;
  virtual void out(std::ostream &out);
  from_clause(scope &s);
  ~from_clause() { }
};

struct value_expr: public prod {
  std::string type;
  virtual void out(std::ostream &out) = 0;
  virtual ~value_expr() { }
  static shared_ptr<value_expr> factory(struct query_spec *q);
};

struct const_expr: value_expr {
  const_expr() { type = "integer"; }
  virtual void out(std::ostream &out) { out << random()%43; }
  virtual ~const_expr() { }
};

struct column_reference: value_expr {
  column_reference(struct query_spec *q);
  virtual void out(std::ostream &out) { out << reference; }
  std::string reference;
  virtual ~column_reference() { }
};

struct bool_expr : value_expr {
  virtual ~bool_expr() { }
  bool_expr(struct query_spec *q) { type = "bool"; }
  static shared_ptr<bool_expr> factory(struct query_spec *q);
};

struct truth_value : bool_expr {
  virtual ~truth_value() { }
  const char *op;
  virtual void out(std::ostream &out) { out << op; }
  truth_value(struct query_spec *q) : bool_expr(q) {
    op = ((random()&1) ? "true" : "false");
  }
};

struct null_predicate : bool_expr {
  virtual ~null_predicate() { }
  const char *negate;
  shared_ptr<value_expr> expr;
  null_predicate(struct query_spec *q) : bool_expr(q) {
    negate = ((random()&1) ? "not " : "");
    expr = value_expr::factory(q);
  }
  virtual void out(std::ostream &out) {
    out << *expr << " is " << negate << "NULL";
  }
};

struct bool_term : bool_expr {
  virtual ~bool_term() { }
  shared_ptr<bool_expr> lhs, rhs;
  const char *op;
  virtual void out(std::ostream &out) {
    out << "( " << *lhs << " ) " << op << " ( " << *rhs << " )";
  }
  bool_term(struct query_spec *q) : bool_expr(q)
  {
    op = ((random()&1) ? "or" : "and");
    lhs = bool_expr::factory(q);
    rhs = bool_expr::factory(q);
  }
};

struct comparison_op : bool_expr {
  shared_ptr<value_expr> lhs;
  shared_ptr<value_expr> rhs;
  op *oper;
  comparison_op(struct query_spec *q);
  virtual ~comparison_op() { };
  virtual void out(std::ostream &o) {
    o << *lhs << oper->name << *rhs;
  }
};
  
struct select_list : public prod {
  struct query_spec *query;
  std::vector<shared_ptr<value_expr> > value_exprs;
  relation derived_table;
  int columns = 0;
  select_list(struct query_spec *q);
  virtual void out(std::ostream &out);
  ~select_list() { }
};

struct query_spec : public prod {
  std::string set_quantifier;
  from_clause fc;
  select_list sl;
  shared_ptr<bool_expr> search;
  std::string limit_clause;
  virtual void out(std::ostream &out);
  query_spec(scope &s);
  virtual ~query_spec() { }
};


#endif
