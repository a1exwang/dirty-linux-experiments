#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>

#include "libprime.hpp"

enum class State {
  Integer,
  Decimal,
  Other,
};
void integer_filter(std::istream &is, std::ostream &os,
                    std::function<void(std::ostream &, int64_t)> f) {
  State state = State::Other;
  std::stringstream ss;
  int ch = is.get();
  while (ch >= 0) {
    if (ch >= '0' && ch <= '9') {
      if (state == State::Other || state == State::Integer) {
        state = State::Integer;
        ss << (char)ch;
      } else { // Decimal
        os << (char)ch;
      }
    } else if (ch == '.') {
      state = State::Decimal;
      ss << ".";
    } else {
      // from number to non-number
      if (state == State::Integer) {
        auto value = std::strtol(ss.str().c_str(), 0, 10);
        f(os, value);
        ss.str("");
        ss.clear();
      } else if (state == State::Decimal) {
        os << ss.str();
        ss.str("");
        ss.clear();
      }
      state = State::Other;
      os << (char)ch;
    }
    ch = is.get();
  }
}

template <typename Int>
void decompose(const std::vector<Int> &primes, int64_t value,
               std::vector<Int> &result, size_t start_from = 0) {
  static_assert(std::is_integral<Int>::value);
  auto max_possible = int64_t(std::ceil(std::sqrt(value)));
  bool found = false;
  for (size_t i = start_from; i < primes.size(); i++) {
    if (primes[i] > max_possible) {
      break;
    }
    if (value % primes[i] == 0) {
      result.push_back(primes[i]);
      decompose(primes, value / primes[i], result, i);
      found = true;
      break;
    }
  }
  // value is prime or bigger than we expected
  if (!found && value != 1) {
    result.push_back(value);
  }
}

int main(int argc, char **argv) {
  if (argc != 3) {
    std::cerr << "invalid arguments" << std::endl;
    return 1;
  }

  std::string input_file_path = argv[1];
  std::string output_file_path = argv[2];
  std::ifstream ifs(input_file_path);
  std::ofstream ofs(output_file_path);

  auto primes = prime::get_primes<int64_t>(200);
  integer_filter(
      input_file_path == "-" ? std::cin : ifs,
      output_file_path == "-" ? std::cout : ofs,
      [&](std::ostream &os, int64_t n) {
        std::vector<int64_t> factors;
        decompose<int64_t>(primes, n, factors);
        if (factors.empty()) {
          os << "1";
        } else {
          if (factors.size() > 1) {
            os << "(";
          }
          for (size_t i = 0; i < factors.size(); i++) {
            os << factors[i];
            if (i != factors.size() - 1) {
              os << "*";
            }
          }
          if (factors.size() > 1) {
            os << ")";
          }

        }
      });
}
