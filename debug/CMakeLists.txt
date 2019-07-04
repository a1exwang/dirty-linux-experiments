add_executable(rng src/rng.cpp)
target_link_libraries(rng pthread)

add_executable(expected_write src/unexpected_write.cpp)
