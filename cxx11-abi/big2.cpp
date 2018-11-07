#define _GLIBCXX_USE_CXX11_ABI 0
#include <iostream>
#include <string>


// Force function with CXX11 ABI
__attribute__ ((abi_tag ("cxx11")))
__attribute__ ((noinline))
 int one1() {
   return 1;
 }

// Force external symbol refernece with CXX11 ABI
__attribute__ ((abi_tag ("cxx11")))
int one2();

// Default to non-CXX11 ABI
int one3();

// When _GLIBCXX_USE_CXX11_ABI is 0, stdlib will not use CXX11 ABI, 
//  so the function is mangled with non-CXX11 ABI
std::string one4();

int main() {
  one2();
  one3();
  one4().c_str();
  printf("Hello, %d\n", one1());
}
