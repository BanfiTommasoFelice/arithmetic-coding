#pragma once

#include <stdint.h>

typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float    f32;
typedef double   f64;

typedef struct _u32_pair {
    u32 first, second;
} u32_pair;

typedef struct _u64_pair {
    u64 first, second;
} u64_pair;
