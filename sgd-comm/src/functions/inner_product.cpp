#include <functions/inner_product.h>

#include <random>
#include "inner_product.h"


namespace asgd {

void gemm(Dtype *C, const Dtype *A, const Dtype *B, int64_t M, int64_t N, int64_t K) {
  for (int64_t m = 0; m < M; m++) {
    for (int64_t n = 0; n < N; n++) {
      for (int64_t k = 0; k < K; k++) {
        C[m*K + k] += A[m*N + n] * B[n*K + k];
      }
    }
  }
}


void asgd::InnerProduct::df(Tensor &weight_diff, Tensor &input_diff, const Tensor &output_diff, const Tensor &input, const Tensor &output) const {
  int64_t bs = input.shape()[input.shape().size()-1];
  for (int64_t i_bs = 0; i_bs < bs; i_bs++) {
    // df/dx
    for (int64_t i_in = 0; i_in < this->n_in; i_in++) {
      Dtype val = 0;
      for (int64_t i_out = 0; i_out < this->n_out; i_out++) {
//        input_diff[{i_bs, i_in}] += output_diff[{i_bs, i_out}] * weight_[{i_out, i_in}];
         val += output_diff[i_bs*n_out+i_out] * weight_[i_out*(n_in+1)+i_in];
      }
      input_diff[i_bs*n_in+i_in] = val;
    }
  }
}

void InnerProduct::setup(int64_t bs) {
  std::default_random_engine generator;
  std::normal_distribution<Dtype> distribution(0, 0.1);

  for (int i = 0; i < this->weight_.size(); ++i) {
    this->weight_[i] = 0;
  }
}

void InnerProduct::operator()(Tensor &output, const Tensor &input) const {
  int64_t bs = input.shape()[input.shape().size() - 1];
  for (int64_t i_bs = 0; i_bs < bs; i_bs++) {
    for (int64_t i_out = 0; i_out < n_out; i_out++) {
      Dtype o = 0;
      for (int64_t i_in = 0; i_in < n_in; i_in++) {
//        o += input[{i_bs, + i_in}] * weight_[{i_out, i_in}];
        o += input[i_bs*n_in+i_in] * weight_[i_out*(n_in+1)+i_in];
      }
//      o += weight_[{i_out, n_in}];
      o += weight_[i_out*(n_in+1)+n_in];
//      output[{i_bs, i_out}] = o;
      output[i_bs*n_out+i_out] = o;
    }
  }
}

void InnerProduct::applyUpdates(const Tensor &input, Dtype lr) {
  int64_t bs = input.shape()[input.shape().size()-1];

  // df/db
  for (int64_t i_out = 0; i_out < this->n_out; i_out++) {
    for (int64_t i_bs = 0; i_bs < bs; i_bs++) {
//      weight_diff[{i_out, n_in}] = output_diff[{i_bs, i_out}];
      weight_diff_[i_out * (n_in + 1) + n_in] = output_diff_[i_bs * (n_out) + i_out];
    }
  }

  for (int64_t i = 0; i < weight_diff_.size(); i++) {
    weight_diff_[i] = 0;
  }

  // df/dw
  for (int64_t i_bs = 0; i_bs < bs; i_bs++) {
    for (int64_t i_out = 0; i_out < this->n_out; i_out++) {
      for (int64_t i_in = 0; i_in < this->n_in; i_in++) {
//        weight_diff[{i_out, i_in}] += output_diff[{i_bs, i_out}] * input[{i_bs, i_in}]/bs;
        weight_diff_[i_out*(n_in+1)+i_in] += (output_diff_[i_bs*n_out+i_out] * input[i_bs*n_in+i_in]);
      }
    }
  }
  weight_diff_ /= bs;

  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if (rank == 0) {
    std::cout << "weight_diff: " << "inner product" << " " << std::endl;
    weight_diff_.pprint2(std::cout);
  }

  Function::applyUpdates(input, lr);
}
}
