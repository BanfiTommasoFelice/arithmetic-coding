#include "utils/arithmetic-coding.h"
#include "utils/type.h"
#include "utils/vec.h"

#define LEN 6

i32 main(void) {
    char m[LEN + 1] = "210013";
    fprintf(stdout, "%s\n", m);
    u8Vec data = u8vec_new(LEN);
    for (u32 i = 0; i < LEN; i++) u8vec_push(&data, m[i]);
    u32Vec cum_distr = u32vec_new(sizeof(*data.ptr) << 8);
    cum_distr.len    = cum_distr.cap;
    for (u32 i = 0; i < '0'; i++) cum_distr.ptr[i] = 0;
    cum_distr.ptr['0'] = 0;
    cum_distr.ptr['1'] = (u64)UINT32_MAX * 2 / 10;
    cum_distr.ptr['2'] = (u64)UINT32_MAX * 7 / 10;
    cum_distr.ptr['3'] = (u64)UINT32_MAX * 9 / 10;
    for (u32 i = '3' + 1; i < cum_distr.len; i++) cum_distr.ptr[i] = UINT32_MAX;
    Message encoded = arithmetic_encoder(data, cum_distr);
    message_print_hex(stdout, encoded);
    fprintf(stdout, "\n");
    u8vec_free(data);
    u32vec_free(cum_distr);
    message_free(encoded);
    return 0;
}
