#ifndef RANDOM_HH
#define RANDOM_HH

#include <random>
#include  <iterator>

namespace smith {
  extern std::default_random_engine rng;
}

template<typename T> T& random_pick(std::vector<T>& container) {
  std::uniform_int_distribution<int> pick(0, container.size()-1);
  return container[pick(smith::rng)];
}

template<typename I>
I random_pick(I beg, I end) {
    std::uniform_int_distribution<> pick(0, std::distance(beg, end) - 1);
    std::advance(beg, pick(smith::rng));
    return beg;
}

#endif
