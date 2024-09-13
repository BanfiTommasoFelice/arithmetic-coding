# Arithmetic Coding

Implementation of arithmetic codings

## Compilation
Debug:
```bash
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make
./arithmetic-coding <CMakeCache.txt # or any other file
```
Release:
```bash
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
./arithmetic-coding <CMakeCache.txt # or any other file
```

## Todo
- test cases
- write directly to a file
- adaptive coding
- simd intrinsics
- multithread

## Extra

There are also other library functions, that probably will be usefull for generating random arrays
mod P, P not necessarily prime

