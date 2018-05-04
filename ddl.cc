#include "ddl.hh"

table_definition::table_definition(prod *parent, struct scope *s)
     : prod(parent), created_table(s->stmt_uid("new_tab"), "public", true, true)
{

}

void table_definition::out(std::ostream &out)
{
     out << "create table ";
     out << created_table.ident();
     out << "(";
     out << ")";
}
