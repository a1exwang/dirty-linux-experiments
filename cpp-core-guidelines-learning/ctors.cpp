#include <iostream>
#include <string>
#include <utility>

using namespace std;

class buffer {
public:
  buffer(std::string name) :name(name) { cout << "buffer(name=" << name << ")" << endl; }
  buffer(const buffer &rhs) :name(rhs.name) { cout << "buffer(const buffer&), rhs.name = " << name << endl; }
  buffer(buffer &&rhs) :name(move(rhs.name)) { cout << "buffer(buffer&&), rhs.name = " << name << endl; }
private:
  string name;
};

int main() {
  buffer b1("b1");
  buffer b2 = b1;
  buffer b3 = move(buffer("b3"));
  // This is an initialization, not copy
  buffer b4 = buffer("b4");

  return 0;
}
