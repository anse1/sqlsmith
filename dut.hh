/// @file
/// @brief Base class for device under test

#ifndef DUT_HH
#define DUT_HH
#include <stdexcept>
#include <string>

#include "prod.hh"

namespace dut {
  
struct failure : public std::exception {
  std::string errstr;
  const char* what() const throw()
  {
    return errstr.c_str();
  }
  failure(const char *s) throw() {
    errstr = s;
  };
};

struct broken : failure {
  broken(const char *s) throw()
    : failure(s) { }
};

struct timeout : failure {
  timeout(const char *s) throw()
    : failure(s) { }
};

struct syntax : failure {
  syntax(const char *s) throw()
    : failure(s) { }
};

}

struct dut_base {
  std::string version;
  virtual void test(const std::string &stmt) = 0;
};


#endif
