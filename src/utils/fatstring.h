#pragma once

#include <stdint.h>
#include <stdio.h>

#include "type.h"
#include "vec.h"

typedef struct _string {
    u64   len;
    u64   cap;
    char *ptr;
} String;

String string_new(uint64_t const cap);
void   string_free(String const s);
void   string_push(String *const s, char const ch);
void   string_shrink(String *const s);
String string_read(FILE *const stream);
String string_read_file(FILE *const stream);
void   string_rev(String *const s);
u8Vec  string_to_u8vec(String const s);
