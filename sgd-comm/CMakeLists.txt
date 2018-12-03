cmake_minimum_required(VERSION 3.8)
project(sgd-comm)

set(CMAKE_CXX_STANDARD 17)
find_package(MPI)
find_package(CUDA)
find_package(OpenCV)

include_directories(src)
add_executable(sgd_comm sgd_comm.cpp)
target_link_libraries(sgd_comm ${MPI_CXX_LIBRARIES})
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -rdynamic")

add_executable(asgd asgd.cpp
        src/net.cpp
        src/tensor.cpp
        src/functions/function.cpp
        src/functions/data.cpp
        src/functions/inner_product.cpp
        src/functions/loss.cpp
        src/utils/benchmark.cpp
        )
target_link_libraries(asgd ${MPI_CXX_LIBRARIES})

add_executable(mpi_win mpi_win.cpp)
target_link_libraries(mpi_win ${MPI_CXX_LIBRARIES} pthread curl opencv_core opencv_highgui glog)

add_executable(cache_test cache_test.cpp)
target_link_libraries(cache_test curl pthread opencv_core opencv_highgui)

add_executable(distributedhash distributedhash.c)
target_link_libraries(distributedhash ${MPI_C_LIBRARIES})

add_executable(test_hash test_hash.cpp)
target_link_libraries(test_hash ${MPI_C_LIBRARIES} ${MPI_CXX_LIBRARIES})
