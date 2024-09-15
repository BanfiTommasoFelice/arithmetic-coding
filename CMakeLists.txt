cmake_minimum_required(VERSION 3.9)
project(constant_time_constant_weight_encoding C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "-Wall -Wextra")
set(CMAKE_C_FLAGS_DEBUG   "-O0 -g3 -fsanitize=address,undefined")
set(CMAKE_C_FLAGS_RELEASE "-O3 -march=native -DNDEBUG")
# set(CMAKE_C_FLAGS_RELEASE "-O3 -g3 -march=native -DNDEBUG")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB SOURCE_LIB "src/utils/*.c")

# file(GLOB HEADER_LIB "src/utils/*.h") # useless?
# include_directories(include)          # useless?

add_executable(arithmetic-coding src/main.c ${SOURCE_LIB})
