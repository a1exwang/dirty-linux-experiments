cmake_minimum_required(VERSION 3.0)
project(weak-symbol)

add_executable(add main.c add.c)
add_executable(add_optimized main.c add.c add_optimized.c)
