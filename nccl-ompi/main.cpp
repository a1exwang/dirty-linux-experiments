
#include <iostream>
#include "allreduce.h"
#include <mpi.h>
#include <chrono>
#include <iostream>

using namespace std;

int main(int argc, char **argv) {
  int size = atoi(argv[1]);

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

  NcclContext ctx(world_size, world_rank);
  ctx.ncclInit();
  cout << "init" << endl;
  auto buf = ctx.alloc(size);

  MPI_Barrier(MPI_COMM_WORLD);

  auto t0 = std::chrono::high_resolution_clock::now();
  ctx.allReduce(buf, buf, size);
  auto t1 = std::chrono::high_resolution_clock::now();
  cout << "allReduce time: " << (t1-t0).count() / 1e6 << "ms" << endl;
  ctx.sync();
  cout << "sync time: " << (chrono::high_resolution_clock::now() - t1).count() / 1e6 << "ms" << endl;

  MPI_Finalize();
}
