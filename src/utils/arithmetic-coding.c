#include "arithmetic-coding.h"

#include <assert.h>
#include <stdlib.h>

#include "type.h"
#include "vec.h"

#define INIT_BIT 256u
#define D        256u  // don't change! with 2^4 and 2^16 it does not work!
#define D_BIT    __builtin_ctz(D)
#define P        32u / D_BIT
static_assert((1ULL << (D_BIT * P)) == 0x100000000, "we use u32 for operations!");
#define DtoP_1 (1u << (D_BIT * (P - 1)))  // D^(P-1)
#define DtoP_2 (1u << (D_BIT * (P - 2)))  // D^(P-2)

Message message_from_partialmessage(PartialMessage m) {
    assert(!m.outstanding);
    return (Message){
        .cap = m.cap,
        .len = m.len,
        .ptr = m.ptr,
    };
}

PartialMessage partialmessage_new(u64 cap) {
    assert(cap);
    PartialMessage m = {
        .cap = cap,
        .len = 0,
        .ptr = malloc(m.cap * sizeof(*m.ptr)),
    };
    assert(m.ptr);
    return m;
}

void message_free(Message m) {
    free(m.ptr);
}

void partialmessage_resize(PartialMessage *m, u64 cap) {
    if (m->cap == cap) return;
    m->len = min(m->len, cap);
    m->cap = cap;
    m->ptr = realloc(m->ptr, cap * sizeof(*m->ptr));
    assert(m->ptr);
}

void partialmessage_push(PartialMessage *m, u32 symbol) {
    if (symbol != D - 1) {
        for (; m->outstanding; m->outstanding--) partialmessage_push_unchecked(m, D - 1);
        partialmessage_push_unchecked(m, symbol);
    } else m->outstanding++;
}

void partialmessage_push_unchecked(PartialMessage *m, u32 symbol) {
    if (m->len >= m->cap) partialmessage_resize(m, m->cap << 1);
    m->ptr[m->len++] = symbol;
}

u64 message_print_hex(FILE *stream, Message m) {
    u64 retval      = 0;
    u32 upper_limit = (m.len + 7) >> 3;
    for (u32 i = 0; i < upper_limit; i++) retval += fprintf(stream, "%2x", m.ptr[i]);
    return retval;
}

Message arithmetic_encoder(u8Vec input, u32Vec cum_distr) {
    u32            base   = 0;
    u32            len    = UINT32_MAX;
    PartialMessage output = partialmessage_new(INIT_BIT);
    for (u64 i = 0; i < input.len; i++) {
        interval_update(input.ptr[i], cum_distr, &output, &base, &len);
        if (len < DtoP_1) encoder_renormalization(&base, &len, &output);
    }
    code_value_selection(&base, &len, &output);
    return message_from_partialmessage(output);
}

void interval_update(u8 symbol, u32Vec cum_distr, PartialMessage *output, u32 *base, u32 *len) {
    u32 y  = symbol == cum_distr.len - 1 ? *len : (((u64)*len * cum_distr.ptr[symbol + 1]) >> 32);
    u32 a  = *base;
    u32 x  = ((u64)*len * cum_distr.ptr[symbol]) >> 32;
    *base += x;
    *len   = y - x;
    assert(len);  // otherwise probability(symbol) = 0!
    if (a > *base) propagate_carry(output);
}

void propagate_carry(PartialMessage *output) {
    output->ptr[output->len - 1]++;
    for (; output->outstanding; output->outstanding--) partialmessage_push_unchecked(output, 0);
}

void encoder_renormalization(u32 *base, u32 *len, PartialMessage *output) {
    while (*len < DtoP_1) {
        partialmessage_push(output, ((u64)*base * D) >> 32);
        *len  *= D;
        *base *= D;
    }
}

void code_value_selection(u32 *base, u32 *len, PartialMessage *output) {
    u32 a = *base;
    *base = (*base + (DtoP_1 << 1));
    *len  = DtoP_1 - 1;
    if (a > *base) propagate_carry(output);
    encoder_renormalization(base, len, output);
}
