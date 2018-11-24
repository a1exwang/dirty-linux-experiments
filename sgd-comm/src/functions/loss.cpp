#include <functions/loss.h>
#include <utils/endian.h>

#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;
namespace asgd {

void Loss::setup() {
  auto filename = "t10k-labels-idx1-ubyte";
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

}
