#pragma once

#include <stdio.h>

#include "type.h"
#include "vec.h"

typedef struct _Message {
    u64 cap, len, byte_encoded;
    u8 *ptr;
} Message;

void  message_free(Message const m);
u64   message_print_hex(FILE *const stream, Message const m);
u8Vec message_to_u8Vec(Message const m);

Message arithmetic_encoder(u8Vec const input, u32Vec const cum_distr);
u8Vec   arithmetic_decoder(Message const input, u32Vec const cum_distr);

u32Vec cum_distr_from_rnd_u8vec(u8Vec const v);
u32Vec cum_distr_from_rnd_u8vec_unsafe(u8Vec const v);
