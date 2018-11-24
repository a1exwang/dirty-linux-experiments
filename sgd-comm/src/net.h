#pragma once
#include <tensor.h>
#include <functions/function.h>

#include <tuple>

namespace asgd {

class Net {
public:
  Net(std::vector<Function*> fns, Dtype lr, int64_t test_size) :fns(std::move(fns)), loss_(0), lr(lr), test_size(test_size) { }

  void setup() {
    for (auto l : fns) {
      l->setup();
    }
  }
  void forward();
  std::tuple<Dtype, Dtype> test(int64_t test_size);
  void backward();
  void applyUpdate();
  Dtype loss() const { return loss_; };
private:
  std::vector<Function*> fns;
  Dtype loss_;
  Dtype lr;
  int64_t test_size;
};

}
