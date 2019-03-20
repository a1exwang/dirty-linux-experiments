cmake_minimum_required(VERSION 3.0)
project(OptimizingCpp C ASM)


add_executable(data_dependency dd.S print.c)
set_source_files_properties(dd.S PROPERTIES COMPILE_FLAGS "-x assembler-with-cpp -no-pie")
target_link_options(data_dependency PUBLIC -no-pie)
