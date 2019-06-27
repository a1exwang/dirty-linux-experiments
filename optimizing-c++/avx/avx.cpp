#include <utils/scan.hpp>
#include <x86intrin.h>
#include <iostream>
using namespace std;

extern "C" void avxadd(int64_t iters, int64_t instrs);

int main() {
  unsigned long long cycles;
  double dt, ipc, ips;
  scan_2exp(27, 28, 1, 15, [&](uint64_t i, uint64_t n, uint64_t p) {

    double ops = 0;
    std::tie(cycles, dt, ipc, ips) = scan([&]() {
      avxadd(n, p);
      unsigned long long function_instructions = p + 3;
      unsigned long long instructions = function_instructions * n;
      // 8float op per avx256 register x p op per iter x n iters
      ops = p * 8 * n;
      return instructions;
    });

    cout << "iters: 2^" << i << "=" << n << " "
         << "p: " << p << " "
         << "duration: " << dt << "s "
         << "cycles: " << cycles << " "
         << "GFLOPs: " << ops/1e9/dt << " "
         << "IPC: " << ipc << " "
         << "I/s: " << ips << " "
         << "C/s: " << double(cycles)/dt/1e9 << "GHz "
         << endl;
  });

}