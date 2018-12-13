#include <stdio.h>

int __attribute((weak)) add(int a, int b) {
  printf("add_slow %d, %d\n", a, b);
  return a + b;
}

int add3(int a, int b, int c) {
  printf("add3 %d, %d, %d\n", a, b, c);
  return add(add(a, b), c);
}

