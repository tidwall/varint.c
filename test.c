// Test the varint.c library
// $ cc -Wextra -Werror -pedantic varint.c test.c && ./a.out

#include <math.h>
#include <time.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "varint.h"

static void rand_fill(void *data, size_t len) {
    uint8_t *bytes = data;
    for (size_t i = 0; i < len; i++) {
        bytes[i] = rand();
    }
}

static uint64_t rand_uint(void) {
    uint64_t x;
    rand_fill(&x, 8);
    int n = rand()%20;
    if (n >= 0 && n <= 18) {
        uint64_t m = 1;
        for (int i = 0; i < n; i++) {
            m *= 10;
        }
        return x % m;
    }
    return x;
}

static int64_t rand_int(void) {
    return rand_uint();
}

struct stats {
    size_t ns[11];
    size_t zeros;
    size_t bads;
    size_t oks;
};

void stats_add(struct stats *stats, int n) {
    assert(n >= -1 && n <= 10);
    if (n <= 0) {
        stats->zeros += n == 0;
        stats->bads += n == -1;
    } else {
        stats->oks++;
        stats->ns[n]++;
    }
}

void stats_print(struct stats *stats) {
    printf("  ok=%zu (", stats->oks);
    for (int i = 1; i <= 10; i++) {
        if (i > 1) {
            printf(" ");
        }
        printf("%zu", stats->ns[i]);
    }
    printf(")\n  zero=%zu\n  bad=%zu\n", stats->zeros, stats->bads);
}

void test_randoms(int steps, int size) {
    int N = size;
    printf("[RANDOMS] ");
    uint64_t *uints = malloc(sizeof(uint64_t)*N);
    char *data = malloc(10*N);
    assert(uints && data);
    struct stats stats = { 0 };
    for (int h = 0; h < steps; h++) {
        for (int i = 0, n = 0; i < N; i++) {
            uints[i] = rand_uint();
            int nn = varint_write_u(data+n, uints[i]);
            assert(nn > 0);
            n += nn;
            stats_add(&stats, nn);
        }
        for (int i = 0, n = 0; i < N; i++) {
            uint64_t x;
            int nn = varint_read_u(data+n, 10*N-n, &x);
            assert(nn > 0);
            assert(x == uints[i]);
            n += nn;
            stats_add(&stats, nn);
        }
        int64_t *ints = (int64_t*)uints;
        for (int i = 0, n = 0; i < N; i++) {
            ints[i] = rand_int();
            int nn = varint_write_i(data+n, ints[i]);
            assert(nn > 0);
            n += nn;
            stats_add(&stats, nn);
        }
        for (int i = 0, n = 0; i < N; i++) {
            int64_t x;
            int nn = varint_read_i(data+n, 10*N-n, &x);
            assert(nn > 0);
            assert(x == ints[i]);
            n += nn;
            stats_add(&stats, nn);
        }
        printf(".");
        fflush(stdout);
    }
    free(uints);
    free(data);
    printf("\n");
    stats_print(&stats);
}


void test_fuzzing(int steps, int size) {
    int N = size;
    printf("[FUZZING] ");
    int sz = 10*N/2;
    char buf[20];
    struct stats stats = { 0 };
    for (int h = 0; h < steps; h++) {
        for (int i = 0; i < sz; i++) {
            size_t len = rand()&15;
            rand_fill(buf, len);
            int64_t x;
            int n = varint_read_i(buf, len, &x);
            stats_add(&stats, n);
        }
        for (int i = 0; i < sz; i++) {
            size_t len = rand()&15;
            rand_fill(buf, len);
            uint64_t x;
            int n = varint_read_u(buf, len, &x);
            stats_add(&stats, n);
        }
        printf(".");
        fflush(stdout);
    }
    printf("\n");
    stats_print(&stats);
}

int main(void) {
    printf("Running varint.c tests...\n");
    int seed = getenv("SEED")?atoi(getenv("SEED")):time(NULL);
    printf("seed=%d\n", seed);
    srand(seed);
    int steps = 50;
    int size = 100000;
    test_randoms(steps, size);
    test_fuzzing(steps, size);
    printf("PASSED\n");
}
