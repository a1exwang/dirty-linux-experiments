#pragma once
#include <functional>
#include <utility>
#include <chrono>
#include <x86intrin.h>

// cycles, dt, ipc, ips
std::tuple<unsigned long long, double, double, double> scan(const std::function<unsigned long long()>& callback) {

  auto t0 = std::chrono::high_resolution_clock::now();
  unsigned long long n0 = __rdtsc();
  auto instructions = callback();
  unsigned long long n1 = __rdtsc();
  auto t1 = std::chrono::high_resolution_clock::now();

  double dt = std::chrono::duration<double>(t1 - t0).count();
  unsigned long long cycles = n1 - n0;

  double ipc = double(instructions) / cycles;
  double ips = double(instructions) / dt;
  return std::tie(cycles, dt, ipc, ips);
}


void scan_2exp(uint64_t i0, uint64_t i1, uint64_t j0, uint64_t j1,
    const std::function<void(uint64_t, uint64_t, uint64_t)>& callback
) {

  for (uint64_t i = i0; i < i1; i++) {
    uint64_t loop_count = 1UL << i;
    for (uint64_t j = j0; j < j1; j++) {
      callback(i, loop_count, j);
    }
  }
}
