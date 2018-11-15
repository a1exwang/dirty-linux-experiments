#pragma once
#include <cstring>

class NcclContextImpl;
struct NcclContext {
  NcclContext(int proc_size, int rank);

  void ncclInit();
  void allReduce(const void *sendbuf, void *recvbuf, size_t count);

  void *alloc(uint64_t size);
  void sync();

  int proc_size;
  int rank;
  int device_id;

  NcclContextImpl *impl;
};
