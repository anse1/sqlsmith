#ifndef UTIL_HH
#define UTIL_HH
#include <typeinfo>
#include <string>
#include <sstream>

using namespace std;

inline std::string pretty_type(const char *raw)
{
  ostringstream os;
  os << raw;
  string s = os.str();
  while(s[0] <= '9')
    s.erase(s.begin());
  return s;
}

inline std::string pretty_type(struct prod *p)
{
  return pretty_type(typeid(*p).name());
}

#endif
