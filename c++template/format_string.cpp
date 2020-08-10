#include <cstring>
#include <string>
#include <iostream>

template <typename T>
struct ArgTypes {};

template <>
struct ArgTypes<int> {
  static constexpr char format_char = 'd';
};
template <>
struct ArgTypes<double> {
  static constexpr char format_char = 'f';
};

// for simplicity only supports one item in format string
constexpr bool CheckFormat(const char *format_string, char c) {
  return format_string[1] == c;
}

template <typename T>
void SafePrintf(const char *format, T value)
  __attribute__((enable_if(
    CheckFormat(format, ArgTypes<T>::format_char),
    "format not match"))) {
  // do print
}

int main() {
  SafePrintf("%d", 1);
  SafePrintf("%d", 1.1);
}
