// Copyright 2020 Joshua J Baker. All rights reserved.

#include "varint.h"

// varint_write_u writes a uint64 varint to data, which could be to 10 bytes.
// Make sure that you provide a data buffer that can take 10 bytes!
// Returns the number of bytes written.
int varint_write_u(void *data, uint64_t x) {
    char *str = data;
    uint64_t n = 0;
    n+=x>=0x80; str[0]=x|0x80; x>>=7;
    n+=x>=0x80; str[1]=x|0x80; x>>=7;
    n+=x>=0x80; str[2]=x|0x80; x>>=7;
    n+=x>=0x80; str[3]=x|0x80; x>>=7;
    n+=x>=0x80; str[4]=x|0x80; x>>=7;
    n+=x>=0x80; str[5]=x|0x80; x>>=7;
    n+=x>=0x80; str[6]=x|0x80; x>>=7;
    n+=x>=0x80; str[7]=x|0x80; x>>=7;
    n+=x>=0x80; str[8]=x|0x80; x>>=7;
    n+=x>=0x80; str[9]=x|0x80; x>>=7;
    str[n] ^= 0x80;
    return n+1;
}

// varint_write_i writes a int64 varint to data, which could be to 10 bytes.
// Make sure that you provide a data buffer that can take 10 bytes!
// Returns the number of bytes written.
int varint_write_i(void *buf, int64_t x) {
    uint64_t ux = (uint64_t)x << 1;
    if (x < 0) {
        ux = ~ux;
    }
    return varint_write_u(buf, ux);
}

// varint_read_u reads a uint64 varint from data. 
// Returns the number of bytes reads, or returns 0 if there's not enough data
// to complete the read, or returns -1 if the data buffer does not represent
// a valid uint64 varint.
int varint_read_u(const void *data, size_t len, uint64_t *x) {
    const char *str = data;
    uint64_t b;
    *x = 0;
    if (len==0) return 0; b=str[0]; *x|=(b&0x7f)<<(7*0); if (b<0x80) return 0+1;
    if (len==1) return 0; b=str[1]; *x|=(b&0x7f)<<(7*1); if (b<0x80) return 1+1;
    if (len==2) return 0; b=str[2]; *x|=(b&0x7f)<<(7*2); if (b<0x80) return 2+1;
    if (len==3) return 0; b=str[3]; *x|=(b&0x7f)<<(7*3); if (b<0x80) return 3+1;
    if (len==4) return 0; b=str[4]; *x|=(b&0x7f)<<(7*4); if (b<0x80) return 4+1;
    if (len==5) return 0; b=str[5]; *x|=(b&0x7f)<<(7*5); if (b<0x80) return 5+1;
    if (len==6) return 0; b=str[6]; *x|=(b&0x7f)<<(7*6); if (b<0x80) return 6+1;
    if (len==7) return 0; b=str[7]; *x|=(b&0x7f)<<(7*7); if (b<0x80) return 7+1;
    if (len==8) return 0; b=str[8]; *x|=(b&0x7f)<<(7*8); if (b<0x80) return 8+1;
    if (len==9) return 0; b=str[9]; *x|=(b&0x7f)<<(7*9); if (b<0x80) return 9+1;
    return -1;
}

// varint_read_i reads a int64 varint from data. 
// Returns the number of bytes reads, or returns 0 if there's not enough data
// to complete the read, or returns -1 if the data buffer does not represent
// a valid int64 varint.
int varint_read_i(const void *data, size_t len, int64_t *x) {
    uint64_t ux;
    int n = varint_read_u(data, len, &ux);
    *x = (int64_t)(ux >> 1);
    if ((ux&1) != 0) {
        *x = ~*x;
    }
    return n;
}

//==============================================================================
// TESTS
// $ cc -DVARINT_TEST buf.c && ./a.out              # run tests
//==============================================================================
#ifdef VARINT_TEST
#include <math.h>
#include <time.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
#endif
