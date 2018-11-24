#pragma once

#include <tensor.h>
#include <functions/function.h>

namespace asgd {

class InnerProduct :public Function {
public:
  explicit InnerProduct(const std::string &name, int64_t n_in, int64_t n_out, int64_t bs)
      :Function(name, {n_in+1, n_out}, {bs, n_out}), n_in(n_in), n_out(n_out),
       dfdw({bs, n_in+1, n_out})
  { }

  void setup() override;
  const Tensor &operator() (const Tensor &input) override;
  void df(Tensor &input_diff, const Tensor &output_diff, const Tensor &input) override;
  void applyUpdates(Dtype lr) override;
private:
  virtual bool learnable() const { return true; };
  Dtype at(int64_t i_in, int64_t i_out) const {
    return this->weight[i_out*(1+n_in) + i_in];
  }
  // weight is (n_in+1) * n_out
  int64_t n_in, n_out;
  Tensor dfdw;
};

}
