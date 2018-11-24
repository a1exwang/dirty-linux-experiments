#include <functions/inner_product.h>

#include <random>
#include "inner_product.h"


namespace asgd {


void asgd::InnerProduct::df(Tensor &weight_diff, Tensor &input_diff, const Tensor &output_diff, const Tensor &input, const Tensor &output) const {
  int64_t bs = input.shape()[0];
  for (int64_t i_bs = 0; i_bs < bs; i_bs++) {
    // df/dx
    for (int64_t i_in = 0; i_in < this->n_in; i_in++) {
      input_diff[{i_bs, i_in}] = 0;
      for (int64_t i_out = 0; i_out < this->n_out; i_out++) {
        input_diff[{i_bs, i_in}] += output_diff[{i_bs, i_out}] * weight_[{i_out, i_in}];
      }
    }
  }
  // df/dw
  for (int64_t i_out = 0; i_out < this->n_out; i_out++) {
    for (int64_t i_in = 0; i_in < this->n_in; i_in++) {
      weight_diff[{i_out, i_in}] = 0;
    }
    for (int64_t i_bs = 0; i_bs < bs; i_bs++) {
      weight_diff[{i_out, n_in}] = output_diff[{i_bs, i_out}];
    }
  }

  for (int64_t i_bs = 0; i_bs < bs; i_bs++) {
    for (int64_t i_out = 0; i_out < this->n_out; i_out++) {
      for (int64_t i_in = 0; i_in < this->n_in; i_in++) {
        weight_diff[{i_out, i_in}] += output_diff[{i_bs, i_out}] * input[{i_bs, i_in}];
      }
    }
  }
  weight_diff /= bs;
}

void InnerProduct::setup() {
  std::default_random_engine generator;
  std::normal_distribution<Dtype> distribution(0, 0.1);

  for (int i = 0; i < this->weight_.size(); ++i) {
    this->weight_[i] = 0;
  }
}

void InnerProduct::operator()(Tensor &output, const Tensor &input) const {
  int64_t bs = input.shape()[0];
  for (int64_t i_bs = 0; i_bs < bs; i_bs++) {
    for (int64_t i_out = 0; i_out < n_out; i_out++) {
      Dtype o = 0;
      for (int64_t i_in = 0; i_in < n_in; i_in++) {
        o += input[{i_bs, + i_in}] * weight_[{i_out, i_in}];
      }
      o += weight_[{i_out, n_in}];
      output[{i_bs, i_out}] = o;
    }
  }
}

}
