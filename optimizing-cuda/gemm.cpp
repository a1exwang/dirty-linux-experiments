#include <iostream>
#include <cuda_runtime.h>
#include "cublas_v2.h"
#include <glog/logging.h>
#include <chrono>

using namespace std;

#define CUDA_CHECK(condition) \
  /* Code block avoids redefinition of cudaError_t error */ \
  do { \
    cudaError_t error = condition; \
    CHECK_EQ(error, cudaSuccess) << " " << cudaGetErrorString(error); \
  } while (0)

#define CUBLAS_CHECK(condition) \
  do { \
    cublasStatus_t status = condition; \
    CHECK_EQ(status, CUBLAS_STATUS_SUCCESS); \
  } while (0)


int main (){

  cublasHandle_t handle;
  float *A, *B, *C, alpha = 1, beta = 0;

  int test_times = 128;
  uint64_t max_size = 1024UL*1024UL*1024UL;
  // M, N, K
  uint64_t test_range[] = {10, 13, 10, 13, 10, 13};

  uint64_t m0 = 1UL<<test_range[0];
  uint64_t m1 = 1UL<<test_range[1];
  uint64_t n0 = 1UL<<test_range[2];
  uint64_t n1 = 1UL<<test_range[3];
  uint64_t k0 = 1UL<<test_range[4];
  uint64_t k1 = 1UL<<test_range[5];

  CUBLAS_CHECK(cublasCreate(&handle));
  CUDA_CHECK(cudaMalloc((void**)&A, m1*k1*sizeof(float)));
  CUDA_CHECK(cudaMalloc((void**)&B, k1*n1*sizeof(float)));
  CUDA_CHECK(cudaMalloc((void**)&C, m1*n1*sizeof(float)));
  for (uint64_t m = test_range[0]; m < test_range[1]; m++) {
      for (uint64_t n = test_range[2]; n < test_range[3]; n++) {
        for (uint64_t k = test_range[4]; k < test_range[5]; k++) {
          uint64_t M = (1UL<<m), N = (1UL<<n), K = (1UL<<k);
          if ((M*N + N*K + M*K)*sizeof(float) > max_size) {
            continue;
          }
          auto t0 = chrono::high_resolution_clock::now();
          for (int i = 0; i < test_times; i++) {
            // A: MxK, B: KxN, C: MxN
            CUBLAS_CHECK(cublasSgemm(handle, CUBLAS_OP_N, CUBLAS_OP_N, M, N, K, &alpha, A, M, B, K, &beta, C, M));
          }
          cudaStreamSynchronize(cudaStreamDefault);
          auto t1 = chrono::high_resolution_clock::now();
          auto gemm_time = chrono::duration<double>(t1-t0).count() / test_times;
          cout << M << " " << N << " " << K << " " << gemm_time << " " << (1.0*M*N*(2*K+2)/gemm_time/1e12) << "TFLOPs " << ((2*M*N+N*K+K*M)*sizeof(float))/gemm_time/1e9 << "G " << endl;
        }
    }
  }
  cout << "test done"<<endl;
  CUDA_CHECK(cudaFree(A));
  CUDA_CHECK(cudaFree(B));
  CUDA_CHECK(cudaFree(C));

  cublasDestroy(handle);
  return EXIT_SUCCESS;
}