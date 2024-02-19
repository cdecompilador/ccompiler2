#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define i8 int8_t
#define i16 int16_t
#define i32 int32_t
#define i64 int64_t

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t

#define true 1
#define false 0

#define FAIL(fmt, ...) \
{\
    fprintf(stderr, "error %s:%d:\n\t" fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__);\
    exit(1);\
}

#define countof(A) (sizeof(A) / sizeof(A[0]))

#endif
