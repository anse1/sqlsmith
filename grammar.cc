#include <numeric>
#include <algorithm>
#include <stdexcept>

#include "random.hh"
#include "relmodel.hh"
#include "grammar.hh"
#include "schema.hh"

using namespace std;

std::ostream& operator<<(std::ostream& s, prod& p)
{
  p.out(s); return s;
}

shared_ptr<table_ref> table_ref::factory(prod *p) {
  if (p->level < d6()) {
    if (d6() <= 3)
      return make_shared<table_subquery>(p);
    else
      return make_shared<joined_table>(p);
  }
  return make_shared<table_or_query_name>(p);
}

int table_or_query_name::sequence = 0;
table_or_query_name::table_or_query_name(prod *p) : table_ref(p) {
  t = random_pick<named_relation*>(scope->tables);
  ostringstream o;
  o << "rel" << sequence++;
  refs.push_back(make_shared<aliased_relation>(o.str(), t));
}

void table_or_query_name::out(std::ostream &out) {
  out << t->ident() << " as " << refs[0]->ident();
}

int table_subquery::instances;

table_subquery::table_subquery(prod *p, bool lateral)
  : table_ref(p), is_lateral(lateral) {
  ostringstream r;
  r << "subq_" << instances++;
  query = make_shared<query_spec>(p, scope);
  string alias = r.str();
  relation *aliased_rel = &query->select_list->derived_table;
  refs.push_back(make_shared<aliased_relation>(alias, aliased_rel));
}

table_subquery::~table_subquery() { }

void table_subquery::accept(prod_visitor *v) {
  query->accept(v);
  v->visit(this);
}

joined_table::joined_table(prod *p) : table_ref(p) {
 retry:
  lhs = table_ref::factory(this);
  rhs = table_ref::factory(this);

  condition = "";

  if (!lhs->refs[0]->columns().size())
    goto retry;

  column &c1 = random_pick<column>(lhs->refs[0]->columns());

  for (auto c2 : rhs->refs[0]->columns()) {
    if (c1.type == c2.type) {
      condition +=
	lhs->refs[0]->ident() + "." + c1.name + " = " + rhs->refs[0]->ident() + "." + c2.name + " ";
      break;
    }
  }
  if (condition == "") {
    goto retry;
  }

  if (d6()<4) {
    type = "inner";
  } else if (d6()<4) {
    type = "left";
  } else {
    type = "right";
  }

  for (auto ref: lhs->refs)
    refs.push_back(ref);
  for (auto ref: rhs->refs)
    refs.push_back(ref);
}

void joined_table::out(std::ostream &out) {
  out << *lhs << " " << type << " join "
      << *rhs << " on (" << condition << ")";
}

void table_subquery::out(std::ostream &out) {
  if (is_lateral)
    out << "lateral";
  out << "(" << *query << ") as " << refs[0]->ident();
}

void from_clause::out(std::ostream &out) {
  if (! reflist.size())
    return;
  out << "\n    from ";

  for (auto r = reflist.begin(); r < reflist.end(); r++) {
    out << **r;
    if (r + 1 != reflist.end())
      out << ",";
  }
}

from_clause::from_clause(prod *p) : prod(p) {
  reflist.push_back(table_ref::factory(this));
  for (auto r : reflist.back()->refs)
    scope->refs.push_back(&*r);

  while (d6() > 5) {
    // add a lateral subquery
    reflist.push_back(make_shared<table_subquery>(p, true));
    for (auto r : reflist.back()->refs)
      scope->refs.push_back(&*r);
  }
}

select_list::select_list(query_spec *q) : prod(q)
{
  do {
    shared_ptr<value_expr> e = value_expr::factory(this);
    value_exprs.push_back(e);
    ostringstream name;
    name << "c" << columns++;
    derived_table.columns().push_back(column(name.str(), e->type));
  } while (d6() > 1);
}

void select_list::out(std::ostream &out)
{
  int i = 0;
  for (auto expr = value_exprs.begin(); expr != value_exprs.end(); expr++) {
    out << **expr << " as " << derived_table.columns()[i].name;
    i++;
    if (expr+1 != value_exprs.end())
      out << ", ";
  }
}

void query_spec::out(std::ostream &out) {
  out << "select " << set_quantifier << " "
      << *select_list << " " << *from_clause << " where " << *search << " "
      << limit_clause;
}

query_spec::query_spec(prod *p, struct scope *s, bool lateral) :
  prod(p)
{
  vector<column> &cols = select_list->derived_table.columns();
  scope = new struct scope(s);
  scope->tables = s->tables;

  if (lateral)
    scope->refs = s->refs;
  
  from_clause = make_shared<struct from_clause>(this);
  select_list = make_shared<struct select_list>(this);
  
//   if (!count_if(cols.begin(), cols.end(),
// 		[] (column c) { return c.type == "anyarray"; })) {
//     set_quantifier = (d<>) == 1) ? "" : "distinct ";
//   }
  set_quantifier = "";

  search = bool_expr::factory(this);

  if (d6() > 2) {
    ostringstream cons;
    cons << " fetch first " << d100() + d100() << " rows only ";
    limit_clause = cons.str();
  }
}

long prepare_stmt::seq;
