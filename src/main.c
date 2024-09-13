#include <assert.h>
#include <sys/time.h>

#include "utils/arithmetic-coding.h"
#include "utils/fatstring.h"
#include "utils/type.h"
#include "utils/vec.h"

// #define PRINT_CONTENT

i32 main(void) {
    u8Vec        data      = string_to_u8vec(string_read_file(stdin));
    u32Vec const cum_distr = cum_distr_from_rnd_u8vec(data);

    fprintf(stdout, "Data         : %6lu bytes\n", data.len);
#ifdef PRINT_CONTENT
    fprintf(stdout, "`%s`\n", data.ptr);
#endif

    struct timeval end, start;
    gettimeofday(&start, NULL);
    Message encoded = arithmetic_encoder(data, cum_distr);
    gettimeofday(&end, NULL);
    f64 encoding_time = (end.tv_sec - start.tv_sec) + (f64)(end.tv_usec - start.tv_usec) / 1e6;

    fprintf(stdout, "Encoded      : %6lu bytes\n", encoded.len);
#ifdef PRINT_CONTENT
    fprintf(stdout, "`");
    message_print_hex(stdout, encoded);
    fprintf(stdout, "`\n");
#endif

    gettimeofday(&start, NULL);
    u8Vec decoded = arithmetic_decoder(&encoded, cum_distr);
    gettimeofday(&end, NULL);
    f64 decoding_time = (end.tv_sec - start.tv_sec) + (f64)(end.tv_usec - start.tv_usec) / 1e6;
#ifdef PRINT_CONTENT
    fprintf(stdout, "Decoded    : %6lu bytes\n", decoded.len);
    fprintf(stdout, "`%s`\n", decoded.ptr);
#endif

    fflush(stdout);
    assert(data.len == decoded.len && "original data len and decoded len are different");
    for (u32 i = 0; i < data.len; i++)
        assert(data.ptr[i] == decoded.ptr[i] && "original data and decoded data are different");

    fprintf(stdout, "Compression  : %6.2f %%\n", (double)encoded.len * 100 / data.len);
    fprintf(stdout, "Encoding time: %6.3f seconds \n", encoding_time);
    fprintf(stdout, "Decoding time: %6.3f seconds \n", decoding_time);


    u8vec_free(data);
    u32vec_free(cum_distr);
    message_free(encoded);
    u8vec_free(decoded);
    return 0;
}
