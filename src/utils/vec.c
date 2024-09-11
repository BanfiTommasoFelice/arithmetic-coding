#include "vec.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "stdio.h"

#define _IMPLEMENT_VEC(_type_)                                                        \
                                                                                      \
    _type_##Vec _type_##vec_new(u64 cap) {                                            \
        assert(cap);                                                                  \
        _type_##Vec v = {                                                             \
            .cap = cap,                                                               \
            .len = 0,                                                                 \
            .ptr = malloc(v.cap * sizeof(*v.ptr)),                                    \
        };                                                                            \
        assert(v.ptr);                                                                \
        return v;                                                                     \
    }                                                                                 \
                                                                                      \
    _type_##Vec _type_##vec_new_init(u64 cap, _type_ digit) {                         \
        _type_##Vec v = _type_##vec_new(cap);                                         \
        for (u64 i = 0; i < cap; i++) v.ptr[i] = digit;                               \
        return v;                                                                     \
    }                                                                                 \
                                                                                      \
    _type_##Vec _type_##vec_clone(_type_##Vec v) {                                    \
        _type_##Vec w = {                                                             \
            .cap = v.cap,                                                             \
            .len = v.len,                                                             \
            .ptr = malloc(v.cap * sizeof(*v.ptr)),                                    \
        };                                                                            \
        assert(w.ptr);                                                                \
        memcpy(w.ptr, v.ptr, w.cap * sizeof(*v.ptr));                                 \
        return w;                                                                     \
    }                                                                                 \
                                                                                      \
    void _type_##vec_shrink(_type_##Vec *v) {                                         \
        if (v->len < v->cap) {                                                        \
            v->ptr = realloc(v->ptr, v->len * sizeof(*v->ptr));                       \
            assert(v->ptr);                                                           \
            v->cap = v->len;                                                          \
        } else assert(v->len == v->cap);                                              \
    }                                                                                 \
                                                                                      \
    void _type_##vec_resize(_type_##Vec *v, u64 cap) {                                \
        if (v->cap == cap) return;                                                    \
        v->len = v->len < v->cap ? v->len : v->cap;                                   \
        v->cap = cap;                                                                 \
        v->ptr = realloc(v->ptr, cap * sizeof(*v->ptr));                              \
        assert(v->ptr);                                                               \
    }                                                                                 \
                                                                                      \
    void _type_##vec_push(_type_##Vec *v, _type_ digit) {                             \
        if (v->len == v->cap) _type_##vec_resize(v, v->cap * 2);                      \
        v->ptr[v->len] = digit;                                                       \
        v->len++;                                                                     \
    }                                                                                 \
                                                                                      \
    void _type_##vec_free(_type_##Vec v) {                                            \
        free(v.ptr);                                                                  \
    }                                                                                 \
                                                                                      \
    void _type_##vec_print(FILE *stream, _type_##Vec v, char *format_specifier) {     \
        fprintf(stream, "[");                                                         \
        if (v.len) {                                                                  \
            for (u32 i = 0; i < v.len - 1; i++) {                                     \
                fprintf(stream, format_specifier, v.ptr[i]);                          \
                fprintf(stream, ", ");                                                \
            }                                                                         \
            fprintf(stream, format_specifier, v.ptr[v.len - 1]);                      \
        }                                                                             \
        fprintf(stream, "]");                                                         \
    }                                                                                 \
                                                                                      \
    void _type_##vec_print_rev(FILE *stream, _type_##Vec v, char *format_specifier) { \
        fprintf(stream, "[");                                                         \
        if (v.len) {                                                                  \
            for (u32 i = v.len - 1; i > 0; i--) {                                     \
                fprintf(stream, format_specifier, v.ptr[i]);                          \
                fprintf(stream, ", ");                                                \
            }                                                                         \
            fprintf(stream, format_specifier, v.ptr[0]);                              \
        }                                                                             \
        fprintf(stream, "]");                                                         \
    }

_IMPLEMENT_VEC(u64)
_IMPLEMENT_VEC(u8)
