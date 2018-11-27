#include <tensor.h>
#include <cassert>
#include <iostream>
#include "tensor.h"


namespace asgd {


//asgd::Tensor &&asgd::Tensor::operator*(const asgd::Tensor &rhs) const {
//  assert(this->size() == rhs.size());
//  Tensor t(this->shape_);
//  for (int64_t i = 0; i < this->size(); i++) {
//    t[i] = (*this)[i] * rhs[i];
//  }
//  return std::move(t);
//}

asgd::Dtype asgd::Tensor::scalar() const { assert(this->size() == 1); return *data_; }

//asgd::Tensor &&asgd::Tensor::operator*(asgd::Dtype rhs) const {
//  Tensor t(this->shape_);
//  for (int64_t i = 0; i < this->size(); i++) {
//    t[i] = data_[i] * rhs;
//  }
//  return std::move(t);
//}

int64_t asgd::Tensor::size() const {
  return size_;
}

const asgd::Dtype &asgd::Tensor::operator[](int64_t offset) const {
  assert(offset >= 0 && offset < this->size());
  return data_[offset];
}

asgd::Dtype &asgd::Tensor::operator[](int64_t offset) {
  assert(offset >= 0 && offset < this->size());
  return data_[offset];
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

  std::vector<int64_t> index(shape_.size(), 0);
  int64_t overflow = 1;
  int64_t first_overflow = 0;
  while (true) {
    for (int64_t j = 0; j < shape_.size(); j++) {
      index[j] += overflow;
      index[j] %= shape_[j];
      overflow = index[j] / shape_[j];
      if (j == 0)
        first_overflow = overflow;
    }
    if (overflow) {
      // all done
      break;
    }
    os << (*this)[index] << " ";
    if (first_overflow) {
      os << "]" << std::endl << "[";
    }
  }

  for (int i = 0; i < shape_.size(); i++) {
    os << "[";
  }
  for (int i = 0; i < this->size(); i++) {
    os << this->data_[i] << ", ";
  }
  os << "]" << std::endl;
}
std::ostream &operator <<(std::ostream &os, const Tensor &t) {
  os << t;
  return os;
}

Dtype &Tensor::operator[](const std::vector<int64_t> &indexes) {
  assert(indexes.size() == shape_.size());
  for (int i = 0; i < indexes.size(); i++) {
    assert(indexes[i] >= 0 && indexes[i] < shape_[i]);
  }

  int64_t offset = 0;
  for (int i = 0; i < radixes_.size(); i++) {
    offset += indexes[i] * radixes_[i];
  }
  return data_[offset];
}
const Dtype &Tensor::operator[](const std::vector<int64_t> &indexes) const {
  assert(indexes.size() == shape_.size());
  for (int i = 0; i < indexes.size(); i++) {
    assert(indexes[i] >= 0 && indexes[i] < shape_[i]);
  }

  int64_t offset = 0;
  for (int i = 0; i < radixes_.size(); i++) {
    offset += indexes[i] * radixes_[i];
  }
  return data_[offset];
}


void Tensor::setupRadix() {
  int64_t r = 1;
  for (int i = 0; i < shape_.size(); i++) {
    radixes_.push_back(r);
    r *= shape_[i];
  }
}

void Tensor::pprint1(std::ostream &os, int64_t depth, const std::vector<int64_t> &index) const {
  if (depth > shape_.size() - 2) {
    print_indent(os, depth+1); os << "[";
    pprint_vector(os, index);
    print_indent(os, depth+1); os << "]," << std::endl;
  } else {
    auto next_index = index;
    next_index.insert(next_index.begin(), 0);
    auto n = shape_[shape_.size()-1-depth];
    for (int64_t i = 0; i < n; i++) {
      print_indent(os, depth+1);
      os << "[" << std::endl;
      next_index[0] = i;
      pprint1(os, depth+1, next_index);
      print_indent(os, depth+1);
      os << "]," << std::endl;
    }
  }
}

void Tensor::pprint_vector(std::ostream &os, const std::vector<int64_t> &index) const {
  auto new_index = index;
  new_index.insert(new_index.begin(), 0);
  for (int64_t i = 0; i < shape_[0]; i++) {
    new_index[0] = i;
    os << (*this)[new_index] << " ";
  }
}


}
