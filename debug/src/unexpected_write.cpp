#include <iostream>

/**
 * GDB:
 *  - breakpoint
 *  - watch
 *  - x/32bx
 */

struct Data {
  short age;
  char name[10];
};

short filter_age(short age) {
  if (age < 0) {
    std::cerr << "You're not born yet" << std::endl;
    age = 0;
  }
  return age;
}


int main() {
  Data data;
  data.age = 0;
  data.name[0] = '\0';

  std::cout << "Enter your name and age: 'name age\\n'" << std::endl;
  scanf ("%s", data.name);
  scanf ("%d", &data.age);

  data.age = filter_age(data.age);

  std::cout << "name: '" << data.name << "', age: '" << data.age << "'" << std::endl;
}