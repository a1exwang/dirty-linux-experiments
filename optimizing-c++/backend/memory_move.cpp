#include "utils/scan.hpp"
#include <iostream>
using namespace std;

extern "C" void memory_move(void *dst, void *src, uint64_t count, uint64_t block_size);

int main() {
  // 1GiB
  double dt, ipc, ips;
  unsigned long long cycles;
  scan_2exp(27, 28, 0, 4, [&](uint64_t i, uint64_t n, uint64_t p) {
    double bytes = 0;
    uint64_t block_size = 1UL<<p;
    std::tie(cycles, dt, ipc, ips) = scan([&]() {
      char *src = new char[n*block_size];
      char *dst = new char[n*block_size];
      memory_move(dst, src, n, block_size);
      delete []src;
      delete []dst;
      unsigned long long function_instructions = 5;
      unsigned long long instructions = function_instructions * n;
      return instructions;
    });
    bytes = (block_size* n) / dt;

    cout << "iters: 2^" << i << "=" << n << " "
         << "p: " << p << " "
         << "duration: " << dt << "s "
         << "cycles: " << cycles << " "
         << "mem GB/s: " << bytes/1e9 << " "
         << "IPC: " << ipc << " "
         << "I/s: " << ips << " "
         << "C/s: " << double(cycles)/dt/1e9 << "GHz "
         << endl;
  });
}
