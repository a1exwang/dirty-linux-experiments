#pragma once
#include "function.h"
#include <tensor.h>

#include <string>
#include <vector>

namespace asgd {

class Data :public Function {
public:
  Data(const std::string &name, const std::vector<int64_t> &oshape, int64_t bs, bool use_mpi)
    :Function(name, {}, oshape), offset(0), test_offset(0), bs(bs), use_mpi(use_mpi) { }
  ~Data() {
    if (current_batch) delete current_batch;
  }
  void setup(int64_t bs) override;
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
  int world_rank, world_size;
  bool use_mpi;
};

}
