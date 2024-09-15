#include "arithmetic-coding.h"

#include <assert.h>
#include <stdlib.h>

typedef struct _PartialMessage {
    u64 cap, len, outstanding;
    u8 *ptr;
} PartialMessage;

#define INIT_CAP   4u
#define D          256u  // don't change! 2^(2^x), x in {1,2,3,4} -> does not work for others
#define D_BIT      __builtin_ctz(D)
#define P          32u / D_BIT
#define DtoP_1     (1u << (D_BIT * (P - 1)))  // D^(P-1)
#define DtoP_2     (1u << (D_BIT * (P - 2)))  // D^(P-2)
#define MSBP(x, y) (((u64)(x) * (y)) >> 32)   // most significant bits product
#define _to_f32(x) ((f32)(x) / 0x100000000)

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)  // std >= c11
static_assert((1ULL << (D_BIT * P)) == 0x100000000, "Use u32 for operations!");
#endif

static Message        message_from_partialmessage(PartialMessage const m, u64 const byte_encoded);
static PartialMessage partialmessage_new(u64 cap);

static void           partialmessage_resize(PartialMessage *const m, u64 const cap);
static void           message_resize(Message *const m, u64 const cap);
static void           partialmessage_push(PartialMessage *const m, u32 const symbol);
static void           partialmessage_push_unchecked(PartialMessage *const m, u32 const symbol);

static void interval_update(u8 const symbol, u32Vec const cum_distr, PartialMessage *const output,
                            u32 *const base, u32 *const len);
static void propagate_carry(PartialMessage *const output);
static void encoder_renormalization(u32 *const base, u32 *const len, PartialMessage *const output);
static void code_value_selection(u32 *const base, u32 *const len, PartialMessage *const output);

static u8   interval_selection(u32 *const value, u32 *const len, u32Vec const cum_distr);
static void decoder_renormalization(u32 *const value, u32 *const len, u64 *const byte_decoded,
                                    Message const input);

static Message message_from_partialmessage(PartialMessage const m, u64 const byte_encoded) {
    assert(!m.outstanding && "message was not finished");
    return (Message){
        .cap          = m.cap,
        .len          = m.len,
        .ptr          = m.ptr,
        .byte_encoded = byte_encoded,
    };
}

static PartialMessage partialmessage_new(u64 const cap) {
    assert(cap && "cap should be != 0");
    PartialMessage const m = {
        .cap = cap,
        .len = 0,
        .ptr = malloc(m.cap * sizeof(*m.ptr)),
    };
    assert(m.ptr && "malloc failed");
    return m;
}

void message_free(Message const m) {
    free(m.ptr);
}

static void partialmessage_resize(PartialMessage *const m, u64 const cap) {
    if (m->cap == cap) return;
    m->len = min(m->len, cap);
    m->cap = cap;
    m->ptr = realloc(m->ptr, cap * sizeof(*m->ptr));
    assert(m->ptr && "realloc failed");
}

static void message_resize(Message *const m, u64 const cap) {
    if (m->cap == cap) return;
    m->len = min(m->len, cap);
    m->cap = cap;
    m->ptr = realloc(m->ptr, cap * sizeof(*m->ptr));
    assert(m->ptr && "realloc failed");
}

static void partialmessage_push(PartialMessage *const m, u32 const symbol) {
    if (symbol != D - 1) {
        for (; m->outstanding; m->outstanding--) partialmessage_push_unchecked(m, D - 1);
        partialmessage_push_unchecked(m, symbol);
    } else m->outstanding++;
}

static void partialmessage_push_unchecked(PartialMessage *const m, u32 const symbol) {
    if (m->len >= m->cap) partialmessage_resize(m, m->cap << 1);
    m->ptr[m->len++] = symbol;
}

u64 message_print_hex(FILE *const stream, Message const m) {
    u64 retval      = 0;
    u32 upper_limit = m.len;
    for (u32 i = 0; i < upper_limit; i++) retval += fprintf(stream, "%02x", m.ptr[i]);
    return retval;
}

static void message_pad_with_zeroes(Message *const m, u32 const n) {
    if (m->len + n > m->cap) message_resize(m, m->len + n);
    for (u32 i = m->len; i < m->len + n; i++) m->ptr[i] = 0;
}

Message arithmetic_encoder(u8Vec const input, u32Vec const cum_distr) {
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

static void interval_update(u8 const symbol, u32Vec const cum_distr, PartialMessage *const output,
                            u32 *const base, u32 *const len) {
    u32 const y  = symbol == cum_distr.len - 1 ? *len : MSBP(*len, cum_distr.ptr[symbol + 1]);
    u32 const a  = *base;
    u32 const x  = MSBP(*len, cum_distr.ptr[symbol]);
    *base       += x;
    *len         = y - x;
    assert(*len && "otherwise probability(symbol) == 0");
    if (a > *base) propagate_carry(output);
}

static void propagate_carry(PartialMessage *const output) {
    output->ptr[output->len - 1]++;
    for (; output->outstanding; output->outstanding--) partialmessage_push_unchecked(output, 0);
}

static void encoder_renormalization(u32 *const base, u32 *const len, PartialMessage *const output) {
    while (*len < DtoP_1) {
        partialmessage_push(output, MSBP(*base, D));
        *len  *= D;
        *base *= D;
    }
}

static void code_value_selection(u32 *const base, u32 *const len, PartialMessage *const output) {
    u32 const a = *base;
    *base       = (*base + (DtoP_1 >> 1));
    *len        = DtoP_2 - 1;
    if (a > *base) propagate_carry(output);
    encoder_renormalization(base, len, output);
}

u8Vec arithmetic_decoder(Message const input, u32Vec const cum_distr) {
    u32   len          = UINT32_MAX;
    u64   byte_decoded = P;
    u32   val          = 0;
    u8Vec output       = u8vec_new(input.byte_encoded);
    for (u32 i = 0; i < P; i++) val += (u32)input.ptr[i] << ((P - i - 1) << 3);
    for (u32 k = 0; k < input.byte_encoded; k++) {
        const u8 symbol = interval_selection(&val, &len, cum_distr);
        u8vec_push(&output, symbol);
        if (len < DtoP_1) decoder_renormalization(&val, &len, &byte_decoded, input);
    }
    return output;
}

static u8 interval_selection(u32 *const val, u32 *const len, u32Vec const cum_distr) {
    u32 lb_idx = 0, ub_idx = cum_distr.len;
    u32 lb_int = 0, ub_int = *len;

    while (ub_idx - lb_idx > 1) {  // D_BIT loops
        u32 const mid_idx = (lb_idx + ub_idx) >> 1;
        u32 const mid_int = MSBP(*len, cum_distr.ptr[mid_idx]);
        u32       cond    = mid_int > *val;                          // if (mid_int > *val) {
        u32       mask_0  = cond - 1;                                //     ub_idx = mid_idx;
        u32       mask_1  = !cond - 1;                               //     ub_int = mid_int;
        ub_idx            = (ub_idx & mask_0) ^ (mid_idx & mask_1);  // } else {
        ub_int            = (ub_int & mask_0) ^ (mid_int & mask_1);  //     lb_idx = mid_idx;
        lb_idx            = (lb_idx & mask_1) ^ (mid_idx & mask_0);  //     lb_int = mid_int;
        lb_int            = (lb_int & mask_1) ^ (mid_int & mask_0);  // }
    }

    *val = *val - lb_int;
    *len = ub_int - lb_int;
    return lb_idx;
}

static void decoder_renormalization(u32 *const val, u32 *const len, u64 *const byte_decoded,
                                    Message const input) {
    while (*len < DtoP_1) {
        *val = (*val << D_BIT) + input.ptr[(*byte_decoded)++];
        *len = (*len << D_BIT);
    }
}

u32Vec cum_distr_from_rnd_u8vec(u8Vec const data) {
    // the minimum probability usable is D^(1-P) -> normalized to u32 is D
    assert(data.len <= UINT32_MAX && "not supported yet");
    const u32 distr_len = sizeof(*data.ptr) << 8;
    u32Vec    distr     = u32vec_new_init(distr_len, 0);
    distr.len           = distr.cap;
    for (u32 i = 0; i < data.len; i++) distr.ptr[data.ptr[i]]++;
    const u64 prob_max = UINT32_MAX - D * distr_len;
    u64       tot      = distr.ptr[0];
    distr.ptr[0]       = 0;
    for (u32 i = 1; i < distr_len; i++) {
        u32 const val  = tot * prob_max / data.len + (i << D_BIT);
        tot           += distr.ptr[i];
        distr.ptr[i]   = val;
        assert(distr.ptr[i] > distr.ptr[i - 1] && val - distr.ptr[i - 1] >= D &&
               "probability too low");
    }
    assert(0x100000000 - distr.ptr[distr.len - 1] >= D && "probability too low");
    return distr;
}

u32Vec cum_distr_from_rnd_u8vec_unsafe(u8Vec const data) {
    // the minimum probability usable is D^(1-P) -> normalized to u32 is D
    assert(data.len <= UINT32_MAX && "not supported yet");
    const u32 distr_len = sizeof(*data.ptr) << 8;
    u32Vec    distr     = u32vec_new_init(distr_len, 0);
    distr.len           = distr.cap;
    for (u32 i = 0; i < data.len; i++) distr.ptr[data.ptr[i]]++;
    u64 tot   = 0;
    u8  check = 0;
    for (u32 i = 0; i < distr_len; i++) {
        u32 const val = tot * UINT32_MAX / data.len;
        if (check) assert(val - distr.ptr[i - 1] >= D && "probability too low");
        check         = !!distr.ptr[i];
        tot          += distr.ptr[i];
        distr.ptr[i]  = val;
    }
    if (check) assert(0x100000000 - distr.ptr[distr.len - 1] >= D && "probability too low");
    return distr;
}
