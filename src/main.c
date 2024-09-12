#include <assert.h>
#include <string.h>

#include "utils/arithmetic-coding.h"
#include "utils/type.h"
#include "utils/vec.h"

i32 main(void) {
    char  m[]      = "210013";
    u32   orig_len = strlen(m);

    u8Vec data     = u8vec_new(orig_len + 1);
    for (u32 i = 0; i < orig_len; i++) u8vec_push(&data, m[i]);

    u32Vec cum_distr = u32vec_new(sizeof(*data.ptr) << 8);
    cum_distr.len    = cum_distr.cap;
    for (u32 i = 0; i < '0'; i++) cum_distr.ptr[i] = 0;
    cum_distr.ptr['0'] = 0;
    cum_distr.ptr['1'] = (u64)UINT32_MAX * 2 / 10;
    cum_distr.ptr['2'] = (u64)UINT32_MAX * 7 / 10;
    cum_distr.ptr['3'] = (u64)UINT32_MAX * 9 / 10;
    for (u32 i = '3' + 1; i < cum_distr.len; i++) cum_distr.ptr[i] = UINT32_MAX;

    u8vec_push(&data, '\0');
    fprintf(stdout, "Data   : %s\n", data.ptr);
    u8vec_pop(&data);

    Message encoded = arithmetic_encoder(data, cum_distr);
    fprintf(stdout, "Encoded: ");
    message_print_hex(stdout, encoded);
    fprintf(stdout, "\n");

    u8Vec decoded = arithmetic_decoder(&encoded, cum_distr);
    u8vec_push(&decoded, '\0');
    fprintf(stdout, "Data   : %s\n", decoded.ptr);
    u8vec_pop(&decoded);

    fflush(stdout);
    assert(orig_len == decoded.len && "original data len and decoded len are different");
    for (u32 i = 0; i < orig_len; i++)
        assert(m[i] == decoded.ptr[i] && "original data and decoded data are different");

    u8vec_free(data);
    u32vec_free(cum_distr);
    message_free(encoded);
    u8vec_free(decoded);
    return 0;
}
