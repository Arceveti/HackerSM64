#include <stdarg.h>
#include <string.h>
#include "printf.h"

#define ATOI(i, a)                                                                                     \
    for ((i) = 0; *(a) >= '0' && *(a) <= '9'; (a)++) {                                                 \
        if ((i) < 999) {                                                                               \
            (i) = (*(a) + ((i) * 10) - '0');                                                           \
        }                                                                                              \
    }
#define _PROUT(dst, fmt, _size)                                                                        \
    if ((_size) > 0) {                                                                                 \
        (dst) = prout((dst), (fmt), (_size));                                                          \
        if ((dst) != 0) {                                                                              \
            printfArg.size += (_size);                                                                 \
        } else {                                                                                       \
            return printfArg.size;                                                                     \
        }                                                                                              \
    }
#define _PAD(i, m, size, src, extracond)                                                               \
    if ((extracond) && (m) > 0) {                                                                      \
        for ((i) = (m); (i) > 0; (i) -= (size)) {                                                      \
            if ((u32)(i) > 32) {                                                                       \
                (size) = 32;                                                                           \
            } else {                                                                                   \
                (size) = (i);                                                                          \
            }                                                                                          \
            _PROUT((dst), (src), (size));                                                              \
        }                                                                                              \
    }

const char length_str[] = "hlL";
const char flags_str[] = " +-#0";
const u32 flags_arr[] = { FLAGS_SPACE, FLAGS_PLUS, FLAGS_MINUS, FLAGS_HASH, FLAGS_ZERO, 0 };
char _spaces[] = "                                ";
char _zeroes[] = "00000000000000000000000000000000";

static void _Putfld(printf_struct* printfArg, va_list* args, u8 type, u8* buff);

s32 _Printf(char*(*prout)(char*, const char*, size_t), char* dst, const char* fmt, va_list args) {
    printf_struct printfArg;
    const u8* fmt_ptr;
    u8 c;
    const char* flag_index;
    u8 buff[0x20];
    s32 size, i;
    printfArg.size = 0;
    while (TRUE) {
        fmt_ptr = (u8*)fmt;
        while ((c = *fmt_ptr) != 0 && c != '%') {
            fmt_ptr++;
        }
        _PROUT(dst, fmt, fmt_ptr - (u8*)fmt);
        if (c == 0) {
            return printfArg.size;
        }
        fmt = (char*)++fmt_ptr;
        printfArg.flags = 0;
        for (; (flag_index = strchr(flags_str, *fmt_ptr)) != NULL; fmt_ptr++) {
            printfArg.flags |= flags_arr[flag_index - flags_str];
        }
        if (*fmt_ptr == '*') {
            printfArg.width = va_arg(args, s32);
            if (printfArg.width < 0) {
                printfArg.width = -printfArg.width;
                printfArg.flags |= FLAGS_MINUS;
            }
            fmt_ptr++;
        } else {
            ATOI(printfArg.width, fmt_ptr);
        }
        if (*fmt_ptr != '.') {
            printfArg.precision = -1;
        } else {
            fmt_ptr++;
            if (*fmt_ptr == '*') {
                printfArg.precision = va_arg(args, s32);
                fmt_ptr++;
            } else {
                ATOI(printfArg.precision, fmt_ptr);
            }
        }
        if (strchr(length_str, *fmt_ptr) != NULL) {
            printfArg.length = *fmt_ptr++;
        } else {
            printfArg.length = 0;
        }

        if (printfArg.length == 'l' && *fmt_ptr == 'l') {
            printfArg.length = 'L';
            fmt_ptr++;
        }
        _Putfld(&printfArg, &args, *fmt_ptr, buff);
        printfArg.width -= printfArg.part1_len + printfArg.num_leading_zeros
                         + printfArg.part2_len + printfArg.num_mid_zeros
                         + printfArg.part3_len + printfArg.num_trailing_zeros;
        _PAD(i, printfArg.width, size, _spaces, !(printfArg.flags & FLAGS_MINUS));
        _PROUT(dst, (char*)buff, printfArg.part1_len);
        _PAD(i, printfArg.num_leading_zeros, size, _zeroes, TRUE);
        _PROUT(dst, printfArg.buff, printfArg.part2_len);
        _PAD(i, printfArg.num_mid_zeros, size, _zeroes, TRUE);
        _PROUT(dst, (char*)(&printfArg.buff[printfArg.part2_len]), printfArg.part3_len)
        _PAD(i, printfArg.num_trailing_zeros, size, _zeroes, TRUE);
        _PAD(i, printfArg.width, size, _spaces, (printfArg.flags & FLAGS_MINUS));
        fmt = (char*)fmt_ptr + 1;
    }
}

static void _Putfld(printf_struct* printfArg, va_list* args, u8 type, u8* buff) {
    printfArg->part1_len = printfArg->num_leading_zeros = printfArg->part2_len = printfArg->num_mid_zeros = printfArg->part3_len =
        printfArg->num_trailing_zeros = 0;

    switch (type) {
        case 'c':
            buff[printfArg->part1_len++] = va_arg(*args, u32);
            break;

        case 'd':
        case 'i':
            if (printfArg->length == 'l') {
                printfArg->value.s64 = va_arg(*args, s32);
            } else if (printfArg->length == 'L') {
                printfArg->value.s64 = va_arg(*args, s64);
            } else {
                printfArg->value.s64 = va_arg(*args, s32);
            }

            if (printfArg->length == 'h') {
                printfArg->value.s64 = (s16)printfArg->value.s64;
            }

            if (printfArg->value.s64 < 0) {
                buff[printfArg->part1_len++] = '-';
            } else if (printfArg->flags & FLAGS_PLUS) {
                buff[printfArg->part1_len++] = '+';
            } else if (printfArg->flags & FLAGS_SPACE) {
                buff[printfArg->part1_len++] = ' ';
            }

            printfArg->buff = (char*)&buff[printfArg->part1_len];

            _Litob(printfArg, type);
            break;

        case 'x':
        case 'X':
        case 'u':
        case 'o':
            if (printfArg->length == 'l') {
                printfArg->value.s64 = va_arg(*args, s32);
            } else if (printfArg->length == 'L') {
                printfArg->value.s64 = va_arg(*args, s64);
            } else {
                printfArg->value.s64 = va_arg(*args, s32);
            }

            if (printfArg->length == 'h') {
                printfArg->value.s64 = (u16)printfArg->value.s64;
            } else if (printfArg->length == 0) {
                printfArg->value.s64 = (u32)printfArg->value.s64;
            }

            if (printfArg->flags & FLAGS_HASH) {
                buff[printfArg->part1_len++] = '0';
                if (type == 'x' || type == 'X') {
                    buff[printfArg->part1_len++] = type;
                }
            }
            printfArg->buff = (char*)&buff[printfArg->part1_len];
            _Litob(printfArg, type);
            break;

        case 'e':
        case 'f':
        case 'g':
        case 'E':
        case 'G':
            //... okay?
            printfArg->value.f64 = (printfArg->length == 'L') ? va_arg(*args, f64) : va_arg(*args, f64);

            if (printfArg->value.u16 & 0x8000) {
                buff[printfArg->part1_len++] = '-';
            } else {
                if (printfArg->flags & FLAGS_PLUS) {
                    buff[printfArg->part1_len++] = '+';
                } else if (printfArg->flags & FLAGS_SPACE) {
                    buff[printfArg->part1_len++] = ' ';
                }
            }

            printfArg->buff = (char*)&buff[printfArg->part1_len];
            _Ldtob(printfArg, type);
            break;

        case 'n':
            if (printfArg->length == 'h') {
                *(va_arg(*args, u16*)) = printfArg->size;
            } else if (printfArg->length == 'l') {
                *va_arg(*args, u32*) = printfArg->size;
            } else if (printfArg->length == 'L') {
                *va_arg(*args, u64*) = printfArg->size;
            } else {
                *va_arg(*args, u32*) = printfArg->size;
            }
            break;

        case 'p':
            printfArg->value.s64 = (intptr_t)va_arg(*args, void*);
            printfArg->buff = (char*)&buff[printfArg->part1_len];
            _Litob(printfArg, 'x');
            break;

        case 's':
            printfArg->buff = va_arg(*args, char*);
            printfArg->part2_len = strlen(printfArg->buff);
            if (printfArg->precision >= 0 && printfArg->part2_len > printfArg->precision) {
                printfArg->part2_len = printfArg->precision;
            }
            break;

        case '%':
            buff[printfArg->part1_len++] = '%';
            break;

        default:
            buff[printfArg->part1_len++] = type;
            break;
    }
}
