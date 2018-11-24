#pragma once
#include <string>
#include <vector>
#include <functional>
#include <cassert>
#include <map>
#include <math.h>
#include <cstdlib>
#include <cstring>
#include <iostream>


namespace asgd {
//  class FakeData :public Function {
//  public:
//    FakeData(const std::string &name, Tensor *tensor) :Function(name, {}, tensor->shape()), tensor(tensor) { }
//    const Tensor &operator() (const Tensor &input) override {
//      for (int i = 0; i < this->output_.size(); i++) {
//        this->output_[i] = (*this->tensor)[i];
//      }
//      return *this->tensor;
//    }
//    bool learnable() const override { return false; };
//  private:
//    Tensor *tensor;
//  };
//  class Softmax :public Function {
//  public:
//    Softmax(const std::string &name, int64_t n)
//        :Function(name, {}, {n}), n(n) { }
//
//    const Tensor &operator() (const Tensor &input) override {
//      auto sum = 0;
//      for (int i = 0; i < n; i++) {
//        sum += exp(input[i]);
//      }
//      for (int i = 0; i < n; i++) {
//        output_[i] = exp(input[i]) / sum;
//      }
//      return output_;
//    }
//    void df(Tensor &input_diff, const Tensor &, const Tensor &input) override {
//    }
//
//  private:
//    virtual bool learnable() const { return false; };
//    // weight is (n_in+1) * n_out
//    int64_t n;
//  };
}
