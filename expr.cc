#include <numeric>
#include <algorithm>
#include <stdexcept>
#include <memory>
#include <cassert>

#include "random.hh"
#include "relmodel.hh"
#include "grammar.hh"
#include "schema.hh"

#include "expr.hh"

using namespace std;

shared_ptr<value_expr> value_expr::factory(prod *p, sqltype *type_constraint)
{
  try {
    if (1 == d20() && p->level < 6)
      return make_shared<coalesce>(p, type_constraint);
    else if (p->scope->refs.size() && d6() > 3)
      return make_shared<column_reference>(p, type_constraint);
    else if (d6()<3)
      return make_shared<funcall>(p, type_constraint);
    else if (d6()<3)
      return make_shared<atomic_subselect>(p, type_constraint);
    else
      return make_shared<const_expr>(p, type_constraint);
  } catch (runtime_error &e) {
  }
  p->retry();
  return factory(p, type_constraint);
}

column_reference::column_reference(prod *p, sqltype *type_constraint) : value_expr(p)
{
  if (type_constraint) {
    auto pairs = scope->refs_of_type(type_constraint);
    for (auto p : pairs)
      assert(p.second.type == type_constraint);
    auto picked = random_pick(pairs);
    reference += picked.first->ident()
      + "." + picked.second.name;
    type = picked.second.type;
    assert(type == type_constraint);
  } else {
    named_relation *r = random_pick(scope->refs);

    reference += r->ident() + ".";
    column &c = random_pick(r->columns());
    type = c.type;
    reference += c.name;
  }
}

shared_ptr<bool_expr> bool_expr::factory(prod *p)
{
  if(d6() < 4)
    return make_shared<comparison_op>(p);
  else if (d6() < 4)
    return make_shared<bool_term>(p);
  else if (d6() < 4)
    return make_shared<null_predicate>(p);
  else if (d6() < 4)
    return make_shared<truth_value>(p);
  else
    return make_shared<exists_predicate>(p);
//     return make_shared<distinct_pred>(q);
}

exists_predicate::exists_predicate(prod *p) : bool_expr(p)
{
  subquery = make_shared<query_spec>(this, scope);
}

void exists_predicate::accept(prod_visitor *v)
{
  v->visit(this);
  subquery->accept(v);
}

void exists_predicate::out(std::ostream &out)
{
  out << "EXISTS (";
  indent(out);
  out << *subquery << ")";
}

distinct_pred::distinct_pred(prod *p) : bool_binop(p)
{
  lhs = make_shared<column_reference>(this);
  rhs = make_shared<column_reference>(this, lhs->type);
}

comparison_op::comparison_op(prod *p) : bool_binop(p)
{
  retry:
  lhs = value_expr::factory(this);
  rhs = value_expr::factory(this);

  auto op_iter =
    scope->schema->find_operator(lhs->type, rhs->type, scope->schema->booltype);

  if (op_iter == scope->schema->index.end())
    { p->retry(); goto retry; }

  oper = &op_iter->second;
}

coalesce::coalesce(prod *p, sqltype *type_constraint) : value_expr(p)
{
  value_exprs.push_back(value_expr::factory(this, type_constraint));
  type = value_exprs[0]->type;
  assert(!type_constraint || type == type_constraint);
  value_exprs.push_back(value_expr::factory(this, type));
  assert(value_exprs[1]->type == value_exprs[0]->type);
}
 
void coalesce::out(std::ostream &out)
{
  out << "cast(coalesce(";
  for (auto expr = value_exprs.begin(); expr != value_exprs.end(); expr++) {
    out << **expr;
    if (expr+1 != value_exprs.end())
      out << "," << endl,indent(out);
  }
  out << ")";
  out << " as " << type->name << ")";
}

const_expr::const_expr(prod *p, sqltype *type_constraint)
    : value_expr(p), expr("")
{
  type = type_constraint ? type_constraint : scope->schema->inttype;
      
  if (type == scope->schema->inttype)
    expr = to_string(d100());
  else if (type == scope->schema->booltype)
    expr += (d6() > 3) ? "true" : "false";
  else if (dynamic_cast<insert_stmt*>(p) && (d6() > 3))
    expr += "default";
  else
    expr += "null";
}

funcall::funcall(prod *p, sqltype *type_constraint)
  : value_expr(p)
{
  if (type_constraint == scope->schema->internaltype)
    throw runtime_error("cannot call functions involving internal type");

  auto &idx = (4 < d6()) ?
    p->scope->schema->routines_returning_type
    : p->scope->schema->parameterless_routines_returning_type;

 retry:
  
  if (!type_constraint) {
    proc = random_pick(idx.begin(), idx.end())->second;
  } else {
    auto iters = idx.equal_range(type_constraint);
    proc = random_pick<>(iters)->second;
    assert(!proc || proc->restype == type_constraint);
  }
  type = proc->restype;

  if (type == scope->schema->internaltype)
    goto retry;
  for (auto type : proc->argtypes)
    if (type == scope->schema->internaltype)
      goto retry;
  
  for (auto type : proc->argtypes)
    parms.push_back(value_expr::factory(this, type));
}

void funcall::out(std::ostream &out)
{
  out << proc->ident() << "(";
  for (auto expr = parms.begin(); expr != parms.end(); expr++) {
    indent(out);
    out << "cast(" << **expr << " as " << (*expr)->type->name << ")";
    if (expr+1 != parms.end())
      out << "," << endl, indent(out);
  }
  out << ")";
}

atomic_subselect::atomic_subselect(prod *p, sqltype *type_constraint)
  : value_expr(p), offset(d42())
{
  auto &idx = p->scope->schema->tables_with_columns_of_type;

  col = 0;
  if (!type_constraint)
    type_constraint = p->scope->schema->inttype;

  auto iters = idx.equal_range(type_constraint);
  tab = random_pick<>(iters)->second;

  for (auto &cand : tab->columns()) {
    if (cand.type == type_constraint) {
      col = &cand;
      break;
    }
  }
  if (!col)
    throw logic_error("bogus index entry");
  type = col->type;
}

void atomic_subselect::out(std::ostream &out)
{
  out << "(select " << col->name << " from " <<
    tab->ident() << " limit 1 offset " << offset << ")"
      << std::endl;
}
