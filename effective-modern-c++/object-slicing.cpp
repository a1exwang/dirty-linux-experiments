#include <iostream>


class Parent {
 public:
  ~Parent() {
    std::cout << "~Parent() '" << name << "'" << std::endl;
  }
  explicit Parent(const char *name) :name(name) {
    std::cout << "Parent() '" << name << "'" << std::endl;
  }
  Parent(const Parent &rhs) :name(rhs.name) {
    std::cout << "Parent(const Parent &) '" << name << "'" << std::endl;
  }

  virtual void func() {
    std::cout << "Parent::func() '" << name << "'" << std::endl;
  }
  const char *name = "default";
};

class Child :public Parent {
 public:
  Child(const char* name) :Parent(name) {
    std::cout << "Child() '" << name << "'" << std::endl;
  }
  ~Child() {
    std::cout << "~Child() '" << name << "'" << std::endl;
  }
  void func() override {
    std::cout << "Child::func() " << name << "'" << std::endl;
  }
};

int main() {
  Child c1("child1");
  // By the output, we can know that object slicing is implemented by calling parent class's copy constructor
  // You can try disabling the copy constructor
  Parent p = c1; // NOLINT(cppcoreguidelines-slicing)
  p.name = "parent1";
  p.func();
}
