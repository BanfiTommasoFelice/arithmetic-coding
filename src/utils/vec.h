#pragma once

#include "stdio.h"
#include "type.h"

// Achtung: custom free are not implement yet: not vector of pointers!

#define _DECLARE_VEC(_type_)                                                                       \
    typedef struct _##_type_##Vec {                                                                \
        u64     cap, len;                                                                          \
        _type_ *ptr;                                                                               \
    } _type_##Vec;                                                                                 \
                                                                                                   \
    _type_##Vec _type_##vec_new(u64 const cap);                                                    \
    _type_##Vec _type_##vec_new_init(u64 const cap, _type_ const digit);                           \
    void        _type_##vec_shrink(_type_##Vec *const v);                                          \
    _type_##Vec _type_##vec_clone(_type_##Vec const v);                                            \
    void        _type_##vec_resize(_type_##Vec *const v, u64 const cap);                           \
    void        _type_##vec_double(_type_##Vec *v);                                          const \
    _type_      _type_##vec_pop(_type_##Vec *const v);                                             \
    void        _type_##vec_push(_type_##Vec *const v, _type_ const digit);                        \
    void        _type_##vec_free(_type_##Vec const v);                                             \
    void _type_##vec_print(FILE *const stream, _type_##Vec const v, char const *const format_specifier); \
    void _type_##vec_print_rev(FILE *const stream, _type_##Vec const v,                                  \
                               char const *const format_specifier);

_DECLARE_VEC(u64)
_DECLARE_VEC(u32)
_DECLARE_VEC(u16)
_DECLARE_VEC(u8)
