#include "common_variable.h"
#include <stdio.h>

int print_x() {
  printf("a=%d, &a=%p\n", a, &a);
  return 0;
}