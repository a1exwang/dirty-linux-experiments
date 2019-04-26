#include <stdio.h>
#include <stdlib.h>


int j, k, l, m;
int main(int argc, char **argv) {

  int n = atoi(argv[1]);
  for (int i = 0; i < n; i++) {
    j = i + 2;
    k = i + j;
    l = j + k;
    m = k + l;
  }
}
