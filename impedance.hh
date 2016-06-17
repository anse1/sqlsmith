/// @file
/// @brief feedback to the grammar about failed productions


#ifndef IMPEDANCE_HH
#define IMPEDANCE_HH

#include <typeinfo>
#include <map>
#include "prod.hh"
#include "util.hh"
#include "log.hh"
#include "dut.hh"

struct impedance_visitor : prod_visitor {
  std::map<const char*, long> &_occured;
  std::map<const char*, bool> found;
  virtual void visit(struct prod *p);
  impedance_visitor(std::map<const char*, long> &occured);
  virtual ~impedance_visitor();
};

struct impedance_feedback : logger {
  virtual void executed(prod &query);
  virtual void error(prod &query, const dut::failure &e);
  impedance_feedback() { }
};

namespace impedance {
  bool matched(const char *p);
  inline bool matched(const std::type_info &id) { return matched(id.name()); }
  inline bool matched(prod *p) { return matched(typeid(*p)); }
  void retry(const char *p);
  inline void retry(const std::type_info &id) { return retry(id.name()); }
  inline void retry(prod *p) { return retry(typeid(*p)); }
  void report();
  void report(std::ostream &out);
}

#endif
