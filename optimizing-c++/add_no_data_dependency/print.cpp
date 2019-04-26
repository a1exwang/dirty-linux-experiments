#include <utils/scan.hpp>
#include <stdint.h>
#include <stdio.h>
#include <chrono>
#include <iostream>
#include <x86intrin.h>
using namespace std;

extern "C" int64_t loop_start(uint64_t n, uint64_t p);

int main() {
  double dt, ipc, ips;
  unsigned long long cycles;
  int64_t max_iters = 0, max_p = 0, max_cycles = 0;
  double max_ipc = 0, max_ips = 0;
  scan_2exp(28, 30, 1, 15, [&](uint64_t i, uint64_t n, uint64_t p) {
    std::tie(cycles, dt, ipc, ips) = scan([&]() {
      loop_start(n, p);
      unsigned long long function_instructions = p + 2;
      unsigned long long instructions = function_instructions * n;
      return instructions;
    });
    if (ipc > max_ipc) {
      max_p = p;
      max_ipc = ipc;
      max_ips = ips;
      max_cycles = cycles;
      max_iters = n;
    }

    cout << "iters: 2^" << i << "=" << n << " "
         << "p: " << p << " "
         << "duration: " << dt << "s "
         << "cycles: " << cycles << " "
         << "IPC: " << ipc << " "
         << "I/s: " << ips << " "
         << "C/s: " << double(cycles)/dt/1e9 << "GHz "
         << endl;
  });

  cout << "iters: " << max_iters << ", p " << max_p << ", max_ipc: " << max_ipc << ", I/s: " << max_ips << "G, cycles: " << max_cycles << endl;
}