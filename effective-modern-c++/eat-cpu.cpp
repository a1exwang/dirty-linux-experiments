#include <atomic>
#include <chrono>
#include <fstream>
#include <iostream>
#include <regex>
#include <thread>
#include <vector>

auto nproc() -> size_t {
  std::ifstream fs("/proc/cpuinfo");
  std::string line;
  size_t cpus = 0;
  while (getline(fs, line)) {
    std::regex re(R"(processor\s*:\s*\d+)");
    std::smatch base_match;
    if (std::regex_match(line, base_match, re)) {
      cpus++;
    }
  }
  return cpus;
}

struct _e {} eol;
std::mutex lo_lock;

class field_stream :public std::ostream {
 public:
  explicit field_stream(std::ostream &os) :os(os) { }
  template <typename T>
  field_stream &operator<<(const T& rhs) {
    if (empty) {
      os << rhs;
      empty = false;
    } else {
      os << ", " << rhs;
    }
    return *this;
  };
  field_stream &operator<<(const _e &e) {
    os << std::endl;
    empty = true;
    return *this;
  }
 private:
  bool empty = true;
  std::ostream &os;
};

field_stream lo(std::cout);


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
int main(int argc, char **argv) {
  size_t cpu_count = nproc();
  if (argc != 2) {
    std::cerr << "Invalid argument" << std::endl;
  }
  double rate = strtod(argv[1], nullptr);
  size_t grain = 500 * 1000; // 1ms

  std::vector<std::thread> threads;
//  cpu_count = 1;
  for (size_t i = 0; i < cpu_count; i++) {
    threads.emplace_back([rate, grain, i]() {
      if (rate == 1) {
        while (true);
      } else {
        auto sleep_ns = int64_t(grain * (1-rate));
        decltype(std::chrono::high_resolution_clock::now()) last_t;
        auto worked_ns_last = 0;
        bool first_time = true;
        int64_t run_ns_total = 0;
        std::atomic<int64_t> worked_ns = 0;
        std::chrono::duration<int64_t, std::nano> sleep_actual{};
        std::thread([&]() {
          while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            {
              std::unique_lock<std::mutex> _(lo_lock);
              lo << i << run_ns_total << worked_ns << sleep_ns << sleep_actual.count() << double(worked_ns) / (sleep_actual.count()+worked_ns) << eol;
            }
          }
        }).detach();

        while (true) {
          // sleep region start
          auto t0 = std::chrono::high_resolution_clock::now();
          std::this_thread::sleep_for(std::chrono::nanoseconds(sleep_ns));
          auto t1 = std::chrono::high_resolution_clock::now();
          // sleep region end

          worked_ns_last = first_time ? 0 : std::chrono::duration<int64_t, std::nano>(last_t - t0).count();

          sleep_actual = t1 - t0;
          run_ns_total = int64_t(sleep_actual.count() / (1-rate) * rate);

          while (true) {
            last_t = std::chrono::high_resolution_clock::now();
            worked_ns = std::chrono::duration<int64_t, std::nano>(last_t - t1).count() + worked_ns_last;
            if (worked_ns >= run_ns_total) {
              break;
            }
          }
          first_time = false;
//          exit(1);
        }
      }
    });
  }

  for (auto &t : threads) {
    t.join();
  }
}
#pragma clang diagnostic pop