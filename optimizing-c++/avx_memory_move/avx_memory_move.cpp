#include <utils/scan.hpp>
#include <x86intrin.h>
#include <iostream>
using namespace std;

extern "C" void avxmov(void *dst, void *src, uint64_t count, uint64_t reg_count);

int main() {
  double dt, ipc, ips;
  unsigned long long cycles;
  scan_2exp(30, 31, 0, 8, [&](uint64_t i, uint64_t n, uint64_t p) {
    double bytes = 0;
    std::tie(cycles, dt, ipc, ips) = scan([&]() {
      char *dst = new char[n];
      char *src = new char[n];
      avxmov(dst, src, n, p);
      delete []src;
      delete []dst;
      unsigned long long function_instructions = 7 + 4*p;
      unsigned long long instructions = function_instructions * (n/(256 * (1+p)));
      bytes = n;
      return instructions;
    });

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
