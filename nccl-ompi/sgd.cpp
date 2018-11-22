#include <iostream>
#include "allreduce.h"
#include <mpi.h>
#include <chrono>
#include <iostream>
#include <cublas_v2.h>
#include <cuda.h>
#include <cuda_runtime.h>
#include <cassert>
#include <vector>
#include "check.h"


using namespace std;

struct Layer {
public:
  Layer(uint64_t parameter_count) :n(parameter_count) {
    assert(cudaMallocManaged(&diff, n, cudaMemAttachGlobal) == cudaSuccess);
    assert(cudaMallocManaged(&w, n, cudaMemAttachGlobal) == cudaSuccess);
  }
public:
  float *output() { return (float*) out; }
  float *weights() { return (float*) w; }
public:
  uint64_t n;
  void *diff;
  void *w;
  void *out;
};

void mpi_worker(int rank, int total_ranks, cublasHandle_t h) {
  // Create layers
  vector<Layer*> layers;
  int width = 1024;
  for (int i = 0; i < 10; ++i) {
    layers.push_back(new Layer(width*width));
  }

  int max_iters = 1000;
  for (int iter = 0; iter < max_iters; iter++) {
    float alpha = 1, beta = 0;
    for (int i = 1; i < layers.size(); ++i) {
      auto in_data = layers[i-1]->output();
      auto out_data = layers[i]->output();
      auto w = layers[i]->weights();
      cublasSgemm_v2(h, CUBLAS_OP_N, CUBLAS_OP_N,
          width, width, width,
          &alpha, w, width, in_data, width,
          &beta, out_data, width);

    }
  }
}

int main(int argc, char **argv) {
  // Initialize the MPI environment
  MPI_Init(NULL, NULL);

  // Get the number of processes
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  // Get the rank of the process
  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  // Get the name of the processor
  char processor_name[MPI_MAX_PROCESSOR_NAME];
  int name_len;
  MPI_Get_processor_name(processor_name, &name_len);

  // Print off a hello world message
  printf("Hello world from processor %s, rank %d out of %d processors\n",
         processor_name, world_rank, world_size);

  cublasHandle_t h;
  CUBLAS_CHECK(cublasCreate(&h));
  mpi_worker(world_rank, world_size, h);


  MPI_Barrier(MPI_COMM_WORLD);

  MPI_Finalize();
}
