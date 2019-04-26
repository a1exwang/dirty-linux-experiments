cmake_minimum_required(VERSION 3.0)
project(OptimizingCpp C CXX ASM)
set(CMAKE_CXX_STANDARD 17)

add_executable(data_dependency dd.S print.cpp)
set_source_files_properties(dd.S PROPERTIES COMPILE_FLAGS "-x assembler-with-cpp -no-pie")
target_link_options(data_dependency PUBLIC -no-pie)
