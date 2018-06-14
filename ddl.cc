#include "ddl.hh"
#include "relmodel.hh"

table_definition::table_definition(prod *parent, struct scope *s)
     : prod(parent),
       created_table(s->stmt_uid("new_tab"), "public", true, true),
       myscope(s)
{
     scope = &myscope;
     scope->refs.push_back(&created_table);
     
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

     struct table_definition *tabledef = dynamic_cast<struct table_definition*>(parent);
     if (tabledef)
	  tabledef->created_table.cols.push_back(*created_column);
     
     if (1 == d6())
	  constraints.push_back(column_constraint::factory(this));

     if (1 == d6())
	  default_expr = std::make_shared<const_expr>(this, created_column->type);
}
							  
void column_definition::out(std::ostream &out)
{
     out << created_column->name << " " << created_column->type->name;
     if (default_expr)
	  out << " DEFAULT " << *default_expr << " ";
     for (auto c : constraints)
	  out << " " << *c;
}

column_constraint::column_constraint(prod *parent)
     : prod(parent)
{
     static const char *specs[] = {
	  "UNIQUE", "NOT NULL", "NULL", "PRIMARY KEY", "", ""
     };

     spec = specs[d6()-1];

}

shared_ptr<column_constraint> column_constraint::factory(prod *parent)
{
     if (1 == d6())
	  return std::make_shared<check_constraint>(parent);
     else
	  return std::make_shared<column_constraint>(parent);
}

void column_constraint::out(std::ostream &out)
{
     out << spec;
}

check_constraint::check_constraint(prod *parent)
     : column_constraint(parent)
{
     expr = bool_expr::factory(this);
}

void check_constraint::out(std::ostream &out)
{
     out << "CHECK(" << *expr << ")";
}
