#include "data.h"

#include <iostream>
#include <cassert>
#include <sys/stat.h>
#include <sys/mman.h>
#include <utils/endian.h>

#include <unistd.h>
#include <fcntl.h>


using namespace std;


namespace asgd {


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
  for (int i = 0; i < current_batch->size(); i++) {
    output_[i] = (*current_batch)[i];
  }
  return *current_batch;
}


void Data::setup() {
  auto filename = "t10k-images-idx3-ubyte";
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
      (*t)[j] = Dtype((uint8_t)base[j]) / Dtype(256);
    }
    this->data.push_back(t);
  }

  close(fd);
  munmap(ptr, fsize);
  current_batch = new Tensor({bs, cols*rows});
}

}
