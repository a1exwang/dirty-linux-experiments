#include <string>
#include <vector>
#include <functional>
#include <cassert>
#include <fstream>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <random>


#include "functions.h"


using namespace asgd;
using namespace std;


int64_t asgd::Tensor::size() const {
  return size_;
}

const asgd::Dtype &asgd::Tensor::operator[](int64_t offset) const {
  assert(offset >= 0 && offset < this->size());
  return data[offset];
}

asgd::Dtype &asgd::Tensor::operator[](int64_t offset) {
  assert(offset >= 0 && offset < this->size());
  return data[offset];
}

std::vector<int64_t> Tensor::shape() const {
  return shape_;
}

void Tensor::pprint(std::ostream &os) const {
  os << "Tensor<" << this->shape_[0];
  for (auto s = 1; s < this->shape_.size(); s++) {
    os << "x" << shape_[s];
  }
  os << ">" << std::endl;

  for (int i = 0; i < shape_.size(); i++) {
    os << "[";
  }
  for (int i = 0; i < this->size(); i++) {
    os << this->data[i] << ", ";
  }
  os << "]" << std::endl;
}

const asgd::Tensor &asgd::InnerProduct::operator()(const asgd::Tensor &input) {
  int64_t bs = input.shape()[0];

  for (int64_t i_bs = 0; i_bs < bs; i_bs++) {
    for (int64_t i_out = 0; i_out < n_out; i_out++) {
      this->output_[i_bs * n_out + i_out] = 0;
      for (int64_t i_in = 0; i_in < n_in; i_in++) {
        this->output_[i_bs * n_out + i_out] += input[i_bs * n_in + i_in] * at(i_in, i_out);
      }
      // bias
      this->output_[i_bs * n_out + i_out] += at(n_in, i_out);
    }
  }
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
    for (int64_t i_out = 0; i_out < this->n_out; i_out++) {
      for (int64_t i_in = 0; i_in < this->n_in; i_in++) {
        dfdw[i_bs*wsize + i_out*(n_in+1) + i_in] =
            output_diff[i_bs * n_out + i_out] *
            input[i_bs * n_in + i_in];
      }
      dfdw[i_bs*wsize + i_out*(n_in+1) + n_in] = output_diff[i_bs * n_out + i_out];
    }
  }
}

void InnerProduct::applyUpdates(Dtype lr) {
  int64_t bs = this->output_.shape()[0];
  for (int64_t i_bs = 0; i_bs < bs; i_bs++) {
    for (int64_t i = 0; i < n_in*n_out; i++) {
      this->weight[i] += -lr * this->dfdw[i_bs*(n_in+1)*n_out + i];
    }
  }
}

void InnerProduct::setup() {
  std::default_random_engine generator;
  std::normal_distribution<Dtype> distribution(0, 0.01);

  for (int i = 0; i < this->weight.size(); ++i) {
    this->weight[i] = 1; // distribution(generator);
  }
}

void Net::forward() {
  const Tensor &out = (*fns[0])(Tensor::zero());
  const Tensor *last_out = &out;
  cout << "forward: " << fns[0]->name() << endl;
  last_out->pprint(std::cout);
  for (int i = 1; i < fns.size(); i++) {
    cout << "forward: " << fns[i]->name() << endl;
    last_out->pprint(std::cout);
    last_out = &((*fns[i])(*last_out));
  }
  cout << "forward: " << fns[fns.size()-1]->name() << endl;
  last_out->pprint(std::cout);
  assert(last_out->size() == 1);
  loss_ = last_out->scalar();
}

void Net::backward() {
  auto loss_tensor = Tensor::scalar(1);
  const Tensor *output_diff = &loss_tensor;

  for (int64_t i = fns.size() - 1; i > 0; i--) {
    // FIXME leaky memory
    const auto &input = fns[i-1]->output();
    Tensor *input_diff = new Tensor(input.shape());
    fns[i]->df(*input_diff, *output_diff, input);
//    cout << "Layer " << fns[i]->name() << " Grad: ";
//    result->pprint(std::cout);
    if (i != fns.size() - 1) {
      delete output_diff;
    }
    output_diff = input_diff;
  }
}

void Net::applyUpdate() {
  for (auto &fn : fns) {
    fn->applyUpdates(lr);
  }
}

const Tensor &Data::operator()(const Tensor &input) {
  for (int64_t i_bs = 0; i_bs < bs; i_bs++) {
    const auto &img = *this->data[offset];
    for (int j = 0; j < img.size(); j++) {
      (*current_batch)[i_bs*rows*cols + j] = img[j];
    }
    std::cout << "Data " << offset << ", ";
    offset++;
    if (offset >= this->data.size()) {
      offset = 0;
    }
  }
  return *current_batch;
}

inline uint32_t reverse32(uint32_t value) {
  return (((value & 0x000000FF) << 24) |
          ((value & 0x0000FF00) <<  8) |
          ((value & 0x00FF0000) >>  8) |
          ((value & 0xFF000000) >> 24));
}

void Data::setup() {
  auto filename = "train-images-idx3-ubyte";
  int fd = open(filename, O_RDONLY);
  assert(fd > 0);
  struct stat s;
  assert(stat(filename, &s) >= 0);
  auto fsize = s.st_size;

  char *ptr = (char*)mmap(NULL, fsize, PROT_READ, MAP_PRIVATE | MAP_POPULATE, fd, 0);
//  close(fd);

  assert((*(int32_t*)(ptr + 0)) == reverse32(2051));
  uint32_t imgs = reverse32(*(uint32_t*)(ptr + 4));
  rows = reverse32(*(uint32_t*)(ptr + 8));
  cols = reverse32(*(uint32_t*)(ptr + 12));
  for (int i = 0; i < imgs; i++) {
    const char *base = (ptr + 0x10 + i * rows * cols);
    auto t = new Tensor({rows, cols});
    for (int j = 0; j < rows * cols; j++) {
      (*t)[j] = Dtype((uint8_t)base[j]);
    }
    this->data.push_back(t);
  }

  close(fd);
  munmap(ptr, fsize);
  current_batch = new Tensor({bs, cols*rows});
}

void L2Norm::setup() {
  auto filename = "train-labels-idx1-ubyte";
  int fd = open(filename, O_RDONLY);
  assert(fd > 0);
  struct stat s;
  assert(stat(filename, &s) >= 0);
  size_t fsize = static_cast<size_t>(s.st_size);

  char *ptr = (char*)mmap(NULL, fsize, PROT_READ, MAP_PRIVATE | MAP_POPULATE, fd, 0);
  close(fd);

  assert((*(int32_t*)(ptr + 0)) == reverse32(2049));
  uint32_t labels = reverse32(*(uint32_t*)(ptr + 4));
  for (int i = 0; i < labels; i++) {
    const char *base = (ptr + 0x8 + i);
    auto t = new Tensor({n_in});
    auto label = *(uint8_t*)base;
    (*t)[label] = Dtype(1);
    this->labels.push_back(t);
  }

  munmap(ptr, fsize);
}
