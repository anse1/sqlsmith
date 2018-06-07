/// @file
/// @brief grammar: Data definition statements

#ifndef DDL_HH
#define DDL_HH

#include <memory>

#include "prod.hh"
#include "relmodel.hh"
#include "grammar.hh"

struct column_definition : prod {
     shared_ptr<column> created_column;
     column_definition(prod *parent);
     virtual void out(std::ostream &out);
     virtual ~column_definition() {  };
};

struct table_definition : prod {
     table created_table;
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

#endif
