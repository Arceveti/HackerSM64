#include <macros.h>
#include <stdlib.h>
#include <string.h>
#include "printf.h"

#define BUFF_LEN 0x20

static s16 _Ldunscale(s16* pex, printf_struct* px);
static void _Genld(printf_struct* px, unsigned char code, unsigned char* p, s16 nsig, s16 xexp);

const double D_80338670[] = { 1e1L, 1e2L, 1e4L, 1e8L, 1e16L, 1e32L, 1e64L, 1e128L, 1e256L }; //! TODO: name this.

/* float properties */
#define _D0 0
#define _DBIAS 0x3ff
#define _DLONG 1
#define _DOFF 4
#define _FBIAS 0x7e
#define _FOFF 7
#define _FRND 1
#define _LBIAS 0x3ffe
#define _LOFF 15
/* integer properties */
#define _C2 1
#define _CSIGN 1
#define _ILONG 0
#define _MBMAX 8
#define NAN 2
#define INF 1
#define FINITE -1
#define _DFRAC ((1 << _DOFF) - 1)
#define _DMASK (0x7fff & ~_DFRAC)
#define _DMAX ((1 << (15 - _DOFF)) - 1)
#define _DNAN (0x8000 | (_DMAX << _DOFF) | (1 << (_DOFF - 1)))
#define _DSIGN 0x8000
#if (_D0 == 3)
#define _D1 2 /* little-endian order */
#define _D2 1
#define _D3 0
#else
#define _D1 1 /* big-endian order */
#define _D2 2
#define _D3 3
#endif

void _Ldtob(printf_struct* args, unsigned char type) {
    unsigned char buff[BUFF_LEN];
    unsigned char* ptr = buff;
    f64 val = args->value.f64;
    s16 nsig;
    s16 exp;

    if (args->precision < 0) {
        args->precision = 6;
    } else {
        if (args->precision == 0 && (type == 'g' || type == 'G')) {
            args->precision = 1;
        }
    }
    s16 err = _Ldunscale(&exp, args);
    if (err > 0) {
        memcpy(args->buff, ((err == 2) ? "NaN" : "Inf"), (args->part2_len = 3));
        return;
    }
    if (err == 0) {
        nsig = 0;
        exp = 0;
    } else {
        if (val < 0) {
            val = -val;
        }
        exp = (((exp * 30103) / 100000) - 4);
        if (exp < 0) {
            s32 n = ((3 - exp) & ~3);
            exp = -n;
            for (s32 i = 0; n > 0; n >>= 1, i++) {
                if ((n & 1) != 0) {
                    val *= D_80338670[i];
                }
            }
        } else {
            if (exp > 0) {
                f64 factor = 1;
                exp &= ~3;
                for (s32 n = exp, i = 0; n > 0; n >>= 1, i++) {
                    if ((n & 1) != 0) {
                        factor *= D_80338670[i];
                    }
                }
                val /= factor;
            }
        }
        s32 gen = (((type == 'f') ? (exp + 10) : 6) + args->precision);
        if (gen > 0x13) {
            gen = 0x13;
        }
        *ptr++ = '0';
        while (gen > 0 && val > 0) {
            s32 lo = val;
            gen -= 8;
            if (gen > 0) {
                val = ((val - lo) * 1.0e8);
            }
            ptr += 8;
            s32 j;
            for (j = 8; lo > 0 && --j >= 0;) {
                ldiv_t qr = ldiv(lo, 10);
                ptr--;
                *ptr = (qr.rem + '0');
                lo = qr.quot;
            }
            while (--j >= 0) {
                ptr--;
                *ptr = '0';
            }
            ptr += 8;
        }

        gen = (ptr - &buff[1]);
        exp += 7;
        for (ptr = &buff[1]; *ptr == '0'; ptr++) {
            gen--;
            exp--;
        }

        nsig = (((type == 'f') ? (exp + 1) : ((type == 'e' || type == 'E') ? 1 : 0)) + args->precision);
        if (gen < nsig) {
            nsig = gen;
        }
        if (nsig > 0) {
            unsigned char drop = ((nsig < gen && ptr[nsig] > '4') ? '9' : '0');

            s32 n2;
            for (n2 = nsig; ptr[--n2] == drop;) {
                nsig--;
            }
            if (drop == '9') {
                ptr[n2]++;
            }
            if (n2 < 0) {
                ptr--;
                nsig++;
                exp++;
            }
        }
    }
    _Genld(args, type, ptr, nsig, exp);
}

static s16 _Ldunscale(s16* pex, printf_struct* px) {
    unsigned short* ps = (unsigned short*)px;
    short xchar = ((ps[_D0] & _DMASK) >> _DOFF);

    if (xchar == _DMAX) { /* NaN or INF */
        *pex = 0;
        return (s16)(((ps[_D0] & _DFRAC) || ps[_D1] || ps[_D2] || ps[_D3]) ? NAN : INF);
    } else if (0 < xchar) {
        ps[_D0] = ((ps[_D0] & ~_DMASK) | (_DBIAS << _DOFF));
        *pex = (xchar - (_DBIAS - 1));
        return FINITE;
    }

    if (0 > xchar) {
        return NAN;
    } else {
        *pex = 0;
        return 0;
    }
}

static void _Genld(printf_struct* px, unsigned char code, unsigned char* p, s16 nsig, s16 xexp) {
    const unsigned char point = '.';
    if (nsig <= 0) {
        nsig = 1,

        p = (unsigned char*)"0";
    }

    if (
        code == 'f' ||
        (
            (code == 'g' || code == 'G') &&
            (-4 <= xexp) &&
            (xexp < px->precision)
        )
    ) { /* 'f' format */
        xexp++;            /* change to leading digit count */
        if (code != 'f') { /* fixup for 'g' */
            if (!(px->flags & FLAGS_HASH) && (nsig < px->precision)) {
                px->precision = nsig;
            }
            if ((px->precision -= xexp) < 0) {
                px->precision = 0;
            }
        }
        if (xexp <= 0) { /* digits only to right of point */
            px->buff[px->part2_len++] = '0';
            if ((0 < px->precision) || (px->flags & FLAGS_HASH)) {
                px->buff[px->part2_len++] = point;
            }
            if (-xexp > px->precision) {
                xexp = -px->precision;
            }
            px->num_mid_zeros = -xexp;
            px->precision += xexp;
            if (nsig > px->precision) {
                nsig = px->precision;
            }
            memcpy(&px->buff[px->part2_len], p, px->part3_len = nsig);
            px->num_trailing_zeros = px->precision - nsig;
        } else if (nsig < xexp) { /* zeros before point */
            memcpy(&px->buff[px->part2_len], p, nsig);
            px->part2_len += nsig;
            px->num_mid_zeros = xexp - nsig;
            if ((0 < px->precision) || (px->flags & FLAGS_HASH)) {
                px->buff[px->part2_len] = point;
                ++px->part3_len;
            }
            px->num_trailing_zeros = px->precision;
        } else { /* enough digits before point */
            memcpy(&px->buff[px->part2_len], p, xexp);
            px->part2_len += xexp;
            nsig -= xexp;
            if ((0 < px->precision) || (px->flags & FLAGS_HASH)) {
                px->buff[px->part2_len++] = point;
            }
            if (px->precision < nsig) {
                nsig = px->precision;
            }
            memcpy(&px->buff[px->part2_len], (p + xexp), nsig);
            px->part2_len += nsig;
            px->num_mid_zeros = (px->precision - nsig);
        }
    } else {                              /* 'e' format */
        if (code == 'g' || code == 'G') { /* fixup for 'g' */
            if (nsig < px->precision) {
                px->precision = nsig;
            }
            px->precision--;
            if (px->precision < 0) {
                px->precision = 0;
            }
            code = (code == 'g') ? 'e' : 'E';
        }
        px->buff[px->part2_len++] = *p++;
        if ((0 < px->precision) || (px->flags & FLAGS_HASH)) {
            px->buff[px->part2_len++] = point;
        }
        if (0 < px->precision) { /* put fraction digits */
            nsig--;
            if (nsig > px->precision) {
                nsig = px->precision;
            }
            memcpy(&px->buff[px->part2_len], p, nsig);
            px->part2_len += nsig;
            px->num_mid_zeros = (px->precision - nsig);
        }
        p = (unsigned char*)&px->buff[px->part2_len]; /* put exponent */
        *p++ = code;
        if (0 <= xexp) {
            *p++ = '+';
        } else { /* negative exponent */
            *p++ = '-';
            xexp = -xexp;
        }
        if (100 <= xexp) { /* put oversize exponent */
            if (1000 <= xexp) {
                *p++ = (xexp / 1000) + '0';
                xexp %= 1000;
            }
            *p++ = (xexp / 100) + '0';
            xexp %= 100;
        }
        *p++ = (xexp / 10) + '0';
        xexp %= 10;
        *p++ = xexp + '0';
        px->part3_len = (p - (unsigned char*)&px->buff[px->part2_len]);
    }

    if ((px->flags & (FLAGS_ZERO | FLAGS_MINUS)) == FLAGS_ZERO) { /* pad with leading zeros */
        s32 n = (
            px->part1_len          +
            px->part2_len          +
            px->num_mid_zeros      +
            px->part3_len          +
            px->num_trailing_zeros
        );

        if (n < px->width) {
            px->num_leading_zeros = (px->width - n);
        }
    }
}
