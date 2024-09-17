cmake_minimum_required(VERSION 3.9)
project(constant_time_constant_weight_encoding C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "-Wall -Wextra")
set(CMAKE_C_FLAGS_D "-O0 -g3 -fsanitize=address,undefined")
set(CMAKE_C_FLAGS_R "-O3 -march=native -DNDEBUG")
set(CMAKE_C_FLAGS_A "-O3 -march=native -DNDEBUG -g3 -mno-avx")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB SOURCE_LIB "src/utils/*.c")

file(GLOB HEADER_LIB "src/utils/*.h") # useless?

# add_executable(arithmetic-coding src/main.c ${SOURCE_LIB})
add_executable(test src/test.c ${SOURCE_LIB})
