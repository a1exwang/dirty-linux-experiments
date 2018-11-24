cmake_minimum_required(VERSION 3.8)
project(sgd-comm)

set(CMAKE_CXX_STANDARD 17)
find_package(MPI)
find_package(CUDA)

include_directories(src)
add_executable(sgd_comm sgd_comm.cpp)
target_link_libraries(sgd_comm ${MPI_CXX_LIBRARIES})

add_executable(asgd asgd.cpp
        functions.cpp
        src/net.cpp
        src/tensor.cpp
        src/functions/function.cpp
        src/functions/data.cpp
        src/functions/inner_product.cpp
        src/functions/loss.cpp
        )
target_link_libraries(asgd ${MPI_CXX_LIBRARIES})