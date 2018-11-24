#pragma once

#include <tensor.h>

#include <string>
#include <vector>

namespace asgd {


class Function {
public:
  explicit Function(
      const std::string &name,
      const std::vector<int64_t> &wshape,
      const std::vector<int64_t> &oshape)
      :name_(name), weight(wshape), output_(oshape) { }
  virtual void setup() { }
  virtual const Tensor &operator() (const Tensor &input) = 0;
  virtual void df(Tensor &input_diff, const Tensor &output_diff, const Tensor &input) { };
  virtual void applyUpdates(Dtype lr) { };

  const Tensor &output() const { return output_; }
  const std::string &name() const { return name_; }

  virtual bool learnable() const = 0;
protected:
  Tensor output_;
  Tensor weight;
  std::string name_;
};

}
