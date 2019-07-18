#include <iostream>


class T {
 public:
  T() { std::cout << "T();" << std::endl; }
  ~T() { std::cout << "~T();" << std::endl; }
  T(const T&) { std::cout << "T(const T&)" << std::endl; }
  T &operator=(const T&) { std::cout << "operator=" << std::endl; return *this; }
};

T func() {
  return T();
}

int main() {
  T t = T(T(func()));
}