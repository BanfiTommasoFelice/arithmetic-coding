#include "bignum.h"
#include "type.h"

// @todo check appendix for better function declarations
// @todo base and len should be u64 or u32 or u16?
// @todo.later write to a file?

// the first bit is `.ptr[0] & 0x01`, the second `.ptr[0] & 0x02`, ...,
// the last `.ptr[len >> 3] & (1 << (len & 0x7))`
typedef struct _Message {
    u64 cap, len, byte_encoded;
    u8 *ptr;
} Message;

typedef struct _PartialMessage {
    u64 cap, len, outstanding;
    u8 *ptr;
} PartialMessage;

Message        message_from_partialmessage(PartialMessage m, u64 byte_encoded);
u64            message_print_hex(FILE *stream, Message m);

PartialMessage partialmessage_new(u64 cap);
void           partialmessage_resize(PartialMessage *m, u64 cap);
void           message_resize(Message *m, u64 cap);
void           partialmessage_push(PartialMessage *m, u32 symbol);
void           partialmessage_push_unchecked(PartialMessage *m, u32 symbol);
void           message_free(Message m);
void           message_pad_with_zeroes(Message *input, u32 n);

Message        arithmetic_encoder(u8Vec input, u32Vec cum_distr);
void  interval_update(u8 symbol, u32Vec cum_distr, PartialMessage *output, u32 *base, u32 *len);
void  propagate_carry(PartialMessage *output);
void  encoder_renormalization(u32 *base, u32 *len, PartialMessage *output);
void  code_value_selection(u32 *base, u32 *len, PartialMessage *output);

u8Vec arithmetic_decoder(Message *input, u32Vec cum_distr);
u8    interval_selection(u32 *value, u32 *len, u32Vec *cum_distr);
void  decoder_renormalization(u32 *value, u32 *len, u64 *byte_decoded, Message input);

u32Vec         cum_distr_from_rnd_u8vec(u8Vec v);
