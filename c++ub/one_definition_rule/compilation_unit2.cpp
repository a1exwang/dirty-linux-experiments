#include <iostream>

class Data2 {
 public:
  Data2() {
    std::cout << "Data2()" << std::endl;
  }
  ~Data2() {
    std::cout << "~Data2()" << std::endl;
  }
};

struct Context {
  int i1;
  Data2 data;
  double d1;
};

void cu2() {
  std::cout << "Creating Data2 in compilation 2" << std::endl;
  Context context;
}
