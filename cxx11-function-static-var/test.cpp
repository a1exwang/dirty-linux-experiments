#include <iostream>
#include <string>
#include <mutex>
#include <chrono>
#include <thread>
#include <vector>

/**
 * Compile with clang++ -o test test.cpp
 * View assembly with `objdump -sd test | c++filt` and see assembly of thread_callback()
 */

class Type {
public:
  Type(const std::string &s) :s(s), m() { 
    std::unique_lock<std::mutex> _(m);
    std::cout << "Type::Type(): " << s << std::endl;
  }
  void print() {
    std::unique_lock<std::mutex> _(m);
    std::cout << "Type::print(): " << s << std::endl;
  }
  std::mutex m;
  std::string s;
};

template <int i>
void thread_callback() {
  static Type t("hello");
  std::this_thread::sleep_for(std::chrono::seconds(1));
  t.print();
}

int main() {
  std::vector<std::unique_ptr<std::thread>> threads;
  for (int i = 0; i < 10; i++) {
    threads.push_back(std::make_unique<std::thread>(thread_callback<123>));
  }
  for (const auto &thread : threads) {
    thread->join();
  }
  return 0;
}
