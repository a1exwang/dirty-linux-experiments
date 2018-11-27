#pragma once
#include <string>
#include <map>
#include <vector>
#include <chrono>
#include <ostream>
#include <iostream>
#include <functional>


namespace asgd {

class TimePoint {
public:
  enum Attribute {
    Start,
    End,
  };
  TimePoint(Attribute attr) :tp_(std::chrono::high_resolution_clock::now()), attr_(attr)  {}

  Attribute attr() const { return attr_; }
  std::chrono::high_resolution_clock::time_point tp() const { return tp_; }
private:
  std::chrono::high_resolution_clock::time_point tp_;
  Attribute attr_;
};

class Benchmark {
public:
  Benchmark(const std::string &name = "benchmark") : name(name) { }

  inline void tick(const std::string &metric_name = "default",
      TimePoint::Attribute attr = TimePoint::Attribute::Start) {
    time_points[metric_name].push_back(TimePoint(attr));
  }
  inline void tick(const std::string &metric_name = "default", std::function<void()> fn = nullptr) {
    time_points[metric_name].push_back(TimePoint(TimePoint::Attribute::Start));
    if (fn)
      fn();
    time_points[metric_name].push_back(TimePoint(TimePoint::Attribute::End));
  }

  void pprint(std::ostream &os) const;
private:
  std::string name;
  std::map<std::string, std::vector<TimePoint>> time_points;
  typedef std::chrono::milliseconds OutputTimeFormat;
};

}
