#include "ddl.hh"
#include "relmodel.hh"

table_definition::table_definition(prod *parent, struct scope *s)
     : prod(parent),
       created_table(s->stmt_uid("new_tab"), "public", true, true)
{
     scope = s;

     while (d6() > 1) {
	  column_defs.push_back(std::make_shared<column_definition>(this));
     }
}

void table_definition::out(std::ostream &out)
{
     out << "create table ";
     out << created_table.ident();
     out << "(";

     for (auto defn = column_defs.begin(); defn != column_defs.end(); defn++) {
	  out << **defn;
	  if (defn+1 != column_defs.end())
	       out << ",", indent(out);
     }

     out << ")";
}

column_definition::column_definition(prod *parent)
     : prod(parent)
{
     created_column = std::make_shared<column>(
	  scope->stmt_uid("c"),
	  random_pick(scope->schema->types)
	  );


     if (1 == d6()) {
	  constraints.push_back(std::make_shared<column_constraint>(this));
     }
}
							  
void column_definition::out(std::ostream &out)
{
     out << created_column->name << " " << created_column->type->name;
     for (auto c : constraints)
	  out << " " << *c;
}

column_constraint::column_constraint(prod *parent)
     : prod(parent)
{

}

void column_constraint::out(std::ostream &out)
{
     out << "not null";
}
