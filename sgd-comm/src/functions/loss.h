#pragma once

#include <functions/function.h>
#include <tensor.h>

#include <vector>
#include <string>
#include <cassert>
#include <iostream>
#include <cmath>
#include <tuple>


namespace asgd {


class Loss :public Function {
public:
  Loss(const std::string &name, int64_t n_in, int64_t bs) :Function(name, {}, {1}), n_in(n_in), bs(bs), offset(0), test_offset(0) { }
  void setup(int64_t bs) override;
  virtual std::tuple<Dtype, Dtype> test(const Tensor &input) = 0;
  Dtype accuracy() const { return acc_; }
protected:
  bool learnable() const override { return false; };
  // weight_ is (n_in+1) * n_out
  std::vector<Tensor*> labels;
  std::vector<Tensor*> test_labels;
  int64_t n_in;
  int64_t offset;
  int64_t test_offset;
  int64_t bs;
  Dtype acc_;
};

class L2Norm :public Loss {
public:
  L2Norm(const std::string &name, int64_t n_in, int64_t bs, bool use_mpi);
  const Tensor &labelAt(int64_t at) const {
    assert(at >= 0);
    return *labels[at % labels.size()];
  }

  const Tensor &forward(const Tensor &input) override;
  void df(Tensor &input_diff, const Tensor &input) override;
  std::tuple<Dtype, Dtype> test(const Tensor &input) override;
private:
  int world_rank, world_size;
  bool use_mpi;
};

class CrossEntropy :public Loss {
public:
  CrossEntropy(const std::string &name, int64_t n_in, int64_t bs) :Loss(name, n_in, bs){ }
  void operator() (Tensor &output, const Tensor &input) const override {
    Dtype loss = 0;
    int64_t i_bs = 0;
    int64_t label_n = labels[0]->size();
    assert(input.size() % label_n == 0);
    for (int i = 0; i < input.size(); ++i) {
      if (i % label_n == 0) {
        i_bs++;
      }
      auto label = (*labels[offset + i_bs])[i % label_n];
      auto l = -log(input[i]) * label;
      loss += l;
    }

    std::cout << "Label [" << offset << "," << offset+bs <<  "), ";
    labels[offset]->pprint(std::cout);
    input.pprint(std::cout);

    output[0] = loss;
  }
  void df(Tensor &input_diff, const Tensor &input) override {
    int64_t label_n = labels[0]->size();
    for (int i = 0; i < input_diff.size(); i++) {
      auto label = (*labels[offset + i/label_n])[i%label_n];
      input_diff[i] = -label/input[i] * output_diff_[0];
    }

    this->offset += bs;
    if (this->offset >= this->labels.size()){
      this->offset = 0;
    }
  }
};
}
