cmake_minimum_required(VERSION 3.0)
project(dso)


add_executable(common-variable common_variable.c common_variable2.c)
target_compile_options(common-variable PUBLIC -fcommon)

add_library(hidden SHARED hidden.c )
target_compile_options(hidden PUBLIC -fvisibility=hidden)
