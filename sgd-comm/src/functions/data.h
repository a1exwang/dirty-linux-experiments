#pragma once
#include "function.h"
#include <tensor.h>

#include <string>
#include <vector>

namespace asgd {

class Data :public Function {
public:
  Data(const std::string &name, const std::vector<int64_t> &oshape, int64_t bs) :Function(name, {}, oshape), offset(0), test_offset(0), bs(bs) { }
  ~Data() {
    if (current_batch) delete current_batch;
    if (current_test_batch) delete current_test_batch;
  }
  void setup() override;
  const Tensor &forward(const Tensor &input) override;
  const Tensor &test(int64_t test_size);
  bool learnable() const override { return false; };
private:
  std::vector<Tensor*> train_data;
  std::vector<Tensor*> test_data;
  int64_t rows, cols;
  int64_t offset;
  int64_t test_offset;
  int64_t bs;
  Tensor *current_batch;
  Tensor *current_test_batch;
  int world_rank, world_size;
};

}
