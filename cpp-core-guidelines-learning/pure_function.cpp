#include <iostream>

static const int ok = 10;
static int wtf = 10;

constexpr int add(int a, int b) {
  return a + b + wtf;
}

constexpr int add1(int a, int b) {
  return a + b + ok;
}

