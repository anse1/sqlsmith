#include <numeric>
#include <algorithm>
#include <stdexcept>
#include <cassert>

#include "random.hh"
#include "relmodel.hh"
#include "grammar.hh"
#include "schema.hh"

using namespace std;

shared_ptr<table_ref> table_ref::factory(prod *p) {
  if (p->level < d6()) {
    if (d6() <= 3)
      return make_shared<table_subquery>(p);
    else
      return make_shared<joined_table>(p);
  }
  if (d6() > 3)
    return make_shared<table_or_query_name>(p);
  else
    return make_shared<table_sample>(p);
}

table_or_query_name::table_or_query_name(prod *p) : table_ref(p) {
  t = random_pick(scope->tables);
  refs.push_back(make_shared<aliased_relation>(scope->stmt_uid("ref"), t));
}

void table_or_query_name::out(std::ostream &out) {
  out << t->ident() << " as " << refs[0]->ident();
}

table_sample::table_sample(prod *p) : table_ref(p) {
  do {
    auto pick = random_pick(scope->tables);
    t = dynamic_cast<struct table*>(pick);
  } while (!t || !t->is_base_table);
  
  refs.push_back(make_shared<aliased_relation>(scope->stmt_uid("sample"), t));
  percent = 0.1 * d100();
  method = (d6() > 2) ? "system" : "bernoulli";
}

void table_sample::out(std::ostream &out) {
  out << t->ident() <<
    " as " << refs[0]->ident() <<
    " tablesample " << method <<
    " (" << percent << ") ";
}


int table_subquery::instances;

table_subquery::table_subquery(prod *p, bool lateral)
  : table_ref(p), is_lateral(lateral) {
  query = make_shared<query_spec>(this, scope, lateral);
  string alias = scope->stmt_uid("subq");
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

  named_relation *left_rel = &*random_pick(lhs->refs);
  
  if (!left_rel->columns().size())
    { retry(); goto retry; }

  named_relation *right_rel = &*random_pick(rhs->refs);

  column &c1 = random_pick(left_rel->columns());

  for (auto c2 : right_rel->columns()) {
    if (c1.type == c2.type) {
      condition +=
	left_rel->ident() + "." + c1.name + " = " + right_rel->ident() + "." + c2.name + " ";
      break;
    }
  }
  if (condition == "") {
    retry(); goto retry;
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
  out << *lhs;
  indent(out);
  out << type << " join " << *rhs;
  indent(out);
  out << "on (" << condition << ")";
}

void table_subquery::out(std::ostream &out) {
  if (is_lateral)
    out << "lateral ";
  out << "(" << *query << ") as " << refs[0]->ident();
}

void from_clause::out(std::ostream &out) {
  if (! reflist.size())
    return;
  out << "from ";

  for (auto r = reflist.begin(); r < reflist.end(); r++) {
    indent(out);
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
    reflist.push_back(make_shared<table_subquery>(this, true));
    for (auto r : reflist.back()->refs)
      scope->refs.push_back(&*r);
  }
}

select_list::select_list(prod *p) : prod(p)
{
  do {
    shared_ptr<value_expr> e = value_expr::factory(this);
    value_exprs.push_back(e);
    ostringstream name;
    name << "c" << columns++;
    derived_table.columns().push_back(column(name.str(), e->type));
  } while (d6() > 2);
}

void select_list::out(std::ostream &out)
{
  int i = 0;
  for (auto expr = value_exprs.begin(); expr != value_exprs.end(); expr++) {
    indent(out);
    out << **expr << " as " << derived_table.columns()[i].name;
    i++;
    if (expr+1 != value_exprs.end())
      out << ", ";
  }
}

void query_spec::out(std::ostream &out) {
  out << "select " << set_quantifier << " "
      << *select_list;
  indent(out);
  out << *from_clause;
  indent(out);
  out << "where ";
  out << *search;
  if (limit_clause.length()) {
    indent(out);
    out << limit_clause;
  }
}

query_spec::query_spec(prod *p, struct scope *s, bool lateral) :
  prod(p)
{
  scope = new struct scope(s);
  scope->tables = s->tables;

  if (lateral)
    scope->refs = s->refs;
  
  from_clause = make_shared<struct from_clause>(this);
  select_list = make_shared<struct select_list>(this);
  
  set_quantifier = "";

  search = bool_expr::factory(this);

  if (d6() > 2) {
    ostringstream cons;
    cons << "limit " << d100() + d100();
    limit_clause = cons.str();
  }
}

long prepare_stmt::seq;

delete_stmt::delete_stmt(prod *p, struct scope *s) : prod(p) {
  scope = 0;
  do {
    struct named_relation *pick = random_pick(s->tables);
    victim = dynamic_cast<struct table*>(pick);
    retry();
  } while (! victim || !victim->is_base_table);

  scope = new struct scope(s);
  scope->tables = s->tables;
  scope->refs.push_back(victim);
  search = bool_expr::factory(this);
}

delete_returning::delete_returning(prod *p, struct scope *s) : delete_stmt(p, s) {
  select_list = make_shared<struct select_list>(this);
}

insert_stmt::insert_stmt(prod *p, struct scope *s) : prod(p)
{
  scope = s;
  do {
    struct named_relation *pick = random_pick(s->tables);
    victim = dynamic_cast<struct table*>(pick);
    retries++;
  } while (! victim || !victim->is_insertable);

  for (auto col : victim->columns()) {
    auto expr = value_expr::factory(this, col.type);
    value_exprs.push_back(expr);
  }
}

void insert_stmt::out(std::ostream &out)
{
  out << "insert into " << victim->ident() << " ";

  if (!value_exprs.size()) {
    out << "default values";
    return;
  }

  out << "values (";
  
  for (auto expr = value_exprs.begin();
       expr != value_exprs.end();
       expr++) {
    indent(out);
    out << **expr;
    if (expr+1 != value_exprs.end())
      out << ", ";
  }
  out << ")";
}

update_stmt::update_stmt(prod *p, struct scope *s) : prod(p) {
  scope = 0;
  do {
    struct named_relation *pick = random_pick(s->tables);
    victim = dynamic_cast<struct table*>(pick);
    retry();
  } while (! victim || !victim->is_base_table || !victim->columns().size());
  
  scope = new struct scope(s);
  scope->tables = s->tables;
  scope->refs.push_back(victim);
  search = bool_expr::factory(this);

  do {
    for (auto col : victim->columns()) {
      if (d6() < 4)
	continue;
      auto expr = value_expr::factory(this, col.type);
      value_exprs.push_back(expr);
      names.push_back(col.name);
    }
  } while (!names.size());
}

update_returning::update_returning(prod *p, struct scope *s) : update_stmt(p, s) {
  select_list = make_shared<struct select_list>(this);
}

void update_stmt::out(std::ostream &out)
{
  assert(names.size());
  out << "update " << victim->ident() << " set ";
  for (size_t i = 0; i < names.size(); i++) {
    indent(out);
    out << names[i] << " = " << *value_exprs[i];
    if (i+1 != names.size())
      out << ", ";
  }
}

shared_ptr<prod> statement_factory(struct scope *s)
{
  s->new_stmt();

  if (d12() == 1)
    return make_shared<insert_stmt>((struct prod *)0, s);
  else if (d12() == 1)
    return make_shared<delete_stmt>((struct prod *)0, s);
  else if (d12() == 1)
    return make_shared<delete_returning>((struct prod *)0, s);
  else if (d12() == 1)
    return make_shared<update_stmt>((struct prod *)0, s);
  else if (d12() == 1)
    return make_shared<update_returning>((struct prod *)0, s);
  else
    return make_shared<query_spec>((struct prod *)0, s);
}
