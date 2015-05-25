#ifndef GRAMMAR_HH
#define GRAMMAR_HH

#include "relmodel.hh"

struct prod {
  virtual std::string to_str() = 0;
};

struct from_clause : public prod {
  std::vector<table*> reflist;
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
