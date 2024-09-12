#include <assert.h>
#include <string.h>

#include "utils/arithmetic-coding.h"
#include "utils/type.h"
#include "utils/vec.h"

i32 main(void) {
    char  m[]      = "abdjfdakfjaskdlfuweuifuasdiofjaskdfjweklaj;ejafl;klejflakwe";

    u32   orig_len = strlen(m);

    u8Vec data     = u8vec_new(orig_len + 1);
    for (u32 i = 0; i < orig_len; i++) u8vec_push(&data, m[i]);

    u32Vec cum_distr = cum_distr_from_rnd_u8vec(data);

    u8vec_push(&data, '\0');
    fprintf(stdout, "Data(char)   : `%s`\n", data.ptr);
    u8vec_pop(&data);

    Message encoded = arithmetic_encoder(data, cum_distr);
    fprintf(stdout, "Encoded(char): `");
    message_print_hex(stdout, encoded);
    fprintf(stdout, "`\n");

    u8Vec decoded = arithmetic_decoder(&encoded, cum_distr);
    u8vec_push(&decoded, '\0');
    fprintf(stdout, "Decoded(hex) : `%s`\n", decoded.ptr);
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
