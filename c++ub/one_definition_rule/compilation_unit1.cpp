#include <iostream>

class Data1 {
 public:
  Data1() {
    std::cout << "Data1()" << std::endl;
  }
  ~Data1() {
    std::cout << "~Data1()" << std::endl;
  }
};

struct Context {
  int i1;
  Data1 data;
  double d1;
};

void cu1() {
  std::cout << "Creating Data1 in compilation 1" << std::endl;
  Context context;
}

void cu1();
void cu2();

/**
 * In the entire program, an object or non-inline function cannot have more than one definition.
 * Quote:
 *  "Some violations of the ODR must be diagnosed by the compiler.
 *  Other violations, particularly those that span translation units, are not required to be diagnosed"
 * from https://en.wikipedia.org/wiki/One_Definition_Rule
 *
 * In this example, the class `::Context` is defined twice in compilation_unit1.cpp and compilation_unit2.cpp,
 *  and the definitions are different, which is UB.
 *
 * In this case(on my computer GCC 9.1), the compiler is not required to report a error,
 *   and it may call the wrong constructor for `Data1` and `Data2`.
 * You should not expect the same behavior as me because IT'S UB!
 *
 * Possible ways to detect:
 *  - ld.gold --detect-odr-violations
 *  - Unity build
 *  - ASan: https://github.com/google/sanitizers/wiki/AddressSanitizerOneDefinitionRuleViolation
 */
int main() {
  cu1();
  cu2();
}
