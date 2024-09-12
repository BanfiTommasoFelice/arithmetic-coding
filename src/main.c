#include <assert.h>
#include <string.h>

#include "utils/arithmetic-coding.h"
#include "utils/type.h"
#include "utils/vec.h"

i32 main(void) {
    char const m[] =
        "Lovin' can hurt\n\
Lovin' can hurt sometimes\n\
But it's the only thing that I know\n\
And when it gets hard\n\
You know it can get hard sometimes\n\
It is the only thing that makes us feel alive";

    u32 const orig_len = strlen(m);

    u8Vec     data     = u8vec_new(orig_len + 1);
    for (u32 i = 0; i < orig_len; i++) u8vec_push(&data, m[i]);

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
    assert(orig_len == decoded.len && "original data len and decoded len are different");
    for (u32 i = 0; i < orig_len; i++)
        assert(m[i] == decoded.ptr[i] && "original data and decoded data are different");

    fprintf(stdout, "Compression  : %f", (double)encoded.len / orig_len);

    u8vec_free(data);
    u32vec_free(cum_distr);
    message_free(encoded);
    u8vec_free(decoded);
    return 0;
}
