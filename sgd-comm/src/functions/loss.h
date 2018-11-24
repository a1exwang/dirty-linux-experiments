#pragma once

#include <functions/function.h>
#include <tensor.h>

#include <vector>
#include <string>
#include <cassert>
#include <iostream>
#include <cmath>


namespace asgd {


class Loss :public Function {
public:
  Loss(const std::string &name, int64_t n_in, int64_t bs) :Function(name, {}, {1}), n_in(n_in), bs(bs), offset(0) { }
  void setup() override;
protected:
  bool learnable() const override { return false; };
  // weight is (n_in+1) * n_out
  std::vector<Tensor*> labels;
  int64_t n_in;
  int64_t offset;
  int64_t bs;
};

class L2Norm :public Loss {
public:
  L2Norm(const std::string &name, int64_t n_in, int64_t bs) :Loss(name, n_in, bs) { }
  const Tensor &operator() (const Tensor &input) override {
    Dtype loss = 0;
    int64_t i_bs = 0;
    int64_t label_n = labels[0]->size();
    assert(input.size() % label_n == 0);
    for (int i = 0; i < input.size(); ++i) {
      if (i != 0 && i % label_n == 0) {
        i_bs++;
      }
      auto label = (*labels[offset + i_bs])[i % label_n];
      auto l = input[i] - label;
      loss += l*l;
    }
    loss /= 2;

    std::cout << "------------------------_" << std::endl;
    std::cout << "Label [" << offset << "," << offset+bs <<  "), ";
    labels[offset]->pprint(std::cout);
    input.pprint(std::cout);
    if (loss == 0) {
      loss = 0;
    }

    this->output_[0] = loss;
    return output_;
  }
  void df(Tensor &input_diff, const Tensor &output_diff, const Tensor &input) override {
    int64_t label_n = labels[0]->size();
    for (int i = 0; i < input_diff.size(); i++) {
      auto label = (*labels[offset + i/label_n])[i%label_n];
      input_diff[i] = (input[i] - label);// * output_diff[0];
    }
    std::cout << "L2Norm grad: ";
    input_diff.pprint(std::cout);

    this->offset += bs;
    if (this->offset >= this->labels.size()){
      this->offset = 0;
    }
  }
};

class CrossEntropy :public Loss {
public:
  CrossEntropy(const std::string &name, int64_t n_in, int64_t bs) :Loss(name, n_in, bs){ }
  const Tensor &operator() (const Tensor &input) override {
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

    this->output_[0] = loss;
    return output_;
  }
  void df(Tensor &input_diff, const Tensor &output_diff, const Tensor &input) override {
    int64_t label_n = labels[0]->size();
    for (int i = 0; i < input_diff.size(); i++) {
      auto label = (*labels[offset + i/label_n])[i%label_n];
      input_diff[i] = -label/input[i] * output_diff[0];
    }

    this->offset += bs;
    if (this->offset >= this->labels.size()){
      this->offset = 0;
    }
  }
};
}
