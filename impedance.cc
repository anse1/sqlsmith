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

namespace impedance {
  
bool matched(const char *name)
{
  if (occurances_in_ok_query[name])
    return true;
  if (100 < occurances_in_failed_query[name])
    return false;
  return true;
}

void report()
{
  cerr << "impedance report: " << endl;
  for (auto pair : occurances_in_failed_query) {
    cerr << "  " << pretty_type(pair.first) << ": " <<
      pair.second << "/" << occurances_in_ok_query[pair.first]
	 << " (bad/ok)";
    if (!matched(pair.first))
      cerr << " -> BLACKLISTED";
    cerr << endl;
  }
}

}


