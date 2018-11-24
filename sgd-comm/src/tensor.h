#pragma once
#include <vector>
#include <string>
#include <cstring>

namespace asgd {

typedef double Dtype;

class Tensor {
public:
  static Tensor zero() { return Tensor(std::vector<int64_t>({})); }
  static Tensor scalar(Dtype n) { auto t = Tensor({1}); *t.data = n; return t; }
  Tensor() :size_(0), shape_({}), data(nullptr) { }
  explicit Tensor(const std::vector<int64_t> &shape) :shape_(shape) {
    if (shape.size() == 0) {
      data = nullptr;
      return;
    }
    size_ = 1;
    for (auto val : shape_) {
      size_ *= val;
    }
    data = new Dtype[this->size()];
    for (int i = 0; i < this->size(); i++) {
      data[i] = Dtype(0);
    }
  }
  Tensor(const Tensor &rhs) :shape_(rhs.shape_), size_(rhs.size_) {
    if (this->data) {
      delete this->data;
    }
    if (rhs.shape().size() == 0) {
      this->data = nullptr;
    }
    this->data = new Dtype[rhs.size()];
    memcpy(this->data, rhs.data, sizeof(Dtype) * rhs.size());
  }
  Tensor &operator=(const Tensor &rhs) {
    if (this->data) {
      delete this->data;
    }
    if (rhs.shape().size() == 0) {
      this->data = nullptr;
    }
    this->shape_ = rhs.shape_;
    this->size_ = rhs.size_;
    this->data = new Dtype[rhs.size()];
    memcpy(this->data, rhs.data, sizeof(Dtype) * rhs.size());
    return *this;
  }
//    Tensor(Tensor &&t) noexcept :shape_(std::move(t.shape_)), data(t.data), size_(t.size_) {}

  void pprint(std::ostream &os) const;

  ~Tensor() {
    delete data;
  }
  bool isZero() { return data == nullptr; }

  std::vector<int64_t> shape() const;
  int64_t size() const;

  const Dtype &operator[](int64_t offset) const;
  Dtype &operator[](int64_t offset);

  Tensor &&operator*(const Tensor &rhs) const;
  Tensor &&operator*(Dtype rhs) const;
  Dtype scalar() const;
private:
  std::vector<int64_t> shape_;
  int64_t size_;
  Dtype *data;
};

}
