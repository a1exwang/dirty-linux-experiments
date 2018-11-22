CMAKE_MINIMUM_REQUIRED(VERSION 3.8)

PROJECT(nccl-ompi-allreduce LANGUAGES CXX CUDA)
find_package(CUDA)
find_package(MPI)
include_directories(SYSTEM ${MPI_INCLUDE_PATH})

include_directories(mmpl-1.2.2/include)
link_directories(mmpl-1.2.2/lib)

include_directories(/usr/local/nccl_2.2.13-1+cuda9.0_x86_64/include/)
link_directories(/usr/local/nccl_2.2.13-1+cuda9.0_x86_64/lib/)

add_executable(nccl-ompi-allreduce allreduce.cu main.cpp)
target_compile_options(nccl-ompi-allreduce PRIVATE -std=c++11)
target_link_libraries(nccl-ompi-allreduce nccl cublas ${MPI_C_LIBRARIES} ${MPI_CXX_LIBRARIES})
set_target_properties(nccl-ompi-allreduce
        PROPERTIES CUDA_SEPARABLE_COMPILATION ON)

add_executable(mmpl_main mmpl_main.cu)
target_compile_options(mmpl_main PRIVATE -std=c++11)
target_include_directories(mmpl_main PRIVATE ${CUDA_INCLUDE_DIRECTORIES})
#target_link_libraries(mmpl_main nccl cublas mmpl ${MPI_C_LIBRARIES} ${MPI_CXX_LIBRARIES} boost_system cuda)
set_target_properties(mmpl_main PROPERTIES CUDA_SEPARABLE_COMPILATION ON)

cuda_add_executable(sgd sgd.cpp)
cuda_add_cublas_to_target(sgd)
target_compile_options(sgd PRIVATE -std=c++11)
target_include_directories(sgd PRIVATE ${CUDA_INCLUDE_DIRECTORIES})
target_link_libraries(sgd nccl ${MPI_C_LIBRARIES} ${MPI_CXX_LIBRARIES})
set_target_properties(sgd PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
