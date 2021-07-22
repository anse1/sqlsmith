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
  std::string sqlstate;
  std::string errcode;
  const char* what() const throw()
  {
    return errstr.c_str();
  }
  failure(const char *s, const char *sqlstate_ = "", const char *c = "e") throw()
       : errstr(), sqlstate() {
    errstr = s;
    sqlstate = sqlstate_;
    errcode = c;
  };
};

struct broken : failure {
  broken(const char *s, const char *sqlstate_ = "") throw()
    : failure(s, sqlstate_, "C") { }
};

struct timeout : failure {
  timeout(const char *s, const char *sqlstate_ = "") throw()
    : failure(s, sqlstate_, "t") { }
};

struct syntax : failure {
  syntax(const char *s, const char *sqlstate_ = "") throw()
    : failure(s, sqlstate_, "S") { }
};

}

struct dut_base {
  std::string version;
  virtual void test(const std::string &stmt) = 0;
};


#endif
