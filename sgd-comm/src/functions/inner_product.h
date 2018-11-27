#pragma once

#include <tensor.h>
#include <functions/function.h>
#include <iostream>

namespace asgd {

class InnerProduct :public Function {
public:
  explicit InnerProduct(const std::string &name, int64_t n_in, int64_t n_out, int64_t bs)
      :Function(name, {n_in+1, n_out}, {n_out, bs}), n_in(n_in), n_out(n_out)
  { }

  void setup(int64_t bs) override;
  void operator() (Tensor &output, const Tensor &input) const override;
  void df(Tensor &weight_diff, Tensor &input_diff, const Tensor &output_diff, const Tensor &input, const Tensor &output) const override;
  void applyUpdates(const Tensor &input, Dtype lr) override;
private:
  virtual bool learnable() const { return true; };
  // weight_ is (n_in+1) * n_out
  int64_t n_in, n_out;
};

}
