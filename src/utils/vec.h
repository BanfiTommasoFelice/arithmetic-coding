#pragma once

#include "type.h"
#include "stdio.h"

typedef struct _u64Vec {
    u64  cap, len;
    u64 *ptr;
} u64Vec;

u64Vec u64vec_new(u64 cap);
u64Vec u64vec_new_init(u64 cap, u64 n);
void   u64vec_shrink(u64Vec *v);
u64Vec u64vec_clone(u64Vec v);
void   u64vec_resize(u64Vec *v, u64 cap);
void   u64vec_double(u64Vec *v);
void   u64vec_push(u64Vec *v, u64 digit);
void   u64vec_free(u64Vec v);
void   u64vec_print(FILE *stream, u64Vec v);
void   u64vec_print_rev(FILE *stream, u64Vec v);
