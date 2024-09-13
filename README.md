# Arithmetic Coding

Implementation of arithmetic codings

## Compilation
Debug:
```bash
cd build
cmake -DCMAKE_BUILD_TYPE=Debug .. && make
./arithmetic-coding <../in/divina-commedia.txt
```
Release:
```bash
cd build
cmake -DCMAKE_BUILD_TYPE=Release .. && make
./arithmetic-coding <../in/divina-commedia.txt
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

