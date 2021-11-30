# borked std::regex

include(CheckCXXSourceCompiles)

# check for a broken std::regex
check_cxx_source_compiles([=[
#include <regex>
#include <iostream>

int main()
{
  std::regex re(".*OK.*");
  auto ret = std::regex_match("This should be OK", re);
  if (!ret) {
    std::cout << "not_matched" << std::endl;
    return 1;
  }

  return 0;
}
]=] std_regex_ok
        FAIL_REGEX not_matched)