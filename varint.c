// https://github.com/tidwall/varint.c
//
// Copyright 2024 Joshua J Baker. All rights reserved.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file.
//
// varint.c: Read and write variable sized integers in C.

#include "varint.h"

#ifdef __GNUC__
// ignore gcc warning for varint_read_u single line guards.
#pragma GCC diagnostic ignored "-Wmisleading-indentation"
#endif 

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
