#include <stdio.h>


void a();
void b() {
  printf("b.cpp:b()\n");
  a();
}
