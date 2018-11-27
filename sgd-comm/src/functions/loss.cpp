#include <functions/loss.h>
#include <utils/endian.h>

#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>
#include <mpi.h>


using namespace std;
namespace asgd {

vector<Tensor*> readMnistTest(const std::string &filename, int64_t n_in) {
  int fd = open(filename.c_str(), O_RDONLY);
  assert(fd > 0);
  struct stat s;
  int err = stat(filename.c_str(), &s);
  assert(err == 0);
  size_t fsize = static_cast<size_t>(s.st_size);

  char *ptr = (char*)mmap(NULL, fsize, PROT_READ, MAP_PRIVATE | MAP_POPULATE, fd, 0);
  close(fd);

  vector<Tensor*> ret;
  assert((*(int32_t*)(ptr + 0)) == reverse32(2049));
  uint32_t labels = reverse32(*(uint32_t*)(ptr + 4));
  for (int i = 0; i < labels; i++) {
    const char *base = (ptr + 0x8 + i);
    auto t = new Tensor({n_in});
    auto label = *(uint8_t*)base;
    (*t)[label] = Dtype(1);
    ret.push_back(t);
  }

  munmap(ptr, fsize);
  return ret;
}

void Loss::setup(int64_t bs) {
  labels = readMnistTest("train-labels-idx1-ubyte", n_in);
  test_labels = readMnistTest("t10k-labels-idx1-ubyte", n_in);
}


L2Norm::L2Norm(const std::string &name, int64_t n_in, int64_t bs, bool use_mpi) :Loss(name, n_in, bs), use_mpi(use_mpi) {
  if (use_mpi) {
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    offset = bs * world_rank;
  } else {
    offset = 0;
  }
}

const Tensor &L2Norm::forward(const Tensor &input) {
  int64_t label_vector_size = labels[0]->size();
  assert(input.size() % label_vector_size == 0);

  Dtype loss = 0;
  int64_t correct = 0;
  for (int i_bs = 0; i_bs < bs; i_bs++) {
    int64_t argmax_ = 0;
    Dtype max_val_ = 0;

    cout << "label data " << (offset+i_bs) % labels.size() << endl;

    const auto &nextLabel_ = *labels[(offset+i_bs) % labels.size()];
    for (int i_in = 0; i_in < n_in; i_in++) {
      auto y = nextLabel_[i_in];
//      auto y_ = input[{i_bs, i_in}];
      auto y_ = input[i_bs*n_in+i_in];
      if (y_ > max_val_) {
        max_val_ = y_;
        argmax_ = i_in;
      }
      loss += (y - y_)*(y - y_);
    }
    if (nextLabel_.argmax() == argmax_) {
      correct ++;
    }
  }
  loss /= 2;

  acc_ = Dtype(double(correct) / bs);
  output_[0] = loss;
  return output_;
}

void L2Norm::df(Tensor &input_diff, const Tensor &input) {
  for (int i_bs = 0; i_bs < bs; i_bs++) {
    for (int i_in = 0; i_in < n_in; i_in++) {
      const auto &label = *labels[(offset + i_bs)%labels.size()];
      auto y = label[i_in];
//      auto y_ = input[{i_bs,i_in}];
      auto y_ = input[i_bs*n_in+i_in];
//      input_diff[{i_bs, i_in}] = (y_ - y);
      input_diff[i_bs*n_in+i_in] = (y_ - y);
    }
  }
  if (use_mpi) {
    this->offset += bs * world_size;
  } else {
    this->offset += bs;
  }
  this->offset %= this->labels.size();
}

std::tuple<Dtype, Dtype> L2Norm::test(const Tensor &input) {
  int64_t label_vector_size = test_labels[0]->size();
  assert(input.size() % label_vector_size == 0);

  Dtype loss = 0;
  int correct = 0;
  for (int i_bs = 0; i_bs < bs; i_bs++) {
    int64_t argmax_ = 0;
    Dtype max_val_ = 0;
    const auto &label = *test_labels[(offset + i_bs) % test_labels.size()];
    for (int i_in = 0; i_in < n_in; i_in++) {
      auto y = label[i_in];
      auto y_ = input[i_bs*n_in, i_in];
      if (y_ > max_val_) {
        max_val_ = y_;
        argmax_ = i_in;
      }
      loss += (y - y_)*(y - y_);
    }
    if (label.argmax() == argmax_) {
      correct ++;
    }
  }

  loss /= 2;
//  offset += bs;
  offset %= test_labels.size();
  return std::make_tuple(loss, Dtype((double(correct)/bs)));
}
}
