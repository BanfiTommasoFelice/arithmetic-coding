#include "modinv.h"

#include <assert.h>

static u32 u32_mod_inv_odd(u32 b);
static u64 u64_mod_inv_odd(u64 b);

static u32 u32_mod_inv_odd(u32 b) {
    assert(b & 1 && "b is even: 2^32 and b are not coprime");
    u32_pair tmp;
    u32      y = 0, y1 = 1, q, a;

    q   = UINT32_MAX / b;  // I should use 0x100000000, but so i use only u32
    tmp = (u32_pair){.first = y1, .second = y - q * y1};
    y   = tmp.first;
    y1  = tmp.second;
    tmp = (u32_pair){.first = b, .second = -q * b};
    a   = tmp.first;
    b   = tmp.second;

    while (b) {
        q   = a / b;
        tmp = (u32_pair){.first = y1, .second = y - q * y1};
        y   = tmp.first;
        y1  = tmp.second;
        tmp = (u32_pair){.first = b, .second = a - q * b};
        a   = tmp.first;
        b   = tmp.second;
    }
    assert(a == 1 && "gcd(b, 2^32) should be 1");
    return y;
}

// .first  = modular inverse of the odd
// .second = exp
u32_pair u32_mod_inv(u32 b) {
    assert(b && "modular inverse of 0 does not exist");
    u32 exp = __builtin_ctz(b);
    return (u32_pair){
        .first  = u32_mod_inv_odd(b >> exp),
        .second = exp,
    };
}

static u64 u64_mod_inv_odd(u64 b) {
    assert(b & 1 && "b is even: 2^32 and b are not coprime");
    u64_pair tmp;
    u64      y = 0, y1 = 1, q, a;

    q   = UINT64_MAX / b;  // I should use 0x100000000, but so i use only u64
    tmp = (u64_pair){.first = y1, .second = y - q * y1};
    y   = tmp.first;
    y1  = tmp.second;
    tmp = (u64_pair){.first = b, .second = -q * b};
    a   = tmp.first;
    b   = tmp.second;

    while (b) {
        q   = a / b;
        tmp = (u64_pair){.first = y1, .second = y - q * y1};
        y   = tmp.first;
        y1  = tmp.second;
        tmp = (u64_pair){.first = b, .second = a - q * b};
        a   = tmp.first;
        b   = tmp.second;
    }
    assert(a == 1 && "gcd(b, 2^64) should be 1");
    return y;
}

// .first  = modular inverse of the odd
// .second = exp
u64_pair u64_mod_inv(u64 b) {
    assert(b && "modular inverse of 0 does not exist");
    u64 exp = __builtin_ctzl(b);
    return (u64_pair){
        .first  = u64_mod_inv_odd(b >> exp),
        .second = exp,
    };
}
