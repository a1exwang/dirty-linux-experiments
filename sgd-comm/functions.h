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

    Tensor &&operator*(const Tensor &rhs) const {
      assert(this->size() == rhs.size());
      Tensor t(this->shape_);
      for (int64_t i = 0; i < this->size(); i++) {
        t[i] = (*this)[i] * rhs[i];
      }
      return std::move(t);
    }
    Tensor &&operator*(Dtype rhs) const {
      Tensor t(this->shape_);
      for (int64_t i = 0; i < this->size(); i++) {
        t[i] = data[i] * rhs;
      }
      return std::move(t);
    }
    Dtype scalar() const { assert(this->size() == 1); return *data; }
  private:
    std::vector<int64_t> shape_;
    int64_t size_;
    Dtype *data;
  };

  class Function {
  public:
    explicit Function(
        const std::string &name,
        const std::vector<int64_t> &wshape,
        const std::vector<int64_t> &oshape)
        :name_(name), weight(wshape), output_(oshape) { }
    virtual void setup() { }
    virtual const Tensor &operator() (const Tensor &input) = 0;
    virtual void df(Tensor &input_diff, const Tensor &output_diff, const Tensor &input) { };
    virtual void applyUpdates(Dtype lr) { };

    const Tensor &output() const { return output_; }
    const std::string &name() const { return name_; }

    virtual bool learnable() const = 0;
  protected:
    Tensor output_;
    Tensor weight;
    std::string name_;
  };

  class Data :public Function {
  public:
    Data(const std::string &name, const std::vector<int64_t> &oshape, int64_t bs) :Function(name, {}, oshape), offset(0), bs(bs) { }
    ~Data() { if (current_batch) delete current_batch; }
    void setup() override;
    const Tensor &operator() (const Tensor &input) override;
    bool learnable() const override { return false; };
  private:
    std::vector<Tensor*> data;
    int64_t rows, cols;
    int64_t offset;
    int64_t bs;
    Tensor *current_batch;
  };

  class FakeData :public Function {
  public:
    FakeData(const std::string &name, Tensor *tensor) :Function(name, {}, tensor->shape()), tensor(tensor) { }
    const Tensor &operator() (const Tensor &input) override {
      for (int i = 0; i < this->output_.size(); i++) {
        this->output_[i] = (*this->tensor)[i];
      }
      return *this->tensor;
    }
    bool learnable() const override { return false; };
  private:
    Tensor *tensor;
  };

  class InnerProduct :public Function {
  public:
    explicit InnerProduct(const std::string &name, int64_t n_in, int64_t n_out, int64_t bs)
      :Function(name, {n_in+1, n_out}, {bs, n_out}), n_in(n_in), n_out(n_out),
       dfdw({bs, n_in+1, n_out})
       { }

    void setup() override;
    const Tensor &operator() (const Tensor &input) override;
    void df(Tensor &input_diff, const Tensor &output_diff, const Tensor &input) override;
    void applyUpdates(Dtype lr) override;
  private:
    virtual bool learnable() const { return true; };
    Dtype at(int64_t i_in, int64_t i_out) const {
      return this->weight[i_out*(1+n_in) + i_in];
    }
    // weight is (n_in+1) * n_out
    int64_t n_in, n_out;
    Tensor dfdw;
  };

  class PureFunction :public Function {
  public:
    PureFunction(const std::string &name, int64_t n, int64_t bs,
        std::function<Dtype(Dtype)> fn, std::function<Dtype(Dtype, Dtype)> derivative)
        :Function(name, {}, {bs, n}), n(n), fn(fn), derivative(derivative) { }

    const Tensor &operator() (const Tensor &input) override {
      int64_t bs = input.shape()[0];
      for (int64_t i_bs = 0; i_bs < bs; i_bs++) {
        for (int i = 0; i < n; i++) {
          output_[i_bs*n + i] = fn(input[i_bs*n + i]);
        }
      }
      return output_;
    }
    void df(Tensor &input_diff, const Tensor &output_diff, const Tensor &input) override {
      for (int64_t i = 0; i < n; i++) {
        input_diff[i] = this->derivative(input[i], output_[i]) * output_diff[i];
      }
    }
  private:
    virtual bool learnable() const { return false; };
    // weight is (n_in+1) * n_out
    int64_t n;
    std::function<Dtype(Dtype)> fn;
    std::function<Dtype(Dtype, Dtype)> derivative;
  };

  class Softmax :public Function {
  public:
    Softmax(const std::string &name, int64_t n)
        :Function(name, {}, {n}), n(n) { }

    const Tensor &operator() (const Tensor &input) override {
      auto sum = 0;
      for (int i = 0; i < n; i++) {
        sum += exp(input[i]);
      }
      for (int i = 0; i < n; i++) {
        output_[i] = exp(input[i]) / sum;
      }
      return output_;
    }
    void df(Tensor &input_diff, const Tensor &, const Tensor &input) override {
    }

  private:
    virtual bool learnable() const { return false; };
    // weight is (n_in+1) * n_out
    int64_t n;
  };

  class Loss :public Function {
  public:
    Loss(const std::string &name, int64_t n_in, int64_t bs) :Function(name, {}, {1}), n_in(n_in), bs(bs), offset(0) { }
    void setup() override;
  protected:
    bool learnable() const override { return false; };
    // weight is (n_in+1) * n_out
    std::vector<Tensor*> labels;
    int64_t n_in;
    int64_t offset;
    int64_t bs;
  };

  class L2Norm :public Loss {
  public:
    L2Norm(const std::string &name, int64_t n_in, int64_t bs) :Loss(name, n_in, bs) { }
    const Tensor &operator() (const Tensor &input) override {
      Dtype loss = 0;
      int64_t i_bs = 0;
      int64_t label_n = labels[0]->size();
      assert(input.size() % label_n == 0);
      for (int i = 0; i < input.size(); ++i) {
        if (i != 0 && i % label_n == 0) {
          i_bs++;
        }
        auto label = (*labels[offset + i_bs])[i % label_n];
        auto l = input[i] - label;
        loss += l*l;
      }
      loss /= 2;

      std::cout << "------------------------_" << std::endl;
      std::cout << "Label [" << offset << "," << offset+bs <<  "), ";
      labels[offset]->pprint(std::cout);
      input.pprint(std::cout);
      if (loss == 0) {
        loss = 0;
      }

      this->output_[0] = loss;
      return output_;
    }
    void df(Tensor &input_diff, const Tensor &output_diff, const Tensor &input) override {
      int64_t label_n = labels[0]->size();
      for (int i = 0; i < input_diff.size(); i++) {
        auto label = (*labels[offset + i/label_n])[i%label_n];
        input_diff[i] = (input[i] - label);// * output_diff[0];
      }

      this->offset += bs;
      if (this->offset >= this->labels.size()){
        this->offset = 0;
      }
    }
  };

  class CrossEntropy :public Loss {
  public:
    CrossEntropy(const std::string &name, int64_t n_in, int64_t bs) :Loss(name, n_in, bs){ }
    const Tensor &operator() (const Tensor &input) override {
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

      this->output_[0] = loss;
      return output_;
    }
    void df(Tensor &input_diff, const Tensor &output_diff, const Tensor &input) override {
      int64_t label_n = labels[0]->size();
      for (int i = 0; i < input_diff.size(); i++) {
        auto label = (*labels[offset + i/label_n])[i%label_n];
        input_diff[i] = -label/input[i] * output_diff[0];
      }

      this->offset += bs;
      if (this->offset >= this->labels.size()){
        this->offset = 0;
      }
    }
  };

  class Net {
  public:
    Net(std::vector<Function*> fns, Dtype lr) :fns(std::move(fns)), loss_(0), lr(lr) { }

    void setup() {
      for (auto l : fns) {
        l->setup();
      }
    }
    void forward();
    void backward();
    void applyUpdate();
    Dtype loss() const { return loss_; };
  private:
    std::vector<Function*> fns;
    Dtype loss_;
    Dtype lr;
  };
}
