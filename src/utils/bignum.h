#pragma once

#include "fatstring.h"
#include "type.h"
#include "vec.h"

// `.cap` and `.len` should be multiplied by 64 if referring to bits.
// `.ptr[0]` is the least significant element.
typedef u64Vec BigNum;

BigNum         bignum_new(u64 const cap);
BigNum         bignum_new_init(u64 const cap, u64 const n);
BigNum         bignum_clone(BigNum const n);
void           bignum_shrink(BigNum *const n);
void           bignum_resize(BigNum *const n, u64 const cap);
void           bignum_clean(BigNum *const n);
void           bignum_free(BigNum const n);
void           bignum_force_bit(BigNum *const n, u64 const pos, u8 const val);
void           bignum_set_bit(BigNum *const n, u64 const pos);
void           bignum_unset_bit(BigNum *const n, u64 const pos);
u64            bignum_is_set_bit(BigNum const n, u64 const pos);

BigNum         bignum_read(FILE *stream);
BigNum         bignum_read_hex(FILE *stream);

String         bignum_to_string(BigNum const n);
String         bignum_to_string_hex(BigNum const n, u32 const space);
void           bignum_print(FILE *stream, BigNum const n);
void           bignum_print_hex(FILE *stream, BigNum const n, u32 const space);
void           bignum_print_base(FILE *stream, BigNum const n, u64 const b);

u64            bignum_div_eq_u64(BigNum *const n, u64 const d);
u64Vec         bignum_to_base(BigNum const n, u64 const b);
