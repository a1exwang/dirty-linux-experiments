#include <cstddef>
#include <iostream>

template <typename T>
constexpr size_t size0f(const T &t) noexcept { return 0; }

template<>
constexpr size_t size0f<int>(const int &) noexcept {
  return sizeof(int);
}

template<typename T, size_t N>
constexpr size_t size0f(const T (&)[N]) noexcept { // NOLINT(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
  return sizeof(int) * N;
}


int main() {
  static_assert(size0f(0) == sizeof(0));
  int a[32];
  static_assert(size0f(a) == sizeof(a));
  int b;
//  static_assert(size0f(&b) == sizeof(&b));

}