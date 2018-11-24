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
  void setup() override;
  virtual std::tuple<Dtype, Dtype> test(const Tensor &input) const = 0;
protected:
  bool learnable() const override { return false; };
  // weight_ is (n_in+1) * n_out
  std::vector<Tensor*> labels;
  std::vector<Tensor*> test_labels;
  int64_t n_in;
  int64_t offset;
  int64_t test_offset;
  int64_t bs;
};

class L2Norm :public Loss {
public:
  L2Norm(const std::string &name, int64_t n_in, int64_t bs) :Loss(name, n_in, bs) { }
  const Tensor &nextLabel() {
    const auto &ret = *labels[offset];
    offset ++;
    if (offset == labels.size()) {
      offset = 0;
    }
    return ret;
  }
  const Tensor &labelAt(int64_t at) const {
    assert(at >= 0);
    return *labels[at % labels.size()];
  }
  void operator()(Tensor &output, const Tensor &input) const override {
    int64_t label_vector_size = labels[0]->size();
    assert(input.size() % label_vector_size == 0);

    Dtype loss = 0;
    int64_t correct = 0;
    for (int i_bs = 0; i_bs < bs; i_bs++) {
      int64_t argmax = 0, argmax_ = 0;
      Dtype max_val = 0, max_val_ = 0;

      const auto &nextLabel_ = labelAt(offset + i_bs);
      for (int i_in = 0; i_in < n_in; i_in++) {
        auto y = nextLabel_[i_in];
        auto y_ = input[{i_bs, i_in}];

        if (y > max_val) {
          max_val = y;
          argmax = i_in;
        }
        if (y_ > max_val_) {
          max_val_ = y_;
          argmax_ = i_in;
        }
        loss += (y - y_)*(y - y_);
      }
      if (argmax == argmax_) {
        correct ++;
      }
    }
    loss /= 2;

//    std::cout << "------------------------_" << std::endl;
//    std::cout << "Label [" << offset << "," << offset+bs <<  "), ";
//    labels[offset]->pprint(std::cout);
//    input.pprint(std::cout);
    std::cout << "Accuracy = " << correct / bs << std::endl;
    output[0] = loss;
  }
  std::tuple<Dtype, Dtype> test(const Tensor &input) const override {
    int64_t label_vector_size = test_labels[0]->size();
    assert(input.size() % label_vector_size == 0);

    Dtype loss = 0, accuracy = 0;
    int correct = 0;
    for (int i_bs = 0; i_bs < bs; i_bs++) {
      int64_t argmax = 0, argmax_ = 0;
      Dtype max_val = 0, max_val_ = 0;
      const auto &nextLabel_ = *test_labels[(offset + i_bs) % test_labels.size()];
      for (int i_in = 0; i_in < n_in; i_in++) {
        auto y = nextLabel_[i_in];
        auto y_ = input[i_bs * n_in + i_in];
        if (y > max_val) {
          max_val = y;
          argmax = i_in;
        }
        if (y_ > max_val_) {
          max_val_ = y_;
          argmax_ = i_in;
        }
        loss += (y - y_)*(y - y_);
      }
      if (argmax == argmax_) {
        correct ++;
      }
    }

    loss /= 2;
    return std::make_tuple(loss, Dtype((double(correct)/bs)));
  }
  void df(Tensor &input_diff, const Tensor &input) override {
    int64_t label_n = labels[0]->size();

    for (int i_bs = 0; i_bs < bs; i_bs++) {
      for (int i_in = 0; i_in < n_in; i_in++) {
        const auto &label = labelAt(offset + i_bs);
        auto y = label[i_in];
        auto y_ = input[{i_bs, i_in}];
        input_diff[{i_bs, i_in}] = (y_ - y);
      }
    }
    this->offset += bs;
    this->offset %= this->labels.size();
  }
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
