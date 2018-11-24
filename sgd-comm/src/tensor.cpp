#include <tensor.h>
#include <cassert>
#include <iostream>


namespace asgd {


asgd::Tensor &&asgd::Tensor::operator*(const asgd::Tensor &rhs) const {
  assert(this->size() == rhs.size());
  Tensor t(this->shape_);
  for (int64_t i = 0; i < this->size(); i++) {
    t[i] = (*this)[i] * rhs[i];
  }
  return std::move(t);
}

asgd::Dtype asgd::Tensor::scalar() const { assert(this->size() == 1); return *data; }

asgd::Tensor &&asgd::Tensor::operator*(asgd::Dtype rhs) const {
  Tensor t(this->shape_);
  for (int64_t i = 0; i < this->size(); i++) {
    t[i] = data[i] * rhs;
  }
  return std::move(t);
}

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


}
