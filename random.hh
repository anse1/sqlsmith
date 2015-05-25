#ifndef RANDOM_HH
#define RANDOM_HH

#include <random>

namespace smith {
  extern std::default_random_engine rng;
}

template<typename T> T& random_pick(std::vector<T>& container) {
  std::uniform_int_distribution<int> pick(0, container.size()-1);
  return container[pick(smith::rng)];
}

#endif
