#include "impedance.hh"
#include "log.hh"
#include <iostream>

using namespace std;

static map<const char*, long> occurances_in_failed_query;
static map<const char*, long> occurances_in_ok_query;

impedance_visitor::impedance_visitor(map<const char*, long> &occured)
  :   _occured(occured)
{ }

void impedance_visitor::visit(struct prod *p)
{
  found[typeid(*p).name()] = true;
}

impedance_visitor::~impedance_visitor()
{
  for(auto pair : found)
    _occured[pair.first]++;
}

void impedance_feedback::executed(prod &query)
{
  impedance_visitor v(occurances_in_ok_query);
  query.accept(&v);
}

void impedance_feedback::error(prod &query, const pqxx::failure &e)
{
  impedance_visitor v(occurances_in_failed_query);
  query.accept(&v);
}

bool impedance::matched(type_info &id)
{
  if (occurances_in_ok_query[id.name()])
    return true;
  if (500 < occurances_in_failed_query[id.name()])
    return false;
  return true;
}

void impedance::report()
{
  cerr << "impedance report: " << endl;
  for (auto pair : occurances_in_failed_query) {
    cerr << "  " << pair.first << ": " <<
      pair.second << "/" << occurances_in_ok_query[pair.first]
	 << " (bad/ok)" << endl;
  }
}
