#include <mmpl.hpp>
#include <cuda.h>
#include <mpi.h>
#include <iostream>
#include <chrono>

using namespace std;

int main(int argc, char **argv) {
    int p_rank, p_size;

    MMPL_Init(NULL, NULL, &p_rank, &p_size, false);
    MMPL_SetDevice(p_rank);

    size_t size = 3079000;
    void *buf;
    cudaError r = cudaMallocManaged(&buf, size * sizeof(float));
    if (r != cudaSuccess) {
        cerr << "cudaMallocManaged Failed: " << cudaGetErrorString(r) << endl;
        return 1;
    }

    MPI_Barrier(MPI_COMM_WORLD);

    for (int i = 0; ; i++) {
        auto t0 = std::chrono::high_resolution_clock::now();
        MMPL_Allreduce_gpu(buf, buf, size, 0, MMPL_FLOAT);
        auto t1 = std::chrono::high_resolution_clock::now();
        cout << "Allreduce rank=:" << p_rank << " i=" << i << " t=" << (t1-t0).count()/1e6 << "ms" << endl;
    }

//    MMPL_Finalize();
}