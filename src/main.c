#include <assert.h>
#include <stdio.h>

#include "utils/bignum.h"
#include "utils/type.h"

i32 main(void) {
    u32    d;
    BigNum n = bignum_read_hex(stdin);
    fscanf(stdin, "%x", &d);
    bignum_print_hex(stdout, n);
    fprintf(stdout, "\n");
    bignum_print_base(stdout, n, 2);
    fprintf(stdout, "\n");
    bignum_print_base(stdout, n, 10);
    fprintf(stdout, "\n");
    bignum_print_base(stdout, n, 132);
    fprintf(stdout, "\n");
    bignum_free(n);
    return 0;
}
