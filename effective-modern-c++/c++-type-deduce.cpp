#include <iostream>

typedef int arr[32];
int func_ref_array(arr &x) {
  return x[0];
}

int main() {
  arr x;
  x[0] = 123;
  std::cout << func_ref_array(x) << std::endl;
}