#include <iostream>
#include <cuda.h>
#include <cublas_v2.h>
#include <nccl.h>
#include "allreduce.h"
#include <mpi.h>

struct NcclContextImpl {
  cudaStream_t stream;
  cublasHandle_t cublas_handle;
  ncclComm_t comm;
  ncclUniqueId comm_id;
};

using namespace std;

void NcclContext::ncclInit() {
  cudaSetDevice(device_id);
  cout << "setDevice: " << device_id << endl;

  int priority_high, priority_low;
  cudaDeviceGetStreamPriorityRange(&priority_low, &priority_high);
  // Create stream with highest available priorities
  cudaError e = cudaStreamCreateWithPriority(&this->impl->stream, cudaStreamNonBlocking, priority_high);
//  cudaError e = cudaStreamCreate(&this->impl->stream);
  if (e != cudaSuccess) {
    cerr << "cudaStreamCreateFailed: " << cudaGetErrorString(e) << endl;
    return;
  }
  cout << "streamCreate: " << impl->stream << endl;

  ncclGetUniqueId(&this->impl->comm_id);
  MPI_Bcast(&this->impl->comm_id, NCCL_UNIQUE_ID_BYTES, MPI_CHAR, 0, MPI_COMM_WORLD);
  cout << this->impl->comm << " " << proc_size << " " << rank;
  ncclCommInitRank(&this->impl->comm, proc_size, this->impl->comm_id, rank);

  //int priority_high, priority_low;
  //cudaDeviceGetStreamPriorityRange(&priority_low, &priority_high);
  // Create stream with highest available priorities
  //cudaStreamCreateWithPriority(&this->impl->stream, cudaStreamNonBlocking, priority_high);
}

void NcclContext::allReduce(const void *sendbuf, void *recvbuf, size_t count) {
  ncclResult_t r = ncclAllReduce(sendbuf, recvbuf, count, ncclFloat, ncclSum, this->impl->comm, this->impl->stream);
  if (r != ncclSuccess) {
    std::cerr << "ncclError: " << ncclGetErrorString(r) << std::endl;
  }
}

NcclContext::NcclContext(int proc_size, int rank) :impl(new NcclContextImpl), proc_size(proc_size), rank(rank), device_id(rank) {}

void *NcclContext::alloc(uint64_t size) {
  void *ret;
  cudaError r = cudaMallocManaged(&ret, size);
  if (r != cudaSuccess) {
    cerr << "cudaMallocManaged Failed: " << cudaGetErrorString(r) << endl;
    return 0;
  }
  return ret;
}

void NcclContext::sync() {
  cudaStreamSynchronize(this->impl->stream);
}
