#include <unistd.h>
#include <time.h>
#include <cstdint>
#include <chrono>
#include <iostream>

constexpr int64_t BILLION = 1000UL * 1000UL * 1000UL;
void test_timer(int64_t ns) {
  timespec requested_time{ns/BILLION, ns%BILLION};
  timespec remaining{};

  auto t0 = std::chrono::high_resolution_clock::now();
  while (nanosleep(&requested_time, &remaining) != 0 && errno == EINTR) {
    requested_time = remaining;
  }
  auto t1 = std::chrono::high_resolution_clock::now();
  auto elapsed_ns = std::chrono::duration<int64_t, std::nano>(t1 - t0).count();

  std::cout << "Elapsed " << elapsed_ns << "ns, Requested " << ns << "ns, diff " << labs(elapsed_ns-ns) << "ns, rel " << 2*100.0*labs(elapsed_ns-ns)/(elapsed_ns+ns) << "%" << std::endl;
}

int main() {
  int64_t ns = 1;
  for (int64_t i = 0 ; i < 29; i++) {
    test_timer(ns << i);
  }
  return 0;
}