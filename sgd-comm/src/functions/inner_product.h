#pragma once

#include <tensor.h>
#include <functions/function.h>
#include <iostream>

namespace asgd {

class InnerProduct :public Function {
public:
  explicit InnerProduct(const std::string &name, int64_t n_in, int64_t n_out, int64_t bs)
      :Function(name, {n_out, n_in+1}, {bs, n_out}), n_in(n_in), n_out(n_out)
  { }

  void setup() override;
  void operator() (Tensor &output, const Tensor &input) const override;
  void df(Tensor &weight_diff, Tensor &input_diff, const Tensor &output_diff, const Tensor &input, const Tensor &output) const override;
  void applyUpdates(Dtype lr) override {
    Function::applyUpdates(lr);

//    for (int i_in = 0; i_in < n_in; i_in++) {
//      for (int i_out = 0; i_out < n_out; i_out++) {
//        std::cout << weight_diff_[{i_out, i_in}] << ", ";
//      }
//      std::cout << std::endl;
//    }
  }
private:
  virtual bool learnable() const { return true; };
  // weight_ is (n_in+1) * n_out
  int64_t n_in, n_out;
};

}
