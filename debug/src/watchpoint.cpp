#include <iostream>
#include <random>
#include <thread>
#include <atomic>


int main(int argc, char **argv) {

  int64_t total_number = atoi(argv[1]);
  int thread_count = atoi(argv[2]);
  double average = 0, stdev = 0;

  std::mt19937 generator(time(nullptr));
  std::normal_distribution<double> distribution(0, 1);

  std::vector<double> data;
  std::vector<std::thread> threads;
  std::atomic<int64_t> i{0};

  for (int tid = 0; tid < thread_count; tid ++) {
    threads.emplace_back([&]() {
      for (; i < total_number; i++) {
        data.push_back(distribution(generator));
      }
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }

  for (double j : data) {
    average += j;
  }
  average /= data.size();

  for (double j : data) {
    stdev += (j - average) * (j - average);
  }
  stdev = sqrt(stdev / data.size());

  std::cout << "n = " << data.size() << ", average = " << average << ", stdev = " << stdev << std::endl;
}