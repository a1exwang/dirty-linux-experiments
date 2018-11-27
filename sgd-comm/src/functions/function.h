#pragma once

#include <tensor.h>

#include <string>
#include <vector>
#include <exception>

namespace asgd {


class Function {
public:
  explicit Function(
      const std::string &name,
      const std::vector<int64_t> &wshape,
      const std::vector<int64_t> &oshape)
      :name_(name), weight_(wshape), weight_diff_(wshape), output_(oshape), output_diff_(oshape) { }
  virtual void setup(int64_t bs) { }

  virtual void operator() (Tensor &output, const Tensor &input) const { throw "not implemented"; };
  virtual const Tensor &forward(const Tensor &input) {
    this->operator()(output_, input);
    return output_;
  }

  virtual void df(Tensor &input_diff, const Tensor &input) {
    df(weight_diff_, input_diff, output_diff_, input, output_);
  };
  virtual void df(Tensor &weight_diff, Tensor &input_diff, const Tensor &output_diff, const Tensor &input, const Tensor &output) const { };

  virtual void applyUpdates(const Tensor &input, Dtype lr) {
    if (this->learnable()) {
      for (int64_t i = 0; i < this->weight_diff_.size(); i++) {
        this->weight_[i] += -lr * this->weight_diff_[i];
      }
    }
  };

  const Tensor &output() const { return output_; }
  Tensor &output_diff() { return output_diff_; }
  const std::string &name() const { return name_; }

  virtual bool learnable() const = 0;
protected:
  Tensor output_;
  Tensor output_diff_;
  Tensor weight_;
  Tensor weight_diff_;
  std::string name_;
};

}
