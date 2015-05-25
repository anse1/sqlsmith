#ifndef GRAMMAR_HH
#define GRAMMAR_HH

#include "relmodel.hh"

struct prod {
  virtual std::string to_str() = 0;
};

struct table_ref : public prod {
  table *t;
  static table_ref *factory(scope &s);
};

struct table_or_query_name : public table_ref {
  std::string to_str();
  table_or_query_name(scope &s);
};

struct table_subquery : public table_ref {
  std::string to_str();
  struct query_spec *query;
  table_subquery(scope &s);
  static int instances;
};

struct from_clause : public prod {
  std::vector<table_ref*> reflist;
  std::string to_str();
  from_clause(scope &s);
};

struct table_expression : public prod {
  from_clause fc;
  table_expression(scope &s) : fc(s) { } ; 
  std::string to_str() {
    return fc.to_str();
  }
};

struct query_spec : public prod {
  std::string set_quantifier;
  table_expression expr;
  table derived_table;
  std::vector<column> &sl = derived_table.columns;
  
  std::string to_str();
  query_spec(scope &s);
};


#endif
