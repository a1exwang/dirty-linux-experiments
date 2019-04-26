#include <stdint.h>
#include <stdio.h>
#include <chrono>
#include <iostream>
#include <x86intrin.h>
using namespace std;

void print_number(int64_t n) {
  printf("number: %ld\n", n);
}
extern "C" int64_t loop_start(uint64_t n, uint64_t p);

int main() {
  int64_t max_iters = 0, max_p = 0, max_cycles = 0;
  double max_ipc = 0, max_ips = 0;
  for (uint64_t i = 0; i < 27; i++) {
    uint64_t loop_count = 1UL << i;

    for (uint64_t p = 0; p <= 14; p++) {
      auto t0 = chrono::high_resolution_clock::now();
      unsigned long long n0 = __rdtsc();
      loop_start(loop_count, p);
      unsigned long long n1 = __rdtsc();
      auto t1 = chrono::high_resolution_clock::now();

      double dt = chrono::duration<double>(t1 - t0).count();
      unsigned long long cycles = n1 - n0;
      unsigned long long function_instructions = p + 3;
      unsigned long long instructions = function_instructions * loop_count;
      double ipc = double(instructions)/cycles;
      double ips = double(instructions)/dt/1e9;
      if (ipc > max_ipc) {
        max_p = p;
        max_ipc = ipc;
        max_ips = ips;
        max_cycles = cycles;
        max_iters = loop_count;
      }

      cout << "2^" << i << "=" << loop_count << " iters, "
           << "p " << p << ", "
           << "duration: " << dt << "s, "
           << "cycles: " << cycles << ", "
           << "instructions: " << instructions << ", "
           << "IPC: " << ipc << ", "
           << "I/s: " << ips << "G, "
           << "C/s: " << double(cycles)/dt/1e9 << "GHz, "
           << endl;

    }
    cout << endl;
  }

  cout << "iters: " << max_iters << ", p " << max_p << ", max_ipc: " << max_ipc << ", I/s: " << max_ips << "G, cycles: " << max_cycles << endl;
}