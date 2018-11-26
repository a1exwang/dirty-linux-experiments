#pragma once
#include <vector>
#include <string>
#include <cstring>

namespace asgd {

typedef float Dtype;

class Tensor {
public:
  static Tensor null() { return Tensor(std::vector<int64_t>({})); }

  Tensor() :size_(0), shape_({}), data_(nullptr) {}
  explicit Tensor(const std::vector<int64_t> &shape) :shape_(shape) {
    if (shape.size() == 0) {
      data_ = nullptr;
      return;
    }
    size_ = 1;
    for (auto val : shape_) {
      size_ *= val;
    }
    data_ = new Dtype[this->size()];
    for (int i = 0; i < this->size(); i++) {
      data_[i] = Dtype(0);
    }
    setupRadix();
  }
  Tensor(const Tensor &rhs) :shape_(rhs.shape_), size_(rhs.size_), radixes_(rhs.radixes_) {
    if (this->data_) {
      delete this->data_;
    }
    if (rhs.shape().size() == 0) {
      this->data_ = nullptr;
    }
    this->data_ = new Dtype[rhs.size()];
    memcpy(this->data_, rhs.data_, sizeof(Dtype) * rhs.size());
  }
  Tensor &operator=(const Tensor &rhs) {
    if (this->data_) {
      delete this->data_;
    }
    if (rhs.shape().size() == 0) {
      this->data_ = nullptr;
    }
    this->shape_ = rhs.shape_;
    this->size_ = rhs.size_;
    this->data_ = new Dtype[rhs.size()];
    this->radixes_ = rhs.radixes_;
    memcpy(this->data_, rhs.data_, sizeof(Dtype) * rhs.size());
    return *this;
  }
  ~Tensor() {
    delete data_;
  }

  void pprint(std::ostream &os) const;

  bool isZero() { return data_ == nullptr; }

  std::vector<int64_t> shape() const;
  int64_t size() const;

  const Dtype &operator[](int64_t offset) const;
  Dtype &operator[](int64_t offset);
  Dtype &operator[](const std::vector<int64_t> &indexes);
  const Dtype &operator[](const std::vector<int64_t> &indexes) const;

  Tensor &operator/=(Dtype rhs) {
    for (int64_t i = 0; i < size_; i++) {
      data_[i] /= rhs;
    }
    return *this;
  }

  int64_t argmax() const {
    int64_t ret = 0;
    Dtype max = 0;
    for (int64_t i = 0; i < size_; i++) {
      if (data_[i] > max) {
        max = data_[i];
        ret = i;
      }
    }
    return ret;
  }

//  Tensor &&operator*(const Tensor &rhs) const;
//  Tensor &&operator*(Dtype rhs) const;
  Dtype scalar() const;
  friend std::ostream &operator <<(std::ostream &, const Tensor &);
  Dtype *mutableData() { return data_; }
  const Dtype *data() const { return data_; }
private:
  void setupRadix();
  std::vector<int64_t> shape_;
  int64_t size_;
  Dtype *data_;
  std::vector<int64_t> radixes_;
};

std::ostream &operator <<(std::ostream &, const Tensor &);

}
