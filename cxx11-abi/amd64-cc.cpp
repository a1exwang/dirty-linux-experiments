#include <iostream>
#include <cstring>

class Type {
public:
  int64_t add(int64_t a, int64_t b) const;
};

int64_t Type::add(int64_t a, int64_t b) const {
  int x = a + b;
  char data[1024];
  memset(data, 0, 1024);
  return x * x;
}

int main() {
  Type t;
  auto a = t.add(1, 2);
  return a;
}
