cmake_minimum_required(VERSION 3.0)
project(OptimizingCpp C CXX ASM)
set(CMAKE_CXX_STANDARD 17)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -x assembler-with-cpp -no-pie")
set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -no-pie")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -no-pie -O2")

add_executable(data_dependency add_no_data_dependency/dd.S add_no_data_dependency/print.cpp)
set_source_files_properties(add_no_data_dependency/dd.S PROPERTIES COMPILE_FLAGS "-x assembler-with-cpp -no-pie")
target_link_options(data_dependency PUBLIC -no-pie)

add_executable(avx avx/avx.cpp avx/avx.S)
set_source_files_properties(avx/avx.S PROPERTIES COMPILE_FLAGS "-x assembler-with-cpp -no-pie")
target_compile_options(avx PUBLIC -mavx2 -no-pie)

add_executable(branches branches/branches.cpp branches/branches.S)
set_source_files_properties(branches/branches.S PROPERTIES COMPILE_FLAGS "-x assembler-with-cpp -no-pie")
target_compile_options(branches PUBLIC -mavx2 -no-pie)

add_executable(memory_move backend/memory_move.cpp backend/memory_move.S)

