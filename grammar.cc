#include <cstdlib>
#include <numeric>
#include <algorithm>

#include "random.hh"
#include "relmodel.hh"
#include "grammar.hh"
#include "schema.hh"

using namespace std;

table_ref *table_ref::factory(scope &s) {
  table_ref *r;
  if (random()%4)
    r = new table_or_query_name(s);
  else if (random()%2)
    r = new joined_table(s);
  else
    r = new table_subquery(s);
  return r;
}

int table_or_query_name::sequence = 0;
table_or_query_name::table_or_query_name(scope &s) {
  t = random_pick<named_relation*>(s.tables);
  ostringstream o;
  o << "rel" << sequence++;
  alias = o.str();
}

void table_or_query_name::out(std::ostream &out) {
  string r("");
  r += t->ident();
  out << r << " as " << ident();
}

int table_subquery::instances;

table_subquery::table_subquery(scope &s) {
  query = new query_spec(s);
  t = new named_relation();
  t->columns = query->sl.derived_table.columns;
  ostringstream r;
  r << "subq_" << instances++;
  t->name = r.str();
}

table_subquery::~table_subquery() {
  delete query;
  delete t;
}

joined_table::joined_table(scope &s) {
 retry:
  lhs = table_ref::factory(s);
  rhs = table_ref::factory(s);
  while (lhs->t == rhs->t) {
    delete rhs; rhs = table_ref::factory(s);
  }
    
  condition = "";

  column c1 = random_pick<column>(lhs->t->columns);
  if (c1.type == "ARRAY"
      || c1.type == "anyarray") {
    delete rhs; delete lhs; goto retry;
  }
  
  for (auto c2 : rhs->t->columns) {
    if (c1.type == c2.type) {
      condition +=
	lhs->ident() + "." + c1.name + " = " + rhs->ident() + "." + c2.name + " ";
      break;
    }
  }
  if (condition == "") {
    delete rhs;
    delete lhs;
    goto retry;
  }

  if (random()&1) {
    type = "inner";
    t = lhs->t;
    alias = lhs->ident();
  } else if (random()&1) {
    type = "left";
    t = lhs->t;
    alias = lhs->ident();
  } else {
    type = "right";
    t = rhs->t;
    alias = rhs->ident();
  }
}

void joined_table::out(std::ostream &out) {
  lhs->out(out);
  out << " " << type << " join ";
  rhs->out(out);
  out << " on (" << condition << ")";
}

void table_subquery::out(std::ostream &out) {
  out << "(";
  query->out(out);
  out << ") as " << t->name;
}

void from_clause::out(std::ostream &out) {
  if (! reflist.size())
    return;
  out << "\n    from ";
  reflist[0]->out(out);
}

from_clause::from_clause(scope &s) {
  reflist.push_back(table_ref::factory(s));
}


value_expr* value_expr::factory(query_spec *q)
{
  value_expr *r;

  if (0==random()%42)
    r = new const_expr();
  else
    r = new column_reference(q);

  if (! r->type.size())
    throw logic_error("generated expr with unknown type");
  return r;
}

column_reference::column_reference(query_spec *q)
{
  table_ref *ref = random_pick<table_ref*>(q->fc.reflist);
  relation *r = ref->t;
  reference += ref->ident() + ".";
  column c = random_pick<column>(r->columns);
  type = c.type;
  reference += c.name;
}

comparison_op::comparison_op(struct query_spec *q) : bool_expr(q)
{
  retry:
  lhs = value_expr::factory(q);
  rhs = value_expr::factory(q);

  auto op_iter =
    schema.find_operator(lhs->type, rhs->type, string("boolean"));
  
  if (op_iter == schema.index.end()) {
    delete lhs; delete rhs; goto retry;
  }
  oper = &op_iter->second;
}

select_list::select_list(query_spec *q)
{
  do {
    value_expr *e = value_expr::factory(q);
    value_exprs.push_back(e);
    ostringstream name;
    name << "c" << columns++;
    derived_table.columns.push_back(column(name.str(), e->type));
  } while (random()%7);
}

void select_list::out(std::ostream &out)
{
  int i = 0;
  for (auto expr = value_exprs.begin(); expr != value_exprs.end(); expr++) {
    (*expr)->out(out);
    out << " as " << derived_table.columns[i].name;
    i++;
    if (expr+1 != value_exprs.end())
      out << ", ";
  }
}

void query_spec::out(std::ostream &out) {
  out << "select " << set_quantifier << " ";
  sl.out(out);
  out << " ";
  fc.out(out);
  out << " where ";
  search.out(out);
  out << limit_clause;
}

query_spec::query_spec(scope &s) : fc(s), sl(this), search(this) {
  
  vector<column> &cols = sl.derived_table.columns;

  if (!count_if(cols.begin(), cols.end(),
		[] (column c) { return c.type == "anyarray"; })) {
    set_quantifier = (random() % 5) ? "" : "distinct ";
  }

  if (0 == random()%3) {
    ostringstream cons;
    cons << " fetch first " << random()%100 + random()%100 << " rows only ";
    limit_clause = cons.str();
  }
}
