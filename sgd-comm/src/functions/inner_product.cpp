#include <functions/inner_product.h>

#include <random>


namespace asgd {


const asgd::Tensor &asgd::InnerProduct::operator()(const asgd::Tensor &input) {
  int64_t bs = input.shape()[0];

  for (int64_t i_bs = 0; i_bs < bs; i_bs++) {
    for (int64_t i_out = 0; i_out < n_out; i_out++) {
      Dtype o = 0;
      for (int64_t i_in = 0; i_in < n_in; i_in++) {
        o += input[i_bs * n_in + i_in] * at(i_in, i_out);
      }
      // bias
      o += at(n_in, i_out);
      this->output_[i_bs * n_out + i_out] = o;
    }
  }
//  cout << "bias: ";
//  for (int i = 0; i < n_out; i++) {
//    cout << at(n_in, i) << ", ";
//  }
//  cout << endl;
  return this->output_;
}

void asgd::InnerProduct::df(Tensor &input_diff, const Tensor &output_diff, const Tensor &input) {
  int64_t bs = input.shape()[0];
  int64_t wsize = ((1+n_in)*n_out);
  for (int64_t i_bs = 0; i_bs < bs; i_bs++) {
    // df/dx
    for (int64_t i_in = 0; i_in < this->n_in; i_in++) {
      input_diff[i_bs * n_in + i_in] = 0;
      for (int64_t i_out = 0; i_out < this->n_out; i_out++) {
        input_diff[i_bs * n_in + i_in] += output_diff[i_bs * n_out + i_out] * at(i_in, i_out);
      }
    }
    // df/dw
//    cout << "-------------------------" << endl;
//    cout << "df/db: ";
    for (int64_t i_out = 0; i_out < this->n_out; i_out++) {
      for (int64_t i_in = 0; i_in < this->n_in; i_in++) {
        dfdw[i_bs*wsize + i_out*(n_in+1) + i_in] =
            output_diff[i_bs * n_out + i_out] * input[i_bs * n_in + i_in];
      }
      dfdw[i_bs*wsize + i_out*(n_in+1) + n_in] = output_diff[i_bs * n_out + i_out];
//      cout << output_diff[i_bs * n_out + i_out] << ", ";
    }
//    cout << endl;


//    cout << "-----------------------" << endl;
//    cout << "grad_w: ";
    for (int64_t i_in = 0; i_in < this->n_in; i_in++) {
//      cout << "[";
      for (int64_t i_out = 0; i_out < this->n_out; i_out++) {
        auto d = dfdw[i_bs*wsize + i_out*(n_in+1) + i_in];
//        cout << d << ", ";
      }
//      cout << "]" << endl;
    }

  }
}

void InnerProduct::applyUpdates(Dtype lr) {
  int64_t bs = this->output_.shape()[0];
//  cout << "-----------------" << endl;
//  cout << "applyUpdate: ";
//  this->weight.pprint(cout);
  for (int64_t i_bs = 0; i_bs < bs; i_bs++) {
    for (int64_t i = 0; i < n_in*n_out; i++) {
      this->weight[i] += -lr * this->dfdw[i_bs*(n_in+1)*n_out + i];
    }
  }
}

void InnerProduct::setup() {
  std::default_random_engine generator;
  std::normal_distribution<Dtype> distribution(0, 0.1);

  for (int i = 0; i < this->weight.size(); ++i) {
    this->weight[i] = 0;
  }
}

}
