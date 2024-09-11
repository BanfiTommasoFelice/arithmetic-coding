#pragma once

#include "fatstring.h"
#include "type.h"
#include "vec.h"

// `.cap` and `.len` should be multiplied by 64 if referring to bits.
// `.ptr[0]` is the least significant element.
typedef u64Vec BigNum;

BigNum         bignum_new(u64 cap);
BigNum         bignum_new_init(u64 cap, u64 n);
BigNum         bignum_clone(BigNum n);
void           bignum_shrink(BigNum *n);
void           bignum_resize(BigNum *n, u64 cap);
void           bignum_clean(BigNum *c);
void           bignum_free(BigNum n);
void           bignum_force_bit(BigNum *n, u64 pos, u8 val);
void           bignum_set_bit(BigNum *n, u64 pos);
void           bignum_unset_bit(BigNum *n, u64 pos);
u64            bignum_is_set_bit(BigNum n, u64 pos);

BigNum         bignum_read(FILE *stream);
BigNum         bignum_read_hex(FILE *stream);

String         bignum_to_string(BigNum n);
String         bignum_to_string_hex(BigNum n, u32 space);
void           bignum_print(FILE *stream, BigNum x);
void           bignum_print_hex(FILE *stream, BigNum x, u32 space);
void           bignum_print_base(FILE *stream, BigNum x, u64 b);

u64            bignum_div_eq_u64(BigNum *c, u64 d);
u64Vec         bignum_to_base(BigNum n, u64 b);

