#ifndef GRAMMAR_HH
#define GRAMMAR_HH

#include <ostream>
#include "relmodel.hh"

struct prod {
  virtual void out(std::ostream &out) = 0;
};

struct table_ref : public prod {
  named_relation *t;
  static table_ref *factory(scope &s);
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
  table_ref *lhs;
  table_ref *rhs;
  virtual ~joined_table() {
    delete lhs;
    delete rhs;
  }
};

struct from_clause : public prod {
  std::vector<table_ref*> reflist;
  virtual void out(std::ostream &out);
  from_clause(scope &s);
  ~from_clause() { for (auto p : reflist) delete p; }
};

struct value_expression: public prod {
  std::string type;
  virtual void out(std::ostream &out) = 0;
  virtual ~value_expression() { }
  static struct value_expression *factory(struct query_spec *q);
};

struct const_expression: value_expression {
  const_expression() { type = "integer"; }
  virtual void out(std::ostream &out) { out << "42"; }
  virtual ~const_expression() { }
};

struct column_reference: value_expression {
  column_reference(struct query_spec *q);
  virtual void out(std::ostream &out) { out << reference; }
  std::string reference;
  virtual ~column_reference() { }
};

struct select_list : public prod {
  struct query_spec *query;
  std::vector<value_expression*> value_exprs;
  relation derived_table;
  int columns = 0;
  select_list(struct query_spec *q);
  virtual void out(std::ostream &out);
  ~select_list() { for (auto p : value_exprs) delete p; }
};

struct query_spec : public prod {
  std::string set_quantifier;
  from_clause fc;
  select_list sl;
  std::string limit_clause;
  virtual void out(std::ostream &out);
  query_spec(scope &s);
  virtual ~query_spec() { }
};


#endif
