cmake_minimum_required(VERSION 3.9)
project(constant_time_constant_weight_encoding C)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "-Wall -march=native -O3 ")

file(GLOB SOURCE_LIB "src/utils/*.c")

set(GLOB HEADER_LIB "src/utils/*.h")

set(CMAKE_C_FLAGS_DEBUG   "-Wall -Wextra -O0 -g3 -fsanitize=address,undefined")
set(CMAKE_C_FLAGS_RELEASE "-Wall -Wextra -O3 -march=native -DNDEBUG")

include_directories(include)

add_executable(arithmetic-coding src/main.c ${SOURCE_LIB})
