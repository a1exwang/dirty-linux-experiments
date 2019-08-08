#include <functional>
#include <iostream>
#include <list>
#include <utility>
#include <vector>

#include "libprime.hpp"

bool match(int a, int b, int c, int d) { return a * b + c == d; }

// template <int max_num, typename T, typename ... Args>
// void get_tuples(std::function<bool (T, Args...)> f) {
//  for (int i = 0; i < max_num; i++) {
//    auto lambda = [&, i](Args... args) {
//      std::cout << "size " << (sizeof...(Args)) << ", i = " << i << std::endl;
//      f(i, args...);
//    };
//
//    get_tuples<max_num, Args...>(std::function<bool (Args...)>(lambda));
//  }
//}
//
// template <int max_num, typename T>
// void get_tuples(std::function<bool (T)> f) {
//  for (int i = 0; i < max_num; i++) {
//    if (f(i)) {
//      std::cout << "size 0, i = " << i << std::endl;
//    }
//  }
//}

int main() {
  auto l = prime::get_primes(50);

  // get_tuples<200, int, int, int, int>(std::function<bool (int, int, int,
  // int)>(match));

  int arity = 4;
  for (int i = 0; i < l.size(); i++) {
    std::cout << "i = " << i << std::endl;
    for (int j = i + 1; j < l.size(); j++) {
      for (int k = j + 1; k < l.size(); k++) {
        if (l[i] + l[j] == 1 + l[k]) {
          std::cout << l[j] << " = " << l[k] << " - " << l[i] << " + 1"
                    << std::endl;
        }
      }
    }
  }
}
