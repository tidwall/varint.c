# varint.c

Read and write variable sized integers in C.  
Compatible with Protobuf and Go varints.

## API
```c
// read uint64 or int64 from data. returns the number of bytes read or returns
// the zero if there is not enough data or returns -1 if the data does not
// represent a valid varint.
int varint_read_u(const void *data, size_t len, uint64_t *x);
int varint_read_i(const void *data, size_t len, int64_t *x);

// write a uint64 or int64 to data buffer. The data buffer must be at least
// 10 bytes in order to hold the maximum sized varint. Returns the number of
// bytes written.
int varint_write_u(void *data, uint64_t x);
int varint_write_i(void *data, int64_t x);
```

## Example

```c
#include <stdio.h>
#include "varint.h"

int main() {
    char buf[100];
    int n;
    int64_t i;
    uint64_t u;

    // write and read a signed integer
    i = -305;
    n = varint_write_i(buf, i);
    printf("wrote the value %lld (%d bytes)\n", (long long)i, n);
    n = varint_read_i(buf, sizeof(buf), &i);
    printf("read the value %lld (%d bytes)\n", (long long)i, n);


    // write and read an unsigned integer
    u = 102993;
    n = varint_write_u(buf, u);
    printf("wrote the value %llu (%d bytes)\n", (unsigned long long)u, n);
    n = varint_read_u(buf, sizeof(buf), &u);
    printf("read the value %lld (%d bytes)\n", (unsigned long long)u, n);
}
// output:
// wrote the value -305 (2 bytes)
// read the value -305 (2 bytes)
// wrote the value 102993 (3 bytes)
// read the value 102993 (3 bytes)
```

## Running tests

```sh
$ cc -DVARINT_TEST varint.c && ./a.out
```
