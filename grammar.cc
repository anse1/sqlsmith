#include <numeric>
#include <algorithm>

#include "random.hh"
#include "relmodel.hh"
#include "grammar.hh"
#include "schema.hh"

using namespace std;

std::ostream& operator<<(std::ostream& s, prod& p)
{
  p.out(s); return s;
}

shared_ptr<table_ref> table_ref::factory(scope &s) {
  shared_ptr<table_ref> r;
  if (d(9) == 1)
    r = make_shared<joined_table>(s);
  else if (d(9) == 1)
    r = make_shared<table_subquery>(s);
  else
    r = make_shared<table_or_query_name>(s);
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
  out << t->ident() << " as " << ident();
}

int table_subquery::instances;

table_subquery::table_subquery(scope &s) {
  ostringstream r;
  r << "subq_" << instances++;
  query = new query_spec(s);
  t = new named_relation(r.str());
  t->columns = query->sl.derived_table.columns;
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
    rhs = table_ref::factory(s);
  }

  condition = "";

  column c1 = random_pick<column>(lhs->t->columns);
  if (c1.type == "ARRAY"
      || c1.type == "anyarray") {
    goto retry;
  }

  for (auto c2 : rhs->t->columns) {
    if (c1.type == c2.type) {
      condition +=
	lhs->ident() + "." + c1.name + " = " + rhs->ident() + "." + c2.name + " ";
      break;
    }
  }
  if (condition == "") {
    goto retry;
  }

  if (d(6)<4) {
    type = "inner";
    t = lhs->t;
    alias = lhs->ident();
  } else if (d(6)<4) {
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
  out << *lhs << " " << type << " join "
      << *rhs << " on (" << condition << ")";
}

void table_subquery::out(std::ostream &out) {
  out << "(" << *query << ") as " << t->name;
}

void from_clause::out(std::ostream &out) {
  if (! reflist.size())
    return;
  out << "\n    from " << *reflist[0];
}

from_clause::from_clause(scope &s) {
  reflist.push_back(table_ref::factory(s));
}


shared_ptr<value_expr> value_expr::factory(query_spec *q)
{
  shared_ptr<value_expr> r;

  if (1 == d(42))
    r = make_shared<const_expr>();
  else
    r = make_shared<column_reference>(q);

  if (! r->type.size())
    throw logic_error("generated expr with unknown type");
  return r;
}

column_reference::column_reference(query_spec *q)
{
  shared_ptr<table_ref> ref = random_pick<shared_ptr<table_ref> >(q->fc.reflist);
  relation *r = ref->t;
  reference += ref->ident() + ".";
  column c = random_pick<column>(r->columns);
  type = c.type;
  reference += c.name;
}

shared_ptr<bool_expr> bool_expr::factory(struct query_spec *q)
{
  if(d(6) < 4)
    return make_shared<comparison_op>(q);
  else if (d(6) < 4)
    return make_shared<bool_term>(q);
  else if (d(6) < 4)
    return make_shared<truth_value>(q);
  else
    return make_shared<null_predicate>(q);
//     return make_shared<distinct_pred>(q);
}

distinct_pred::distinct_pred(struct query_spec *q) : bool_expr(q)
{
  lhs = make_shared<column_reference>(q);
 retry:
  rhs = make_shared<column_reference>(q);
  if (lhs->type != rhs->type)
    goto retry;
}

comparison_op::comparison_op(struct query_spec *q) : bool_expr(q)
{
  retry:
  lhs = value_expr::factory(q);
  rhs = value_expr::factory(q);

  auto op_iter =
    schema.find_operator(lhs->type, rhs->type, string("boolean"));

  if (op_iter == schema.index.end())
    goto retry;

  oper = &op_iter->second;
}

select_list::select_list(query_spec *q)
{
  do {
    shared_ptr<value_expr> e = value_expr::factory(q);
    value_exprs.push_back(e);
    ostringstream name;
    name << "c" << columns++;
    derived_table.columns.push_back(column(name.str(), e->type));
  } while (d(6) > 1);
}

void select_list::out(std::ostream &out)
{
  int i = 0;
  for (auto expr = value_exprs.begin(); expr != value_exprs.end(); expr++) {
    out << **expr << " as " << derived_table.columns[i].name;
    i++;
    if (expr+1 != value_exprs.end())
      out << ", ";
  }
}

void query_spec::out(std::ostream &out) {
  out << "select " << set_quantifier << " "
      << sl << " " << fc << " where " << *search << " "
      << limit_clause;
}

query_spec::query_spec(scope &s) : fc(s), sl(this) {

  vector<column> &cols = sl.derived_table.columns;

//   if (!count_if(cols.begin(), cols.end(),
// 		[] (column c) { return c.type == "anyarray"; })) {
//     set_quantifier = (d(6) == 1) ? "" : "distinct ";
//   }
  set_quantifier = "";

  search = bool_expr::factory(this);

  if (d(6) > 2) {
    ostringstream cons;
    cons << " fetch first " << d(100) + d(100) << " rows only ";
    limit_clause = cons.str();
  }
}

long prepare_stmt::seq;
