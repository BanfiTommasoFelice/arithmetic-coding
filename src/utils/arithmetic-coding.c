#include "arithmetic-coding.h"

#include <assert.h>
#include <stdlib.h>

#include "type.h"
#include "vec.h"

#define INIT_CAP   4u
#define D          256u  // don't change! 2^(2^x), x = 1,2,3,4 -> does not work for others
#define D_BIT      __builtin_ctz(D)
#define P          32u / D_BIT
#define DtoP_1     (1u << (D_BIT * (P - 1)))  // D^(P-1)
#define DtoP_2     (1u << (D_BIT * (P - 2)))  // D^(P-2)

#define MSBP(x, y) (((u64)(x) * (y)) >> 32)  // most significant bits product
#define _to_f32(x) ((f32)(x) / 0x100000000)

static_assert((1ULL << (D_BIT * P)) == 0x100000000, "we use u32 for operations!");

Message message_from_partialmessage(PartialMessage m, u64 byte_encoded) {
    assert(!m.outstanding && "message was not finished");
    return (Message){
        .cap          = m.cap,
        .len          = m.len,
        .ptr          = m.ptr,
        .byte_encoded = byte_encoded,
    };
}

PartialMessage partialmessage_new(u64 cap) {
    assert(cap && "cap should be != 0");
    PartialMessage m = {
        .cap = cap,
        .len = 0,
        .ptr = malloc(m.cap * sizeof(*m.ptr)),
    };
    assert(m.ptr && "malloc failed");
    return m;
}

void message_free(Message m) {
    free(m.ptr);
}

// @todo compilte-time abstraction: functions are identical
void partialmessage_resize(PartialMessage *m, u64 cap) {
    if (m->cap == cap) return;
    m->len = min(m->len, cap);
    m->cap = cap;
    m->ptr = realloc(m->ptr, cap * sizeof(*m->ptr));
    assert(m->ptr && "realloc failed");
}

void message_resize(Message *m, u64 cap) {
    if (m->cap == cap) return;
    m->len = min(m->len, cap);
    m->cap = cap;
    m->ptr = realloc(m->ptr, cap * sizeof(*m->ptr));
    assert(m->ptr && "realloc failed");
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

void message_pad_with_zeroes(Message *m, u32 n) {
    if (m->len + n > m->cap) message_resize(m, m->len + n);
    for (u32 i = m->len; i < m->len + n; i++) m->ptr[i] = 0;
}

Message arithmetic_encoder(u8Vec input, u32Vec cum_distr) {
    u32            base   = 0;
    u32            len    = UINT32_MAX;
    PartialMessage output = partialmessage_new(INIT_CAP);
    for (u64 i = 0; i < input.len; i++) {
        interval_update(input.ptr[i], cum_distr, &output, &base, &len);
        if (len < DtoP_1) encoder_renormalization(&base, &len, &output);
    }
    code_value_selection(&base, &len, &output);
    return message_from_partialmessage(output, input.len);
}

void interval_update(u8 symbol, u32Vec cum_distr, PartialMessage *output, u32 *base, u32 *len) {
    u32 y  = symbol == cum_distr.len - 1 ? *len : MSBP(*len, cum_distr.ptr[symbol + 1]);
    u32 a  = *base;
    u32 x  = MSBP(*len, cum_distr.ptr[symbol]);
    *base += x;
    *len   = y - x;
    assert(*len && "otherwise probability(symbol) == 0");
    if (a > *base) propagate_carry(output);
}

void propagate_carry(PartialMessage *output) {
    output->ptr[output->len - 1]++;
    for (; output->outstanding; output->outstanding--) partialmessage_push_unchecked(output, 0);
}

void encoder_renormalization(u32 *base, u32 *len, PartialMessage *output) {
    while (*len < DtoP_1) {
        partialmessage_push(output, MSBP(*base, D));
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

u8Vec arithmetic_decoder(Message *input_ptr, u32Vec cum_distr) {
    message_pad_with_zeroes(input_ptr, P);
    u32     len          = UINT32_MAX;
    u64     byte_decoded = P;
    u32     value        = 0;
    Message input        = *input_ptr;
    u8Vec   output       = u8vec_new(input.byte_encoded);
    for (u32 i = 0; i < P; i++) value += (u32)input.ptr[i] << ((P - i - 1) << 3);
    for (u32 k = 0; k < input.byte_encoded; k++) {
        u8 symbol = interval_selection(&value, &len, &cum_distr);
        u8vec_push(&output, symbol);
        if (len < DtoP_1) decoder_renormalization(&value, &len, &byte_decoded, input);
    }
    return output;
}

u8 interval_selection(u32 *value, u32 *len, u32Vec *cum_distr) {
    u32 lb_idx = 0, ub_idx = cum_distr->len;
    u32 lb_int = 0, ub_int = *len;
    while (ub_idx - lb_idx > 1) {
        u32 mid_idx = (lb_idx + ub_idx) >> 1;
        u32 mid_int = MSBP(*len, cum_distr->ptr[mid_idx]);
        if (mid_int > *value) {
            ub_idx = mid_idx;
            ub_int = mid_int;
        } else {
            lb_idx = mid_idx;
            lb_int = mid_int;
        }
    }
    *value = *value - lb_int;
    *len   = ub_int - lb_int;
    return lb_idx;
}

void decoder_renormalization(u32 *value, u32 *len, u64 *byte_decoded, Message input) {
    while (*len < DtoP_1) {
        *value = (*value << D_BIT) + input.ptr[*byte_decoded++];
        *len   = (*len << D_BIT);
    }
}
