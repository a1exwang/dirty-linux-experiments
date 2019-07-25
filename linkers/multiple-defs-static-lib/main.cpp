#include <stdio.h>
void a();
void b();
void c();
void shared();


int main() {
  printf("main\n");
  a();
  b();
  c();
  shared();
}


