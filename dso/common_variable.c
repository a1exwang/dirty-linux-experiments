#include "common_variable.h"
#include <stdio.h>

int print_x();
int main() {
  print_x();
  printf("a=%d, &a=%p\n", a, &a);
}