#ifndef DUT_HH
#define DUT_HH
#include <cerrno>
#include <stdexcept>
#include <cstring>

namespace dut {
  
struct failure : public std::exception {
  std::string errstr;
  const char* what() const throw()
  {
    return errstr.c_str();
  }
  failure(std::string &s) throw() {
    errstr = s;
  };
};

struct broken : failure {
  broken(std::string &s) throw()
    : failure(s) { }
};

struct dut {
  virtual void test(prod *stmt) = 0;
};

#endif
