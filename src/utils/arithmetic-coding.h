#include "bignum.h"
#include "type.h"

// @todo.later write to a file?

// the first bit is `.ptr[0] & 0x01`, the second `.ptr[0] & 0x02`, ...,
// the last `.ptr[len >> 3] & (1 << (len & 0x7))`
typedef struct _Message {
    u64 cap, len, byte_encoded;
    u8 *ptr;
} Message;

u64            message_print_hex(FILE *stream, Message m);

Message        arithmetic_encoder(u8Vec input, u32Vec cum_distr);

u8Vec          arithmetic_decoder(Message *input, u32Vec cum_distr);

u32Vec         cum_distr_from_rnd_u8vec(u8Vec v);
