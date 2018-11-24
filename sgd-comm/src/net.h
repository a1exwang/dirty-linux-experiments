#pragma once
#include <tensor.h>
#include <functions/function.h>

#include <tuple>

namespace asgd {

class Net {
public:
  Net(std::vector<Function*> fns, Dtype lr, int64_t test_size, int rank, int world_size) :fns(std::move(fns)), loss_(0), lr(lr), test_size(test_size), rank(rank), world_size(world_size) { }

  void setup() {
    for (auto l : fns) {
      l->setup();
    }
  }
  void forward();
  std::tuple<Dtype, Dtype> test(int64_t test_size);
  void backward();
  void applyUpdate();
  std::tuple<Dtype, Dtype> status() const { return std::make_tuple(loss_, acc_); };
private:
  std::vector<Function*> fns;
  Dtype loss_, acc_;
  Dtype lr;
  int64_t test_size;
  int rank, world_size;
};

}
