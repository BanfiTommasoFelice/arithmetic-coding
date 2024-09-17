#include <assert.h>
#include <stdlib.h>
#include <sys/time.h>

#include "utils/arithmetic-coding.h"
#include "utils/type.h"
#include "utils/vec.h"

// #define PRINT_CONTENT

#define time_of(statement, var) \
    gettimeofday(&start, NULL); \
    statement;                  \
    gettimeofday(&end, NULL);   \
    var = (end.tv_sec - start.tv_sec) + (f64)(end.tv_usec - start.tv_usec) / 1e6;

i32 main(i32 __attribute__((unused)) argc, char **argv) {
    assert(argc == 3 && "argc != 3");
    u64 n      = strtoull(argv[1], NULL, 0);
    u64 n_test = strtoull(argv[2], NULL, 0);

    struct timeval seed;
    gettimeofday(&seed, NULL);
    srand(seed.tv_usec);

    u64   passed = 0;
    u8Vec data   = u8vec_new(n);
    for (u64 i = 0; i < n_test; i++) {
        u8vec_fill_rnd_distr(&data, n);
        struct timeval end, start;
        u32Vec const   cum_distr = cum_distr_from_rnd_u8vec(data);
        time_of(Message const encoded = arithmetic_encoder(data, cum_distr),
                f64 const encoding_time);
        time_of(u8Vec const decoded = arithmetic_decoder(encoded, cum_distr),
                f64 const decoding_time);

        u8 wrong  = 0;
        wrong    |= (data.len != decoded.len);
        for (u32 i = 0; i < data.len; i++) wrong |= (data.ptr[i] != decoded.ptr[i]);
        passed += !wrong;


        printf("Test %3lu %s. ", i, wrong ? "not passed" : "passed");
        printf("Encoding: %6.3f sec - ", encoding_time);
        printf("Decoding: %6.3f sec - ", decoding_time);
        printf("Compression: %6.2f %%\n", (double)encoded.len * 100 / data.len);
        if (wrong) {
            printf("    Data         : %6lu bytes\n", data.len);
            printf("    `%s`\n", data.ptr);
            printf("    Encoded      : %6lu bytes\n", encoded.len);
            printf("    `");
            message_print_hex(stdout, encoded);
            printf("`\n");
            printf("    Decoded      : %6lu bytes\n", decoded.len);
            printf("    `%s`\n", decoded.ptr);
        }
        u32vec_free(cum_distr);
        message_free(encoded);
        u8vec_free(decoded);
    }
    u8vec_free(data);
    printf("Total passed: %lu / %lu\n", passed, n_test);

    return 0;
}
