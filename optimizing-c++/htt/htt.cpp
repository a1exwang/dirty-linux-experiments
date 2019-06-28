#include <x86intrin.h>
#include <chrono>
#include <thread>
#include <iostream>
#include <cstdlib>
#include <vector>
using namespace std;

extern "C" int64_t loop_start(uint64_t n, uint64_t p);

/**
 * These sample program illustrates Hyper threading bottlenecks.
 * When your load is ALU-bound, hyper threading will not increase performance.
 *
 * Run the program on a HTT enabled CPU
 *  ./htt YOUR_CORE_COUNT
 *  ./htt YOUR_HYPERTHREAD_COUNT
 *  will result in similar results.
 * However, run
 *  ./htt YOUR_CORE_COUNT
 *  ./htt YOUR_CORE_COUNT/2
 *  should have quite different result(when YOUR_CORE_COUNT >= 2)
 */
int main(int argc, char **argv) {

  int64_t count = 1'000'000'000;
  int64_t ipl = 15;
  int64_t thread_count = atoi(argv[1]);

  {
    vector<thread> threads;
    std::cout << "ALU bound jobs" << std::endl;
    auto t0 = chrono::high_resolution_clock::now();
    for (int thread_id = 0; thread_id < thread_count; thread_id++) {
      threads.emplace_back([=]() {
        auto t0 = chrono::high_resolution_clock::now();
        loop_start(count, ipl - 2); // two additional cmp/jmp instructions
        auto t1 = chrono::high_resolution_clock::now();
        cout << "thread " << thread_id << ": " << chrono::duration<float>(t1 - t0).count() << endl;
      });
    }
    for (auto &thread : threads) {
      thread.join();
    }
    auto t1 = chrono::high_resolution_clock::now();
    cout << "process: " << chrono::duration<float>(t1 - t0).count() << endl;
    cout << "average: " << ipl * count * thread_count / chrono::duration<float>(t1 - t0).count()/1e9 << "G" << endl;
  }

  {
    vector<thread> threads;
    std::cout << "ALU/cache bound jobs" << std::endl;

    auto t0 = chrono::high_resolution_clock::now();
    for (int thread_id = 0; thread_id < thread_count; thread_id++) {
      threads.emplace_back([=]() {
        volatile __m256 a = _mm256_set_ps(0, 0, 0, 1, 0, 0, 0, 0);
        volatile __m256 b = _mm256_set_ps(1, 0, 0, 0, 0, 0, 0, 0);
        volatile __m256 c = _mm256_set_ps(0, 1, 0, 0, 0, 0, 0, 0);
        volatile __m256 d = _mm256_set_ps(0, 0, 1, 0, 0, 0, 0, 0);
        auto t0 = chrono::high_resolution_clock::now();
        for (int64_t i = 0; i < count; i++) {
          c = _mm256_add_ps(a, b);
          a = _mm256_add_ps(c, d);
        }
        (void)a, b, c, d;
        auto t1 = chrono::high_resolution_clock::now();
        cout << "thread " << thread_id << ": " << chrono::duration<float>(t1 - t0).count() << endl;
      });
    }
    for (auto &thread : threads) {
      thread.join();
    }
    auto t1 = chrono::high_resolution_clock::now();

    cout << "process: " << chrono::duration<float>(t1 - t0).count() << endl;
    cout << "average: " << ipl * count * thread_count / chrono::duration<float>(t1 - t0).count()/1e9 << "G" << endl;
  }
  return 0;
}