struct foo {
  foo() = default;
  foo(const foo&) = delete;
};

int main() {
  auto x = foo(); // prev-17: error! | 17: prvalue copy elision
}