#include <utils/scan.hpp>
#include <x86intrin.h>
#include <iostream>
#include <cstring>
using namespace std;


int main() {
  double dt, ips;
  unsigned long long cycles;
  scan_2exp(25, 33, 0, 1, [&](uint64_t i, uint64_t n, uint64_t p) {
    char *dst = new char[n];
    char *src = new char[n];
    double bytes = n;
    memset(src, 0, n);
    memcpy(dst, src, n);

    std::tie(cycles, dt, std::ignore, ips) = scan([&]() {
      memcpy(dst, src, n);
      unsigned long long function_instructions = 10;
      unsigned long long instructions = function_instructions * n;
      return instructions;
    });

    delete []src;
    delete []dst;

    cout << "bytes: 2^" << i << "=" << n << " "
         << "duration: " << dt << "s "
         << "mem GB/s: " << bytes/1e9/dt << " "
         << "C/s: " << double(cycles)/dt/1e9 << "GHz "
         << endl;
  });

}
