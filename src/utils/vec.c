#include "vec.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "stdio.h"

u64Vec u64vec_new(u64 cap) {
    u64Vec v = {
        .cap = cap,
        .len = 0,
        .ptr = malloc(v.cap * sizeof(*v.ptr)),
    };
    assert(v.ptr);
    return v;
}

u64Vec u64vec_clone(u64Vec v) {
    u64Vec w = {
        .cap = v.cap,
        .len = v.len,
        .ptr = malloc(v.cap * sizeof(*v.ptr)),
    };
    assert(w.ptr);
    memcpy(w.ptr, v.ptr, w.cap * sizeof(*v.ptr));
    return w;
}

void u64vec_shrink(u64Vec *v) {
    if (v->len < v->cap) {
        v->ptr = realloc(v->ptr, v->len);
        assert(v->ptr);
        v->cap = v->len;
    } else assert(v->len == v->cap);
}

void u64vec_resize(u64Vec *v, u64 cap) {
    if (v->cap == cap) return;
    v->cap = cap;
    v->ptr = realloc(v->ptr, cap * sizeof(*v->ptr));
    assert(v->ptr);
}

void u64vec_push(u64Vec *v, u64 digit) {
    if (v->len == v->cap) u64vec_resize(v, v->cap * 2);
    v->ptr[v->len] = digit;
    v->len++;
}

void u64vec_free(u64Vec v) {
    free(v.ptr);
}

void u64vec_print(FILE *stream, u64Vec v) {
    fprintf(stream, "[");
    for (u32 i = 0; i < v.len - 1; i++) fprintf(stream, "%lu, ", v.ptr[i]);
    if (v.len) fprintf(stream, "%lu", v.ptr[v.len - 1]);
    fprintf(stream, "]");
}

void u64vec_print_rev(FILE *stream, u64Vec v) {
    fprintf(stream, "[");
    if (v.len) {
        for (u32 i = v.len - 1; i != 0; i--) fprintf(stream, "%lu, ", v.ptr[i]);
        fprintf(stream, "%lu", v.ptr[0]);
    }
    fprintf(stream, "]");
}
