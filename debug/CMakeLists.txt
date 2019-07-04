add_executable(rng src/rng.cpp)
target_link_libraries(rng pthread)

add_compile_options(-Wall)
add_executable(unexpected_write src/unexpected_write.cpp)

add_executable(thread-sanitizer src/thread-sanitizer.cpp)
target_compile_options(thread-sanitizer PUBLIC -fsanitize=thread -fPIE)
target_link_options(thread-sanitizer PUBLIC -fsanitize=thread)


