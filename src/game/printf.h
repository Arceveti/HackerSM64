#ifndef _PRINTF_H_
#define _PRINTF_H_
#include <ultra64.h>
#include <stdarg.h>

#include "types.h"

typedef struct
{
    union {
        /* 00 */ s64 s64;
        u64 u64;
        f64 f64;
        u32 u32;
        u16 u16;
    } value;
    /* 08 */ char* buff;
    /* 0c */ s32 part1_len;
    /* 10 */ s32 num_leading_zeros;
    /* 14 */ s32 part2_len;
    /* 18 */ s32 num_mid_zeros;
    /* 1c */ s32 part3_len;
    /* 20 */ s32 num_trailing_zeros;
    /* 24 */ s32 precision;
    /* 28 */ s32 width;
    /* 2c */ u32 size;
    /* 30 */ u32 flags;
    /* 34 */ u8 length;
} printf_struct;

enum PrintfFlags {
    FLAGS_SPACE = (1 << 0), // 0x01
    FLAGS_PLUS  = (1 << 1), // 0x02
    FLAGS_MINUS = (1 << 2), // 0x04
    FLAGS_HASH  = (1 << 3), // 0x08
    FLAGS_ZERO  = (1 << 4), // 0x10
};

ALWAYS_INLINE static char* write_to_buf(char* buffer, const char* data, size_t size) {
    return ((char*)memcpy(buffer, data, size) + size);
}

s32 _Printf(char*(*prout)(char*, const char*, size_t), char* dst, const char* fmt, va_list args);
void _Litob(printf_struct* args, unsigned char type);
void _Ldtob(printf_struct* args, unsigned char type);
#endif
