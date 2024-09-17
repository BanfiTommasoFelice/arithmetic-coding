# Arithmetic Coding

Implementation of arithmetic codings

## Compilation
Debug:
```bash
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=d
make
./arithmetic-coding <CMakeCache.txt # or any other file
```
Release:
```bash
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=r
make
./arithmetic-coding <CMakeCache.txt # or any other file
```
Analysis:
```bash
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=a
make
./arithmetic-coding <CMakeCache.txt # or any other file
```

## Test
```bash
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=r
make
./test 1000000 10 # or any other number
```

## Todo
- make `main.c` useful
- write directly to a file
- adaptive coding
- simd intrinsics
- multithread

## Extra
There are also other library functions, that probably will be useful for generating random arrays
mod P, P not necessarily prime

