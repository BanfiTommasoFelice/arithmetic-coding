#include <assert.h>

#include "utils/arithmetic-coding.h"
#include "utils/fatstring.h"
#include "utils/type.h"
#include "utils/vec.h"

i32 main(void) {
    u8Vec        data      = string_to_u8vec(string_read_to_eof(stdin));
    u32Vec const cum_distr = cum_distr_from_rnd_u8vec(data);

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
    assert(data.len == decoded.len && "original data len and decoded len are different");
    for (u32 i = 0; i < data.len; i++)
        assert(data.ptr[i] == decoded.ptr[i] && "original data and decoded data are different");

    fprintf(stdout, "Compression  : %f", (double)encoded.len / data.len);

    u8vec_free(data);
    u32vec_free(cum_distr);
    message_free(encoded);
    u8vec_free(decoded);
    return 0;
}
