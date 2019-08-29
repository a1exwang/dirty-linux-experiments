add_executable(c++-type-deduce c++-type-deduce.cpp)
add_executable(sizeof sizeof.cpp)
add_executable(eat-cpu eat-cpu.cpp)
target_link_libraries(eat-cpu PRIVATE pthread)