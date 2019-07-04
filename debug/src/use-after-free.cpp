// Compile with
//   clang -fsanitize=address -O1 -fno-omit-frame-pointer -g debug/src/use-after-free.cpp
#include <stdlib.h>
int main() {
  char *x = (char*)malloc(10 * sizeof(char*));
  free(x);
  return x[5];
}