#include <type_traits>
#include <sstream>
#include <iostream>



/**
 * Using template default argument and substitution rules
 */

template <typename T>
using void_t = void;

template <typename T, typename U = void>
struct CanStringstreamType : std::false_type {};

template <typename T>
struct CanStringstreamType<T, void_t<decltype(*(std::stringstream*)(0) << T())>> : std::true_type {};

template <typename SS, typename T>
decltype(((SS&)*(SS*)(0)) << ((T&)*(T*)(0))) CanStringstreamFuncHelper(int);

/*
 * Using SFINAE
 */
struct FailType { };
template <typename SS, typename T>
FailType CanStringstreamFuncHelper(char);

template <typename T>
struct CanStringstreamFunc {
  constexpr static bool value = !std::is_same<FailType, decltype(CanStringstreamFuncHelper<std::stringstream, T>(0))>::value;
};


// Test code
struct A { };
int main() {
  std::cout << CanStringstreamType<int>::value << std::endl;
  std::cout << CanStringstreamType<A>::value << std::endl;
  std::cout << CanStringstreamFunc<int>::value << std::endl;
  std::cout << CanStringstreamFunc<A>::value << std::endl;
}
