#include "relmodel.hh"

map<string, sqltype*> sqltype::typemap;

sqltype * sqltype::get(string n)
{
  auto &p = typemap[n];
  if (p)
    return p;

  p = new sqltype(n);
  return p;
};

