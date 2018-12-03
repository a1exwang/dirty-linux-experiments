cmake_minimum_required(VERSION 3.11)
project(libclang-find-usage)

include_directories(${CMAKE_SOURCE_DIR}/include)
add_library(find-usage SHARED
        src/plugin.cpp
        src/visitors/find_usages.cpp
        )
target_link_libraries(find-usage clangAST clangBasic)
