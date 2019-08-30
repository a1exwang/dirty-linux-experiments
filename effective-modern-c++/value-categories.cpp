#include <iostream>

class Object {
 public:
  Object(const char *name) :name(name) { std::cout << "ctor '" << name << "'" << std::endl;  }
  Object(Object &&obj) :name(std::move(obj.name)) {  std::cout << "move ctor new name '" << name << "'" << std::endl; obj.name = "empty"; }
  Object &operator=(Object &&obj) noexcept {
    std::cout << "move assignment from '" << name << "' to '" << obj.name << "'" << std::endl;
    this->name = std::move(obj.name);
    obj.name = {};
    return *this;
  }
  ~Object() { std::cout << "destructor '" << name << "'" << std::endl; }
  std::string name;
};
Object &&get_prvalue(Object& i) {
  return static_cast<Object&&>(i);
}

int main() {
//  int stack_var = 0;
//  const int &a = 1;
//  auto &b = const_cast<int &>(a);
//  auto *c = &b;
//  std::cout << c << ", " << &stack_var << ", " << int64_t((const char*)&a - (const char*)&stack_var) << std::endl;
//  const int *p = 0;
//  auto s = 123[p];
//
//  Object obj1{"obj1"};
//  Object obj2{"obj2"};
//  {
//    Object move_target(get_prvalue(obj2));
//    std::cout << "last in scope1" << std::endl;
//  }
//  std::cout << "after scope1" << std::endl;
}