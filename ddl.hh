/// @file
/// @brief grammar: Data definition statements

#ifndef DDL_HH
#define DDL_HH

#include <memory>

#include "prod.hh"
#include "relmodel.hh"
#include "grammar.hh"

struct column_constraint : prod {
     string name;
     string characteristics;
     const char *spec;
     column_constraint(prod *parent);
     virtual void out(std::ostream &out);
     virtual ~column_constraint() {  };
     static shared_ptr<column_constraint> factory(prod *parent);
};

struct column_definition : prod {
     shared_ptr<column> created_column;
     vector<shared_ptr<column_constraint> > constraints;
     shared_ptr<const_expr> default_expr;
     column_definition(prod *parent);
     virtual void out(std::ostream &out);
     virtual ~column_definition() {  };
};

struct table_definition : prod {
     table created_table;
     struct scope myscope;
     vector<shared_ptr<column_definition> > column_defs;
     table_definition(prod *parent, struct scope *s);
     virtual void out(std::ostream &out);
     virtual ~table_definition() {  };
     virtual void accept(prod_visitor *v) {
	  for (auto el : column_defs)
	       el->accept(v);
	  v->visit(this);
     }
};

struct check_constraint : column_constraint {
     shared_ptr<bool_expr> expr;
     check_constraint(prod *parent);
     virtual void out(std::ostream &out);
     virtual ~check_constraint() { };
};
     
#endif
