#pragma once
#include "function.h"
#include <tensor.h>

#include <string>
#include <vector>

namespace asgd {

class Data :public Function {
public:
  Data(const std::string &name, const std::vector<int64_t> &oshape, int64_t bs) :Function(name, {}, oshape), offset(0), bs(bs) { }
  ~Data() { if (current_batch) delete current_batch; }
  void setup() override;
  const Tensor &operator() (const Tensor &input) override;
  bool learnable() const override { return false; };
private:
  std::vector<Tensor*> data;
  int64_t rows, cols;
  int64_t offset;
  int64_t bs;
  Tensor *current_batch;
};

}
