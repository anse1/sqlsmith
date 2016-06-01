#include <typeinfo>
#include <stdexcept>
#include "prod.hh"
#include "impedance.hh"

prod::prod(struct prod *parent)
  : pprod(parent)
{
    if (parent) {
      level = parent->level + 1;
      scope = parent->scope;
    } else {
      scope = 0;
      level = 0;
    }
}

void prod::indent(std::ostream &out)
{
  out << std::endl;
  for (int i = 0; i < level; i++)
    out << "  ";
}

void prod::retry()
{
  if (retries++ > 100000)
    throw std::runtime_error(std::string("excessive retries in ")
			     + typeid(*this).name());
}
