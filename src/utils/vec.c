#include "vec.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#define _IMPLEMENT_VEC(_type_)                                            \
                                                                          \
    _type_##Vec _type_##vec_new(u64 const cap) {                          \
        assert(cap && "cap should be != 0");                              \
        _type_##Vec const v = {                                           \
            .cap = cap,                                                   \
            .len = 0,                                                     \
            .ptr = malloc(v.cap * sizeof(*v.ptr)),                        \
        };                                                                \
        assert(v.ptr && "malloc failed");                                 \
        return v;                                                         \
    }                                                                     \
                                                                          \
    _type_##Vec _type_##vec_new_init(u64 const cap, _type_ const digit) { \
        _type_##Vec v = _type_##vec_new(cap);                             \
        for (u64 i = 0; i < cap; i++) v.ptr[i] = digit;                   \
        return v;                                                         \
    }                                                                     \
                                                                          \
    _type_##Vec _type_##vec_clone(_type_##Vec const v) {                  \
        _type_##Vec w = {                                                 \
            .cap = v.cap,                                                 \
            .len = v.len,                                                 \
            .ptr = malloc(v.cap * sizeof(*v.ptr)),                        \
        };                                                                \
        assert(w.ptr && "malloc failed");                                 \
        memcpy(w.ptr, v.ptr, w.cap * sizeof(*v.ptr));                     \
        return w;                                                         \
    }                                                                     \
                                                                          \
    void _type_##vec_shrink(_type_##Vec *const v) {                       \
        if (v->len < v->cap) {                                            \
            v->ptr = realloc(v->ptr, v->len * sizeof(*v->ptr));           \
            assert(v->ptr && "realloc failed");                           \
            v->cap = v->len;                                              \
        } else assert(v->len == v->cap && "len > cap");                   \
    }                                                                     \
                                                                          \
    void _type_##vec_resize(_type_##Vec *const v, u64 const cap) {        \
        if (v->cap == cap) return;                                        \
        v->len = min(v->len, cap);                                        \
        v->cap = cap;                                                     \
        v->ptr = realloc(v->ptr, cap * sizeof(*v->ptr));                  \
        assert(v->ptr && "realloc failed");                               \
    }                                                                     \
                                                                          \
    void _type_##vec_double(_type_##Vec *const v) {                       \
        _type_##vec_resize(v, v->cap * 2);                                \
    }                                                                     \
                                                                          \
    void _type_##vec_push(_type_##Vec *const v, _type_ const digit) {     \
        if (v->len == v->cap) _type_##vec_double(v);                      \
        v->ptr[v->len] = digit;                                           \
        v->len++;                                                         \
    }                                                                     \
                                                                          \
    _type_ _type_##vec_pop(_type_##Vec *const v) {                        \
        return v->ptr[--v->len];                                          \
    }                                                                     \
                                                                          \
    void _type_##vec_free(_type_##Vec const v) {                          \
        free(v.ptr);                                                      \
    }                                                                     \
                                                                          \
    void _type_##vec_print(FILE *const stream, _type_##Vec const v,       \
                           char const *const format_specifier) {          \
        fprintf(stream, "[");                                             \
        if (v.len) {                                                      \
            for (u32 i = 0; i < v.len - 1; i++) {                         \
                fprintf(stream, format_specifier, v.ptr[i]);              \
                fprintf(stream, ", ");                                    \
            }                                                             \
            fprintf(stream, format_specifier, v.ptr[v.len - 1]);          \
        }                                                                 \
        fprintf(stream, "]");                                             \
    }                                                                     \
                                                                          \
    void _type_##vec_print_rev(FILE *const stream, _type_##Vec const v,   \
                               char const *const format_specifier) {      \
        fprintf(stream, "[");                                             \
        if (v.len) {                                                      \
            for (u32 i = v.len - 1; i > 0; i--) {                         \
                fprintf(stream, format_specifier, v.ptr[i]);              \
                fprintf(stream, ", ");                                    \
            }                                                             \
            fprintf(stream, format_specifier, v.ptr[0]);                  \
        }                                                                 \
        fprintf(stream, "]");                                             \
    }

_IMPLEMENT_VEC(u64)
_IMPLEMENT_VEC(u32)
_IMPLEMENT_VEC(u16)
_IMPLEMENT_VEC(u8)

// call srand() before
void u8vec_fill_rnd_distr(u8Vec *const v, u64 const n) {
    assert(n <= v->cap || "n should be less than or equal to cap");
    assert(RAND_MAX == 0x7fffffff);
    v->len = n;

    u32 r[257];
    u32 tot = 0, cum = 0;
    for (u32 i = 0; i < 256; i++) {
        u32 x = rand();
        r[i]  = ((x & 0xff0) >> 4) << (x & 0xf);  // avoid OF: 64 - 31 (RAND_MAX) - 8 (256) = 25 bit
        // 0b00000000000000000000BBBBBBBBSSSS                16 (bit Shift) + 8 (Base bits) = 24 bit
        tot += r[i];
    }

    r[0] = 0;
    for (u32 i = 1; i < 256; i++) {
        cum += r[i];
        r[i] = (u64)cum * RAND_MAX / tot;
    }
    for (u64 i = 0; i < n; i++) {
        u32 const x      = rand();
        u32       lb_idx = 0, ub_idx = 256;
        for (u32 i = 0; i < 8; i++) {
            u32 const mid_idx = (lb_idx + ub_idx) >> 1;
            u32 const mid_int = r[mid_idx];
            if (mid_int > x) ub_idx = mid_idx;
            else lb_idx = mid_idx;
        }
        assert(ub_idx - lb_idx == 1 && "binary search did not finish");
        v->ptr[i] = lb_idx;
    }
}
