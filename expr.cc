#include <numeric>
#include <algorithm>
#include <stdexcept>
#include <memory>

#include "random.hh"
#include "relmodel.hh"
#include "grammar.hh"
#include "schema.hh"

#include "expr.hh"

using namespace std;

shared_ptr<value_expr> value_expr::factory(prod *p)
{
  try {
    if (1 == d42())
      return make_shared<const_expr>(p);
    else
      return make_shared<column_reference>(p);
  } catch (runtime_error &e) {
    p->retries++;
    return factory(p);
  }
}

column_reference::column_reference(prod *p) : value_expr(p)
{
  named_relation *r = random_pick(scope->refs);

  if (!r)
    throw runtime_error("Cannot find table reference candidate");
  reference += r->ident() + ".";
  if (!r->columns().size())
    throw runtime_error("Cannot find column candidate");
  column &c = random_pick(r->columns());
  type = c.type;
  reference += c.name;
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
 retry:
  rhs = make_shared<column_reference>(this);
  if (lhs->type != rhs->type)
    { retries++; goto retry; }
}

comparison_op::comparison_op(prod *p) : bool_binop(p)
{
  retry:
  lhs = value_expr::factory(this);
  rhs = value_expr::factory(this);

  auto op_iter =
    scope->schema->find_operator(lhs->type, rhs->type, scope->schema->booltype);

  if (op_iter == scope->schema->index.end())
    { retries++; goto retry; }

  oper = &op_iter->second;
}
