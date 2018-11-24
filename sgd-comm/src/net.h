#pragma once
#include <tensor.h>
#include <functions/function.h>

namespace asgd {

class Net {
public:
  Net(std::vector<Function*> fns, Dtype lr) :fns(std::move(fns)), loss_(0), lr(lr) { }

  void setup() {
    for (auto l : fns) {
      l->setup();
    }
  }
  void forward();
  void backward();
  void applyUpdate();
  Dtype loss() const { return loss_; };
private:
  std::vector<Function*> fns;
  Dtype loss_;
  Dtype lr;
};

}
