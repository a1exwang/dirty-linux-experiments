#include <utils/scan.hpp>
#include <x86intrin.h>
#include <iostream>
using namespace std;

extern "C" void avxadd(int64_t iters, int64_t instrs);

int main() {
  unsigned long long cycles, max_cycles;
  uint64_t max_p, max_iters;
  double dt, ipc, max_ipc = 0, ips, max_ips;
  scan_2exp(29, 30, 0, 15, [&](uint64_t i, uint64_t n, uint64_t p) {

    double ops = 0;
    std::tie(cycles, dt, ipc, ips) = scan([&]() {
      avxadd(n, p);
      unsigned long long function_instructions = p + 3;
      unsigned long long instructions = function_instructions * n;
      ops = (p * 8 * n) / dt;
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
         << "add-op/s: " << ops/1e12 << "T "
         << "IPC: " << ipc << " "
         << "I/s: " << ips << " "
         << "C/s: " << double(cycles)/dt/1e9 << "GHz "
         << endl;
  });

}