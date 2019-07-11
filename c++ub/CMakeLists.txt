cmake_minimum_required(VERSION 3.8)

add_library(ub1 ub1.cpp)
add_library(signed_int_overflow signed_int_overflow.cpp)
add_executable(infinite-loop-without-side-effects infinite-loop-without-side-effects.cpp)

add_executable(one_definition_rule
        one_definition_rule/compilation_unit1.cpp
        one_definition_rule/compilation_unit2.cpp)
add_compile_options(-fsantize=address -fPIC)