#include <stdio.h>
void lib2() {
  printf("in my lib2\n");
}
void lib1() {
  printf("in lib1\n");
  printf("calling lib2\n");
  lib2();
}
