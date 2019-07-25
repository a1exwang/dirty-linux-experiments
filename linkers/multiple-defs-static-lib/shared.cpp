#include <stdio.h>

void a();
void b();
void c();

void shared() {
  printf("shared.cpp:shared()\n");
  a();
  b();
  c();
}
