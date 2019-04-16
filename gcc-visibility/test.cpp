#include <iostream>

using namespace std;
__attribute__((visibility("default"))) int global_fn() {
  return 0;
}
__attribute__((visibility("default"))) int main() {
  cout << "hello, world" << endl;
  return 0;
}
