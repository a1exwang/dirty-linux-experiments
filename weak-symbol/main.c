#include <stdio.h>

// https://en.wikipedia.org/wiki/Weak_symbol 
// Weak symbol can be overriden by a normal(strong) symbol without 
//   "duplication symbol" linking errors.
int __attribute((weak)) add3(int a, int b, int c);

int main() {
  printf("1 + 2 + 3 = %d\n", add3(1, 2, 3));

  return 0;
}
