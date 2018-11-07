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

// If CXX11 symbol is used, the function is mangled with CXX ABI
std::string one4();

// GCC makes ABI changes at GCC 5.1 to prevent COW string implementation
// according to C++ 11 standard.
//
// Why COW string is forbidden?
// The logic can be seen in the following if COW were allowed: 
//
//  std::string a("something"); 
//  char& c1 = a[0]; 
//  std::string b(a); 
//  char& c2 = a[1]; 
//
//  c1 is a reference to a. You then "copy" a. Then, when you attempt to take the reference the second time, it has to make a copy to get a non-const reference since there are two strings that point to the same buffer. This would have to invalidate the first reference taken, and is against the section quoted above. 
// from: https://stackoverflow.com/questions/12199710/legality-of-cow-stdstring-implementation-in-c11

int main() {
  one2();
  one3();
  one4().c_str();
  printf("Hello, %d\n", one1());
}
