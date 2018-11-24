#include "data.h"

#include <sys/stat.h>
#include <sys/mman.h>
#include <utils/endian.h>

#include <unistd.h>
#include <fcntl.h>

#include <tuple>
#include <iostream>
#include <cassert>


using namespace std;


namespace asgd {


const Tensor &Data::forward(const Tensor &input) {
  for (int64_t i_bs = 0; i_bs < bs; i_bs++) {
    const auto &img = *this->train_data[offset];
    for (int j = 0; j < img.size(); j++) {
      (*current_batch)[{i_bs, j}] = img[j];
    }
//    std::cout << "Data " << offset << ", ";
    offset++;
    if (offset >= this->train_data.size()) {
      offset = 0;
    }
  }
  for (int i = 0; i < current_batch->size(); i++) {
    output_[i] = (*current_batch)[i];
  }
  return *current_batch;
}

std::tuple<vector<Tensor*>, int64_t, int64_t> readMnistImages(const std::string &filename) {
  vector<Tensor*> ret;
  int fd = open(filename.c_str(), O_RDONLY);
  assert(fd > 0);
  struct stat s;
  assert(stat(filename.c_str(), &s) >= 0);
  auto fsize = s.st_size;

  char *ptr = (char*)mmap(NULL, fsize, PROT_READ, MAP_PRIVATE | MAP_POPULATE, fd, 0);

  assert((*(int32_t*)(ptr + 0)) == reverse32(2051));
  uint32_t imgs = reverse32(*(uint32_t*)(ptr + 4));
  int64_t rows = reverse32(*(uint32_t*)(ptr + 8));
  int64_t cols = reverse32(*(uint32_t*)(ptr + 12));
  for (int i = 0; i < imgs; i++) {
    const char *base = (ptr + 0x10 + i * rows * cols);
    auto t = new Tensor({rows, cols});
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

void Data::setup() {
  tie(test_data, rows, cols) = readMnistImages("t10k-images-idx3-ubyte");
  tie(train_data, rows, cols) = readMnistImages("train-images-idx3-ubyte");
  current_batch = new Tensor({bs, rows*cols});
}

const Tensor &Data::test(int64_t test_size) {
  for (int64_t i_bs = 0; i_bs < test_size; i_bs++) {
    const auto &img = *this->test_data[test_offset];
    for (int j = 0; j < img.size(); j++) {
      (*current_batch)[{i_bs, j}] = img[j];
    }
//    std::cout << "Test " << test_offset << ", ";
    test_offset++;
    if (test_offset >= this->test_data.size()) {
      test_offset = 0;
    }
  }
  return *current_batch;
}

}
