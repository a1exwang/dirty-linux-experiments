#include <utils/benchmark.h>
#include <cassert>
#include <cmath>

using namespace std;


namespace asgd {

template <typename T>
class TimePointName {
public:
};

template <>
class TimePointName<std::chrono::seconds> {
public:
  constexpr static const char *name = "s";
};
template <>
class TimePointName<std::chrono::milliseconds> {
public:
  constexpr static const char *name = "ms";
};
template <>
class TimePointName<std::chrono::microseconds> {
public:
  constexpr static const char *name = "us";
};
template <>
class TimePointName<std::chrono::nanoseconds> {
public:
  constexpr static const char *name = "ns";
};

template <typename T>
void print_duration(ostream &os, chrono::high_resolution_clock::duration duration) {
  using P = typename T::period;
  auto converted_duration = std::chrono::duration<double, P>(duration);
  cout << converted_duration.count() << TimePointName<T>::name;
}
template <typename T>
double get_duration_value(chrono::high_resolution_clock::duration duration) {
  using P = typename T::period;
  auto converted_duration = std::chrono::duration<double, P>(duration);
  return converted_duration.count();
}


void asgd::Benchmark::pprint(std::ostream &os) const {

  std::string name;
  std::vector<TimePoint> time_points_per_metric;
  os << "Benchmark<" << this->name << ">" << endl;
  auto average_on_all_metrics = chrono::high_resolution_clock::duration(0);

  vector<tuple<
      std::chrono::high_resolution_clock::duration,
      std::chrono::high_resolution_clock::duration,
      std::chrono::high_resolution_clock::duration,
      double>> data;

  for (const auto &time_point : time_points) {
    std::tie(name, time_points_per_metric) = time_point;
    assert(time_points_per_metric.size() % 2 == 0);
    auto n = time_points_per_metric.size() / 2;

    std::vector<std::chrono::high_resolution_clock::duration> durations;

    chrono::high_resolution_clock::duration max = chrono::high_resolution_clock::duration(0),
      min = chrono::high_resolution_clock::duration(0),
      sum = chrono::high_resolution_clock::duration(0);

    for (auto i = 0; i < time_points_per_metric.size() / 2; i++) {
      auto &start = time_points_per_metric[2*i];
      auto &end = time_points_per_metric[2*i+1];

      if (!(start.attr() == TimePoint::Start && end.attr() == TimePoint::End)) {
        cerr << "Invalid time point pair" << endl;
        return;
      }
      if (i == 0) {
        min = (end.tp() - start.tp());
      }
      chrono::high_resolution_clock::duration clock_resolution_duration = end.tp() - start.tp();
      durations.push_back(clock_resolution_duration);

      if (clock_resolution_duration > max)
        max = clock_resolution_duration;
      if (clock_resolution_duration < min)
        min = clock_resolution_duration;
      sum += clock_resolution_duration;
    }
    decltype(sum) average = sum / n;
    auto average_value = get_duration_value<OutputTimeFormat>(average);
    double std_dev = 0;
    for (auto duration : durations) {
      auto duration_value = get_duration_value<OutputTimeFormat>(duration);
       std_dev += (duration_value-average_value)*(duration_value-average_value);
    }
    std_dev /= n;
    std_dev = sqrt(std_dev);

    average_on_all_metrics += average;
    data.push_back({average, min, max, std_dev});
  }
  int i = 0;
  for (auto &time_point : time_points) {
    std::tie(name, time_points_per_metric) = time_point;
    assert(time_points_per_metric.size() % 2 == 0);
    auto n = time_points_per_metric.size() / 2;

    chrono::high_resolution_clock::duration max, min, average;
    double std_deviation;
    tie(average, min, max, std_deviation) = data[i++];
    os << "\t<" << name << ">: n=" << n;
    os << ", average=";
    print_duration<OutputTimeFormat>(os, average);
    os << ", std=" << std_deviation << TimePointName<OutputTimeFormat>::name;
    os << ", max=";
    print_duration<OutputTimeFormat>(os, max);
    os << ", min=";
    print_duration<OutputTimeFormat>(os, min);
    os << ", percent=" << double(average.count()) / double(average_on_all_metrics.count())*100 << "%";
    os << endl;
  }
}


}
