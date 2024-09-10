#pragma once

#include "fatstring.h"
#include "type.h"
#include "vec.h"

typedef struct _BigNum {
    u64  cap, len;  // cap and len should be multiplied by 64
    u64 *ptr;
    // bits are used as follow:
    // - the least significant is in the rightmost bit of the first element
    // - the most significant is in the leftmost bit of the last element
} BigNum;

BigNum bignum_new(u64 u64s);
BigNum bignum_clone(BigNum n);
void   bignum_shrink(BigNum *n);
void   bignum_resize(BigNum *n, u64 cap);
void   bignum_clean(BigNum *c);
void   bignum_free(BigNum n);

BigNum bignum_read(FILE *stream);
BigNum bignum_read_hex(FILE *stream);

String bignum_to_string(BigNum n);
String bignum_to_string_hex(BigNum n);
String bignum_to_string_hex_dbg(BigNum n);
void   bignum_print(FILE *stream, BigNum x);
void   bignum_print_hex(FILE *stream, BigNum x);
void   bignum_print_hex_dbg(FILE *stream, BigNum x);

u64    bignum_div_eq_u64(BigNum *c, u64 d);
u64Vec bignum_to_base(BigNum n, u64 b);
void   bignum_print_base(FILE *stream, BigNum x, u64 b);
