#include <atomic>
#include <iostream>
#include <typeinfo>
#include <vector>

typedef int arr[32];
int func_ref_array(arr &x) {
  return x[0];
}
int func_ptr_array(arr *x) {
  return (*x)[0];
}

template <typename T>
class TypeDisplayer;

std::vector<bool> get_vb() {
  return std::vector<bool>(10, true);
}

template <typename ... Ps>
std::vector<int> make_par(Ps &&... others) {
  return std::vector<int>(std::forward<Ps>(others)...);
}

template <typename ... Ps>
std::vector<int> make_brace(Ps &&... others) {
  return std::vector<int>{std::forward<Ps>(others)...};
}

int main() {
  arr x;
  x[0] = 123;
  std::cout << func_ref_array(x) << std::endl;
  std::cout << func_ptr_array(&x) << std::endl;

  int a;
  auto &ra = a;
  decltype(ra) b = a;
  auto l = [](int a, int b) -> int { return 1; };
//  TypeDisplayer<decltype(l)> a2;
  auto vb5 = get_vb()[5];
  std::cout << "vb5 " << vb5 << std::endl;
  std::cout << make_par(2, 0)[0] << std::endl;
  std::cout << make_brace(2, 0)[0] << std::endl;
  std::forward()
}