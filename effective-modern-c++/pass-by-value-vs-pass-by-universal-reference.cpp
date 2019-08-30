#include <utility>
#include <iostream>
#include <tuple>

class Object {
 public:
  Object(std::string name) :name(std::move(name)) {
    std::cout << "Object ctor '" << this->name << "'" << std::endl;
  }
  Object(Object &&obj) noexcept :name(std::move(obj.name)) {
    std::cout << "Object move ctor, new name '" << name << "'" << std::endl;
    obj.name = "";
  }
  Object &operator=(Object &&obj) noexcept {
    std::cout << "Object move assignment, from '" << name << "' to '" << obj.name << "'" << std::endl;
    this->name = std::move(obj.name);
    obj.name = {};
    return *this;
  }
  ~Object() {
    std::cout << "Object destructor '" << name << "'" << std::endl;
  }
 private:
  std::string name{};
};

template <typename T>
class LargeObject1 {
 public:
  template <typename ...Args>
  LargeObject1(Args&& ... args) :value(std::forward<Args>(args)...) {
    std::cout << "LargeObject1 pass-by-universal-reference" << std::endl;
  }
 private:
  T value;
};
class LargeObject2 {
 public:
  LargeObject2(Object value) :value(std::move(value)) {
    std::cout << "LargeObject pass-by-value" << std::endl;
  }
 private:
  Object value;
};


int main() {
  {
    LargeObject1<Object> lo1("obj1");
  }
  {
    LargeObject2 lo2(Object("obj2"));
  }
  int a;
  std::tie(a);
}