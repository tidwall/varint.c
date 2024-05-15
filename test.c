// Test the varint.c library
// $ cc varint.c test.c && ./a.out

#include <math.h>
#include <time.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "varint.h"

static uint64_t rand_uint() {
     return (((uint64_t)rand()<<33) | 
            ((uint64_t)rand()<<1) | 
            ((uint64_t)rand()<<0)) >> (rand()&63);
}

static int64_t rand_int() {
     return (int64_t)(rand_uint()) * ((rand()&1)?1:-1);
}

static void rand_fill(char *data, int len) {
    int i = 0;
    for (; i < len-3; i += 3) {
        uint32_t r = rand();
        data[i+0] = r;
        data[i+1] = r >> 8;
        data[i+2] = r >> 16;
    }
    for (; i < len; i++) {
        data[i] = rand();
    }
}

int main() {
    printf("Running varint.c tests...\n");
    int seed = getenv("SEED")?atoi(getenv("SEED")):time(NULL);
    printf("seed=%d\n", seed);
    srand(seed);
    int N = 500000;
    uint64_t *uints = malloc(sizeof(uint64_t)*N);
    char *data = malloc(10*N);
    assert(uints && data);
    printf("[RANDOMS] ");
    for (int h = 0; h < 50; h++) {
        for (int i = 0, n = 0; i < N; i++) {
            uints[i] = rand_uint();
            int nn = varint_write_u(data+n, uints[i]);
            assert(nn > 0);
            n += nn;
        }
        for (int i = 0, n = 0; i < N; i++) {
            uint64_t x;
            int nn = varint_read_u(data+n, 10*N-n, &x);
            assert(nn > 0);
            assert(x == uints[i]);
            n += nn;
        }
        int64_t *ints = (int64_t*)uints;
        for (int i = 0, n = 0; i < N; i++) {
            ints[i] = rand_int();
            int nn = varint_write_i(data+n, ints[i]);
            assert(nn > 0);
            n += nn;
        }
        for (int i = 0, n = 0; i < N; i++) {
            int64_t x;
            int nn = varint_read_i(data+n, 10*N-n, &x);
            assert(nn > 0);
            assert(x == ints[i]);
            n += nn;
        }
        printf(".");
        fflush(stdout);
    }
    printf("\n");
    printf("[FUZZING] ");
    int sz = 10*N/2;
    for (int h = 0; h < 50; h++) {
        rand_fill(data, sz);
        for (int i = 0; i < sz; i++) {
            int64_t x;
            varint_read_i(data+i, sz-i, &x);
        }
        for (int i = 0; i < sz; i++) {
            uint64_t x;
            varint_read_u(data+i, sz-i, &x);
        }
        printf(".");
        fflush(stdout);
    }
    printf("\n");
    free(uints);
    free(data);
    printf("PASSED\n");
}
