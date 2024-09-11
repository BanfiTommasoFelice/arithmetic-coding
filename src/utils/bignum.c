#include "bignum.h"

#include <assert.h>
#include <stdio.h>

#include "vec.h"

BigNum bignum_new(u64 cap) {
    return u64vec_new_init(cap, 0);
}

BigNum bignum_clone(BigNum n) {
    return u64vec_clone(n);
}

void bignum_shrink(BigNum *n) {
    return u64vec_shrink(n);
}

void bignum_resize(BigNum *n, u64 cap) {
    return u64vec_resize(n, cap);
}

void bignum_clean(BigNum *c) {
    for (u64 i = c->len - 1;; i--) {
        if (c->ptr[i] != 0) {
            c->len = i + 1;
            break;
        } else if (i == 0) {
            c->len = 0;
            break;
        }
    }
}

void bignum_free(BigNum n) {
    return u64vec_free(n);
}

void bignum_set_bit(BigNum *n, u64 pos, u64 val) {
    assert(pos < n->cap << 6);
    if (pos >= n->len << 6) n->len = (pos + 0x3f) >> 6;
    if (val) n->ptr[pos << 6] |= 1ull << (pos & 0x3f);
    else n->ptr[pos << 6] &= ~(1ull << (pos & 0x3f));
}

u64 bignum_is_set_bit(BigNum n, u64 pos) {
    assert(pos < n.len << 6);
    return !!(n.ptr[pos >> 6] & 1ull << (pos & 0x3f));
}

BigNum bignum_read(FILE *stream) {
    String s = string_read(stream);
    for (u64 i = 0; i < s.len - 1; i++) assert(s.ptr[i] >= '0' && s.ptr[i] <= '9');
    BigNum n         = bignum_new((s.len * 4 + 63) / 64);  // 4 > log2(10)
    u64    left_most = 0, pos = 0;
    while (left_most <= s.len - 2) {
        bignum_set_bit(&n, pos++, (s.ptr[s.len - 2] - '0') % 2);
        s.ptr[s.len - 2] = (s.ptr[s.len - 2] - '0') / 2 + '0';
        for (u64 i = s.len - 3; i >= left_most && i != UINT64_MAX; i--) {
            s.ptr[i + 1] += 5 * ((s.ptr[i] - '0') % 2);
            s.ptr[i]      = (s.ptr[i] - '0') / 2 + '0';
        }
        left_most += (s.ptr[left_most] == '0');
    }
    string_free(s);
    n.len = (pos + 63) / 64;
    bignum_shrink(&n);
    return n;
}

BigNum bignum_read_hex(FILE *stream) {
    String s = string_read(stream);
    for (u64 i = 0; i < s.len - 1; i++)
        assert((s.ptr[i] >= '0' && s.ptr[i] <= '9') || (s.ptr[i] >= 'a' && s.ptr[i] <= 'f'));
    BigNum n = bignum_new((s.len + 15) / 16);
    n.len    = n.cap;
    for (u64 i = 0; i < n.len - 1; i++) {
        sscanf(s.ptr + s.len - (i + 1) * 16, "%lx", &n.ptr[i]);
        s.ptr[s.len - (i + 1) * 16] = '\0';
    }
    sscanf(s.ptr, "%lx", &n.ptr[n.len - 1]);
    string_free(s);
    return n;
}

String bignum_to_string(BigNum n) {
    u64 b    = 10;
    n        = bignum_clone(n);
    u64 cap  = (n.len << 6) / (64 - __builtin_clzl(b - 1));  // log2(n) / log2(base) == log_base(n)
    String s = string_new(cap);
    while (n.len) {
        u64 carry = bignum_div_eq_u64(&n, b);
        string_push(&s, carry + '0');
    }
    bignum_free(n);
    if (s.len == 0) string_push(&s, '0');
    string_push(&s, '\0');
    string_shrink(&s);
    string_rev(&s);
    return s;
}

String bignum_to_string_hex(BigNum n, u32 space) {
    String s;
    if (!n.len) {
        s = string_new(2);
        sprintf(s.ptr, "0");
    } else {
        s = string_new(n.len * (space ? 17 : 16) + 2);
        for (u32 i = n.len - 1; i != UINT32_MAX; i--)
            sprintf(s.ptr + ((n.len - 1 - i) * (space ? 17 : 16)), "%016lx ", n.ptr[i]);
    }
    s.len        = s.cap - 1;
    return s;
}

void bignum_print(FILE *stream, BigNum x) {
    String s = bignum_to_string(x);
    fprintf(stream, "%s", s.ptr);
    string_free(s);
}

void bignum_print_hex(FILE *stream, BigNum x, u32 dbg) {
    String s = bignum_to_string_hex(x, dbg);
    fprintf(stream, "%s", s.ptr);
    string_free(s);
}

void bignum_print_base(FILE *stream, BigNum x, u64 b) {
    u64Vec v = bignum_to_base(x, b);
    u64vec_print_rev(stream, v, "%lu");
    u64vec_free(v);
}

u64 bignum_div_eq_u64(BigNum *c, u64 d) {
    if (!c->len || d == 1) return 0;
    u64 quo   = UINT64_MAX / d + (UINT64_MAX % d == d - 1);
    u64 car   = 0 - quo * d;
    u64 carry = 0;
    for (u32 i = c->len - 1; i != UINT32_MAX; i--) {
        u64 n          = c->ptr[i] / d;
        u64 new_carry  = c->ptr[i] - n * d;
        n             += quo * carry;
        new_carry     += car * carry;
        u64 tmp        = new_carry / d;
        c->ptr[i]      = n + tmp;
        carry          = new_carry - tmp * d;
    }
    c->len -= c->ptr[c->len - 1] == 0;
    assert(!c->len || c->ptr[c->len - 1] != 0);
    return carry;
}

u64Vec bignum_to_base(BigNum n, u64 b) {
    n        = bignum_clone(n);
    u64 cap  = (n.len << 6) / (64 - __builtin_clzl(b - 1));  // log2(n) / log2(base) == log_base(n)
    u64Vec v = u64vec_new(cap);
    while (n.len) {
        u64 carry = bignum_div_eq_u64(&n, b);
        u64vec_push(&v, carry);
    }
    bignum_free(n);
    return v;
}

