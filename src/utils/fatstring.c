#include "fatstring.h"

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "type.h"

String string_new(u64 const cap) {
    String s;
    s.cap = cap;
    s.ptr = malloc(sizeof(*s.ptr) * cap);
    assert(s.ptr && "malloc failed");
    s.len = 0;
    return s;
}

void string_free(String const s) {
    free(s.ptr);
}

void string_push(String *const s, char const ch) {
    if (s->len == s->cap) {
        s->ptr = realloc(s->ptr, sizeof(*s->ptr) * (s->cap *= 2));
        assert(s->ptr && "realloc failed");
    }
    s->ptr[s->len++] = ch;
}

void string_shrink(String *const s) {
    s->cap = s->len;
    s->ptr = realloc(s->ptr, sizeof(*s->ptr) * s->cap);
    assert(s->ptr && "realloc failed");
}

String string_read(FILE *const stream) {
    i32    ch;
    String s = string_new(64);
    while (EOF != (ch = getc(stream)) && !isspace(ch)) string_push(&s, ch);
    string_push(&s, '\0');
    string_shrink(&s);
    return s;
}

String string_read_file(FILE *const stream) {
    __attribute__((unused)) u32 check = fseek(stream, 0L, SEEK_END);
    assert(!check && "fseek() error");
    u64 size = ftell(stream);
    assert(size != UINT64_MAX && "ftell() error");
    rewind(stream);
    String s = string_new(size + 1);
    fread(s.ptr, sizeof(char), size, stream);
    s.len = size;
    string_push(&s, '\0');
    return s;
}

void string_rev(String *const s) {
    for (u64 i = 0; i < s->len - 2 - i; i++) {  // '\0' at the end
        char const tmp         = s->ptr[i];
        s->ptr[i]              = s->ptr[s->len - 2 - i];
        s->ptr[s->len - 2 - i] = tmp;
    }
}

u8Vec string_to_u8vec(String const s) {
    return (u8Vec){
        .cap = s.cap,
        .len = s.len,
        .ptr = (u8 *)s.ptr,
    };
}
