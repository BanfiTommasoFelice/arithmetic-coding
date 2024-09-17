#include <assert.h>
#include <sys/time.h>

#include "utils/arithmetic-coding.h"
#include "utils/fatstring.h"
#include "utils/type.h"
#include "utils/vec.h"

// #define PRINT_CONTENT

#define time_of(statement, var) \
    gettimeofday(&start, NULL); \
    statement;                  \
    gettimeofday(&end, NULL);   \
    var = (end.tv_sec - start.tv_sec) + (f64)(end.tv_usec - start.tv_usec) / 1e6;

i32 main(void) {
    u8Vec const    data = string_to_u8vec(string_read_file(stdin));
    struct timeval end, start;
    time_of(u32Vec const cum_distr = cum_distr_from_rnd_u8vec(data), f64 const stat_time);
    time_of(Message const encoded = arithmetic_encoder(data, cum_distr), f64 const encoding_time);
    time_of(u8Vec const decoded = arithmetic_decoder(encoded, cum_distr), f64 const decoding_time);

#ifndef PRINT_CONTENT
    fprintf(stderr, "Data         : %6lu bytes\n", data.len);
    fprintf(stderr, "Encoded      : %6lu bytes\n", encoded.len);
#else
    fprintf(stderr, "Data         : %6lu bytes\n", data.len);
    fprintf(stderr, "`%s`\n", data.ptr);
    fprintf(stderr, "Encoded      : %6lu bytes\n", encoded.len);
    fprintf(stderr, "`");
    message_print_hex(stderr, encoded);
    fprintf(stderr, "`\n");
    fprintf(stderr, "Decoded      : %6lu bytes\n", decoded.len);
    fprintf(stderr, "`%s`\n", decoded.ptr);
#endif

    fflush(stdout);
    assert(data.len == decoded.len && "original data len and decoded len are different");
    for (u32 i = 0; i < data.len; i++)
        assert(data.ptr[i] == decoded.ptr[i] && "original data and decoded data are different");

    fprintf(stderr, "Compression  : %6.2f %%\n", (double)encoded.len * 100 / data.len);
    fprintf(stderr, "Get stat time: %6.3f seconds \n", stat_time);
    fprintf(stderr, "Encoding time: %6.3f seconds \n", encoding_time);
    fprintf(stderr, "Decoding time: %6.3f seconds \n", decoding_time);

    u8vec_free(data);
    u32vec_free(cum_distr);
    message_free(encoded);
    u8vec_free(decoded);
    return 0;
}
