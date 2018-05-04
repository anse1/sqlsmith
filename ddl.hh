/// @file
/// @brief grammar: Data definition statements

#ifndef DDL_HH
#define DDL_HH

#include "prod.hh"
#include "relmodel.hh"
#include "grammar.hh"

struct table_definition : prod {
     table created_table;
     table_definition(prod *parent, struct scope *s);
     virtual void out(std::ostream &out);
     virtual ~table_definition() {  };
};

#endif
