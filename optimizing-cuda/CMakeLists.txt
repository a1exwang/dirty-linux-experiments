cmake_minimum_required(VERSION 3.0)
project(optimizing-cuda C CXX CUDA)

find_package(CUDA)
include_directories(${CUDA_INCLUDE_DIRS})
link_libraries(${CUDA_LIBRARIES} ${CUDA_CUBLAS_LIBRARIES} glog)
add_executable(gemm gemm.cpp)
