#pragma once

#include <functional>
#include <iostream>
#include <list>
#include <type_traits>
#include <utility>
#include <vector>

namespace prime {

template <typename Int>
std::vector<Int> get_primes(Int n) {
  static_assert(std::is_integral<Int>::value, "Prime numbers must be integers");
  std::list<Int> numbers;
  for (Int i = 2; i < n; i++) {
    numbers.push_back(i);
  }

  std::vector<Int> result;
  while (!numbers.empty()) {
    auto v = *numbers.begin();
    result.push_back(v);
    // std::cout << "result: " << v << std::endl;
    numbers.erase(numbers.begin());

    auto it = numbers.begin();
    while (it != numbers.end()) {
      if ((*it % v) == 0) {
        it = numbers.erase(it);
      } else {
        it++;
      }
    }
  }
  return std::move(result);
}

}  // namespace prime