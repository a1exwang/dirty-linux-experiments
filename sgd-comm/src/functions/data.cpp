#include "data.h"

#include <sys/stat.h>
#include <sys/mman.h>
#include <utils/endian.h>

#include <unistd.h>
#include <fcntl.h>

#include <tuple>
#include <iostream>
#include <cassert>
#include <mpi.h>


using namespace std;


namespace asgd {


std::tuple<vector<Tensor*>, int64_t, int64_t> readMnistImages(const std::string &filename) {
  vector<Tensor*> ret;
  int fd = open(filename.c_str(), O_RDONLY);
  assert(fd > 0);
  struct stat s;
  int err = stat(filename.c_str(), &s);
  assert(err == 0);
  auto fsize = s.st_size;

  char *ptr = (char*)mmap(NULL, fsize, PROT_READ, MAP_PRIVATE | MAP_POPULATE, fd, 0);
  if (ptr == MAP_FAILED) {
    perror("mmap");
    abort();
  }

  assert((*(int32_t*)(ptr + 0)) == reverse32(2051));
  uint32_t imgs = reverse32(*(uint32_t*)(ptr + 4));
  int64_t rows = reverse32(*(uint32_t*)(ptr + 8));
  int64_t cols = reverse32(*(uint32_t*)(ptr + 12));
  for (int i = 0; i < imgs; i++) {
    const char *base = (ptr + 0x10 + i * rows * cols);
    auto t = new Tensor({rows*cols});
    for (int row = 0; row < rows; row++) {
      for (int col = 0; col < cols; col++) {
        (*t)[row*cols + col] = Dtype((uint8_t)base[row*cols+col]) / Dtype(256);
      }
    }
    ret.push_back(t);
  }

  close(fd);
  munmap(ptr, fsize);
  return std::make_tuple(ret, rows, cols);
}

void Data::setup(int64_t bs) {
  tie(test_data, rows, cols) = readMnistImages("t10k-images-idx3-ubyte");
  tie(train_data, rows, cols) = readMnistImages("train-images-idx3-ubyte");
  current_batch = new Tensor({bs, rows*cols});
//  current_test_batch = new Tensor({bs, rows*cols});
  if (use_mpi) {
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    offset = world_rank * bs;
  } else {
    offset = 0;
  }
}

const Tensor &Data::forward(const Tensor &input) {
  for (int64_t i_bs = 0; i_bs < bs; i_bs++) {
    const auto &img = *this->train_data[(offset+i_bs)%train_data.size()];
    cout << "forward data " << (offset+i_bs)%train_data.size() << endl;
    for (int j = 0; j < img.size(); j++) {
//      output_[{i_bs, j}] = img[j];
      output_[i_bs*img.size() + j] = img[j];
    }
  }

  if (use_mpi) {
    offset += world_size * bs;
  } else {
    offset += bs;
  }
  offset %= train_data.size();
  return output_;
}

const Tensor &Data::test(int64_t test_size) {
  for (int64_t i_bs = 0; i_bs < bs; i_bs++) {
    const auto &img = *this->test_data[test_offset];
    for (int j = 0; j < img.size(); j++) {
      (*current_batch)[{i_bs, j}] = img[j];
    }
  }

  test_offset += bs;
  test_offset %= test_data.size();
  return *current_batch;
}

}
