#ifndef MATH_UTIL_H
#define MATH_UTIL_H

#include <PR/ultratypes.h>

#include "types.h"


#define NEAR_ZERO   __FLT_EPSILON__
#define NEAR_ONE    (1.0f - __FLT_EPSILON__)

#define FLOAT_ONE   0x3F800000


extern Mat4 identityMtx;
extern Vec3f gVec3fZero;
extern Vec3s gVec3sZero;
extern Vec3i gVec3iZero;
extern Vec3f gVec3fOne;
extern Vec3s gVec3sOne;


// Common cos values for degrees, often used for surface Y normals.
// This is different than coss(), which uses s16 angles.
#define COS0    1.0f
#define COS1    0.99984770f // mario_floor_is_slippery, mario_floor_is_slope
#define COS5    0.99619470f // mario_floor_is_slope
#define COS10   0.98480775f // act_butt_slide_air, act_hold_butt_slide_air, mario_floor_is_slippery, mario_floor_is_slope
#define COS15   0.96592583f // mario_floor_is_slope
#define COS20   0.93969262f // mario_floor_is_slippery, mario_floor_is_slope, mario_floor_is_steep
#define COS25   0.90630779f // ledge grabs
#define COS30   0.86602540f // should_get_stuck_in_ground, mario_floor_is_steep
#define COS35   0.81915204f
#define COS36   0.80901699f
#define COS38   0.78801075f // mario_floor_is_slippery default
#define COS40   0.76604444f
#define COS45   0.70710678f // SURFACE_FLAG_X_PROJECTION
#define COS50   0.64278761f
#define COS55   0.57357644f
#define COS60   0.5f
#define COS65   0.42261826f
#define COS70   0.34202014f
#define COS73   0.29237169f // common_landing_cancels, check_common_idle_cancels, check_common_hold_idle_cancels
#define COS75   0.25881905f
#define COS78   0.20791169f
#define COS80   0.17364818f // braking action
#define COS85   0.087155743f
#define COS90   0.0f


// Trig functions

extern const f32 gSineTable[];
#define gCosineTable (gSineTable + 0x400)

#define sins(x) gSineTable[  (u16)(x) >> 4]
#define coss(x) gCosineTable[(u16)(x) >> 4]

#define tans(x) ({          \
    __auto_type _x = (x);     \
    sins(_x) / coss(_x);    \
})
#define cots(x) ({          \
    __auto_type _x = (x);     \
    coss(_x) / sins(_x);    \
})

#define atans(x) gArctanTable[(s32)((((x) * 1024) + 0.5f))] // is this correct? used for atan2_lookup

#define RAD_PER_DEG (M_PI / 180.0f)
#define DEG_PER_RAD (180.0f / M_PI)

#define angle_to_degrees(x)   (f32)((s16)(x) * 360.0f / (f32)0x10000)
#define degrees_to_angle(x)   (s16)((f32)(x) * (f32)0x10000 / 360.0f)
#define angle_to_radians(x)   (f32)((s16)(x) * M_PI / (f32)0x8000)
#define radians_to_angle(x)   (s16)((f32)(x) * (f32)0x8000 / M_PI)
#define degrees_to_radians(x) (f32)((f32)(x) * M_PI / 180.0f)
#define radians_to_degrees(x) (f32)((f32)(x) * 180.0f / M_PI)

/**
 * Converts an angle in degrees to sm64's s16 angle units. For example, DEGREES(90) == 0x4000
 * This should be used mainly to make camera code clearer at first glance.
 */
#define DEGREES(x) degrees_to_angle(x)

#define signum_positive(x) ((x < 0) ? -1 : 1)

#undef min
#define min(a, b) ({        \
    __auto_type _a = (a);   \
    __auto_type _b = (b);   \
    (_a < _b) ? _a : _b;    \
})
#ifndef MIN
#define MIN(a, b) min((a), (b))
#endif

#undef max
#define max(a, b) ({        \
    __auto_type _a = (a);   \
    __auto_type _b = (b);   \
    (_a > _b) ? _a : _b;    \
})
#ifndef MAX
#define MAX(a, b) max((a), (b))
#endif

#define ABS(x) ({           \
    __auto_type = (x);      \
    (_x > 0) ? _x : -_x;    \
})

#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))

#define CLAMP2(x, low, high)  ({ \
    __auto_type _x    = (x);    \
    __auto_type _low  = (low);  \
    __auto_type _high = (high); \
    (((_x) > (_high)) ? (_high) : (((_x) < (_low)) ? (_low) : (_x))); \
})

// from limits.h
#define S8_MAX __SCHAR_MAX__
#define S8_MIN (-S8_MAX - 1)
#define U8_MAX ((S8_MAX * 2) + 1)
#define S16_MAX __SHRT_MAX__
#define S16_MIN (-S16_MAX - 1)
#define U16_MAX ((S16_MAX * 2) + 1)
#define S32_MAX __INT_MAX__
#define S32_MIN (-S32_MAX - 1)
#define U32_MAX ((S32_MAX * 2U) + 1U)
#define S64_MAX __LONG_LONG_MAX__
#define S64_MIN (-S64_MAX - 1LL)
#define U64_MAX ((S64_MAX * 2ULL) + 1ULL)
#define F32_MAX __FLT_MAX__
#define F32_MIN __FLT_MIN__
#define F64_MAX __DBL_MAX__
#define F64_MIN __DBL_MIN__

#define CLAMP_U8( x)        CLAMP((x),     0x0,  U8_MAX)
#define CLAMP_S8( x)        CLAMP((x),  S8_MIN,  S8_MAX)
#define CLAMP_U16(x)        CLAMP((x),     0x0, U16_MAX)
#define CLAMP_S16(x)        CLAMP((x), S16_MIN, S16_MAX)
#define CLAMP_U32(x)        CLAMP((x),     0x0, U32_MAX)
#define CLAMP_S32(x)        CLAMP((x), S32_MIN, S32_MAX)
#define CLAMP_U64(x)        CLAMP((x),     0x0, U64_MAX)
#define CLAMP_S64(x)        CLAMP((x), S64_MIN, S64_MAX)
#define CLAMP_F32(x)        CLAMP((x), F32_MIN, F32_MAX)
#define CLAMP_F64(x)        CLAMP((x), F64_MIN, F64_MAX)

#define SWAP(a, b)          do { ((a) ^= (b)); ((b) ^= (a)); ((a) ^= (b)); } while (0)

#define sqr(x)              (    (x) * (x))
#define cube(x)             ( sqr(x) * (x))
#define quad(x)             (cube(x) * (x))

#define average_2(a, b      )   (((a) + (b)            ) / 2.0f)
#define average_3(a, b, c   )   (((a) + (b) + (c)      ) / 3.0f)
#define average_4(a, b, c, d)   (((a) + (b) + (c) + (d)) / 4.0f)

#define vec2_same(v, s)     (((v)[0]) = ((v)[1])                       = (s))
#define vec3_same(v, s)     (((v)[0]) = ((v)[1]) = ((v)[2])            = (s))
#define vec4_same(v, s)     (((v)[0]) = ((v)[1]) = ((v)[2]) = ((v)[3]) = (s))

#define vec2_zero(v)        (vec2_same((v), 0))
#define vec3_zero(v)        (vec3_same((v), 0))
#define vec4_zero(v)        (vec4_same((v), 0))

#define vec2_c(v)           (   (v)[0] + (v)[1])
#define vec3_c(v)           (vec2_c(v) + (v)[2])
#define vec4_c(v)           (vec3_c(v) + (v)[3])

#define vec2_average(v)     (vec2_c(v) / 2.0f)
#define vec3_average(v)     (vec3_c(v) / 3.0f)
#define vec4_average(v)     (vec4_c(v) / 4.0f)

#define vec2_sumsq(v)       (  sqr((v)[0]) + sqr((v)[1]))
#define vec3_sumsq(v)       (vec2_sumsq(v) + sqr((v)[2]))
#define vec4_sumsq(v)       (vec3_sumsq(v) + sqr((v)[3]))

#define vec2_mag(v)         (sqrtf(vec2_sumsq(v)))
#define vec3_mag(v)         (sqrtf(vec3_sumsq(v)))
#define vec4_mag(v)         (sqrtf(vec4_sumsq(v)))

#define vec3_yaw(from, to)  (atan2s(((to)[2] - (from)[2]), ((to)[0] - (from)[0])))

#define vec2_dot(a, b)       (((a)[0] * (b)[0]) + ((a)[1] * (b)[1]))
#define vec3_dot(a, b)      (vec2_dot((a), (b)) + ((a)[2] * (b)[2]))
#define vec4_dot(a, b)      (vec3_dot((a), (b)) + ((a)[3] * (b)[3]))

/// Make vector 'dest' the cross product of vectors a and b.
#define vec3_cross(dst, a, b) do {                      \
    __auto_type _x1 = (a)[0];                           \
    __auto_type _y1 = (a)[1];                           \
    __auto_type _z1 = (a)[2];                           \
    __auto_type _x2 = (b)[0];                           \
    __auto_type _y2 = (b)[1];                           \
    __auto_type _z2 = (b)[2];                           \
    __auto_type _x = ((_y1 * _z2) - (_z1 * _y2));       \
    __auto_type _y = ((_z1 * _x2) - (_x1 * _z2));       \
    __auto_type _z = ((_x1 * _y2) - (_y1 * _x2));       \
    (dst)[0] = _x;                                      \
    (dst)[1] = _y;                                      \
    (dst)[2] = _z;                                      \
} while (0)

/**
 * Set 'dest' the normal vector of a triangle with vertices a, b and c.
 * Equivalent to cross((c-b), (c-a)).
 */
#define find_vector_perpendicular_to_plane(dest, a, b, c) do {                                  \
    (dest)[0] = ((b)[1] - (a)[1]) * ((c)[2] - (b)[2]) - ((c)[1] - (b)[1]) * ((b)[2] - (a)[2]);  \
    (dest)[1] = ((b)[2] - (a)[2]) * ((c)[0] - (b)[0]) - ((c)[2] - (b)[2]) * ((b)[0] - (a)[0]);  \
    (dest)[2] = ((b)[0] - (a)[0]) * ((c)[1] - (b)[1]) - ((c)[0] - (b)[0]) * ((b)[1] - (a)[1]);  \
} while (0)

/**
 * | ? ? ? 0 |
 * | ? ? ? 0 |
 * | ? ? ? 0 |
 * | 0 0 0 1 |
 * i.e. a matrix representing a linear transformation over 3 space.
 */
// Multiply a vector by a matrix of the form
#define linear_mtxf_mul_vec3(mtx, dstV, srcV) do {                                                  \
    (dstV)[0] = (((mtx)[0][0] * (srcV)[0]) + ((mtx)[1][0] * (srcV)[1]) + ((mtx)[2][0] * (srcV)[2]));\
    (dstV)[1] = (((mtx)[0][1] * (srcV)[0]) + ((mtx)[1][1] * (srcV)[1]) + ((mtx)[2][1] * (srcV)[2]));\
    (dstV)[2] = (((mtx)[0][2] * (srcV)[0]) + ((mtx)[1][2] * (srcV)[1]) + ((mtx)[2][2] * (srcV)[2]));\
} while (0)

#define linear_mtxf_mul_vec3_and_translate(mtx, dstV, srcV) do {\
    linear_mtxf_mul_vec3((mtx), (dstV), (srcV));                \
    vec3_add((dstV), (mtx)[3]);                                 \
} while (0)

// Multiply a vector by the transpose of a matrix of the form
#define linear_mtxf_transpose_mul_vec3(mtx, dstV, srcV) do {\
    (dstV)[0] = vec3_dot((mtx)[0], (srcV));                 \
    (dstV)[1] = vec3_dot((mtx)[1], (srcV));                 \
    (dstV)[2] = vec3_dot((mtx)[2], (srcV));                 \
} while (0)

#define vec2_set(dst, x, y) do {            \
    (dst)[0] = (x);                         \
    (dst)[1] = (y);                         \
} while (0)
#define vec3_set(dst, x, y, z) do {         \
    (dst)[0] = (x);                         \
    (dst)[1] = (y);                         \
    (dst)[2] = (z);                         \
} while (0)
#define vec4_set(dst, x, y, z, w) do {      \
    (dst)[0] = (x);                         \
    (dst)[1] = (y);                         \
    (dst)[2] = (z);                         \
    (dst)[3] = (w);                         \
} while (0)

#define vec2_copy(dst, src) do {            \
    typeof((src)[0]) _x = (src)[0];         \
    typeof((src)[1]) _y = (src)[1];         \
    (dst)[0] = _x;                          \
    (dst)[1] = _y;                          \
} while (0)
#define vec3_copy(dst, src) do {            \
    typeof((src)[0]) _x = (src)[0];         \
    typeof((src)[1]) _y = (src)[1];         \
    typeof((src)[2]) _z = (src)[2];         \
    (dst)[0] = _x;                          \
    (dst)[1] = _y;                          \
    (dst)[2] = _z;                          \
} while (0)
#define vec4_copy(dst, src) do {            \
    typeof((src)[0]) _x = (src)[0];         \
    typeof((src)[1]) _y = (src)[1];         \
    typeof((src)[2]) _z = (src)[2];         \
    typeof((src)[3]) _w = (src)[3];         \
    (dst)[0] = _x;                          \
    (dst)[1] = _y;                          \
    (dst)[2] = _z;                          \
    (dst)[3] = _w;                          \
} while (0)

#define vec3_copy_y_off(dst, src, y) do {   \
    (dst)[0] =  (src)[0];                   \
    (dst)[1] = ((src)[1] + (y));            \
    (dst)[2] =  (src)[2];                   \
} while (0)

#define vec2_copy_roundf(dst, src) do {     \
    (dst)[0] = roundf((src)[0]);            \
    (dst)[1] = roundf((src)[1]);            \
} while (0)
#define vec3_copy_roundf(dst, src) do {     \
    (dst)[0] = roundf((src)[0]);            \
    (dst)[1] = roundf((src)[1]);            \
    (dst)[2] = roundf((src)[2]);            \
} while (0)
#define vec4_copy_roundf(dst, src) do {     \
    (dst)[0] = roundf((src)[0]);            \
    (dst)[1] = roundf((src)[1]);            \
    (dst)[2] = roundf((src)[2]);            \
    (dst)[3] = roundf((src)[3]);            \
} while (0)

#define vec2_copy_inverse(dst, src) do {    \
    (dst)[0] = (src)[1];                    \
    (dst)[1] = (src)[0];                    \
}
#define vec3_copy_inverse(dst, src) do {    \
    (dst)[0] = (src)[2];                    \
    (dst)[1] = (src)[1];                    \
    (dst)[2] = (src)[0];                    \
} while (0)
#define vec4_copy_inverse(dst, src) do {    \
    (dst)[0] = (src)[3];                    \
    (dst)[1] = (src)[2];                    \
    (dst)[2] = (src)[1];                    \
    (dst)[3] = (src)[0];                    \
} while (0)

#define vec3_copy_offset_m1(dst, src) do {  \
    (dst)[0] = (src)[1];                    \
    (dst)[1] = (src)[2];                    \
    (dst)[2] = (src)[0];                    \
} while (0)

#define vec2_copy_negative(dst, src) do {   \
    (dst)[0] = -(src)[0];                   \
    (dst)[1] = -(src)[1];                   \
} while (0)
#define vec3_copy_negative(dst, src) do {   \
    (dst)[0] = -(src)[0];                   \
    (dst)[1] = -(src)[1];                   \
    (dst)[2] = -(src)[2];                   \
} while (0)
#define vec4_copy_negative(dst, src) do {   \
    (dst)[0] = -(src)[0];                   \
    (dst)[1] = -(src)[1];                   \
    (dst)[2] = -(src)[2];                   \
    (dst)[3] = -(src)[3];                   \
} while (0)

#define vec2_sum(dst, src1, src2) do {      \
    (dst)[0] = ((src1)[0] + (src2)[0]);     \
    (dst)[1] = ((src1)[1] + (src2)[1]);     \
} while (0)
#define vec3_sum(dst, src1, src2) do {      \
    (dst)[0] = ((src1)[0] + (src2)[0]);     \
    (dst)[1] = ((src1)[1] + (src2)[1]);     \
    (dst)[2] = ((src1)[2] + (src2)[2]);     \
} while (0)
#define vec4_sum(dst, src1, src2) do {      \
    (dst)[0] = ((src1)[0] + (src2)[0]);     \
    (dst)[1] = ((src1)[1] + (src2)[1]);     \
    (dst)[2] = ((src1)[2] + (src2)[2]);     \
    (dst)[3] = ((src1)[3] + (src2)[3]);     \
} while (0)

#define vec2_add(dst, src) vec2_sum((dst), (dst), (src))
#define vec3_add(dst, src) vec3_sum((dst), (dst), (src))
#define vec4_add(dst, src) vec4_sum((dst), (dst), (src))

#define vec2_sum_val(dst, src, x) do {      \
    (dst)[0] = ((src)[0] + (x));            \
    (dst)[1] = ((src)[1] + (x));            \
} while (0)
#define vec3_sum_val(dst, src, x) do {      \
    (dst)[0] = ((src)[0] + (x));            \
    (dst)[1] = ((src)[1] + (x));            \
    (dst)[2] = ((src)[2] + (x));            \
} while (0)
#define vec4_sum_val(dst, src, x) do {      \
    (dst)[0] = ((src)[0] + (x));            \
    (dst)[1] = ((src)[1] + (x));            \
    (dst)[2] = ((src)[2] + (x));            \
    (dst)[3] = ((src)[2] + (x));            \
} while (0)

#define vec2_add_val(dst, x) vec2_sum_val((dst), (dst), (x))
#define vec3_add_val(dst, x) vec3_sum_val((dst), (dst), (x))
#define vec4_add_val(dst, x) vec4_sum_val((dst), (dst), (x))

#define vec2_diff(dst, src1, src2) do {     \
    (dst)[0] = ((src1)[0] - (src2)[0]);     \
    (dst)[1] = ((src1)[1] - (src2)[1]);     \
} while (0)
#define vec3_diff(dst, src1, src2) do {     \
    (dst)[0] = ((src1)[0] - (src2)[0]);     \
    (dst)[1] = ((src1)[1] - (src2)[1]);     \
    (dst)[2] = ((src1)[2] - (src2)[2]);     \
} while (0)
#define vec4_diff(dst, src1, src2) do {     \
    (dst)[0] = ((src1)[0] - (src2)[0]);     \
    (dst)[1] = ((src1)[1] - (src2)[1]);     \
    (dst)[2] = ((src1)[2] - (src2)[2]);     \
    (dst)[3] = ((src1)[3] - (src2)[3]);     \
} while (0)

#define vec2_sub(dst, src) vec2_diff((dst), (dst), (src))
#define vec3_sub(dst, src) vec3_diff((dst), (dst), (src))
#define vec4_sub(dst, src) vec4_diff((dst), (dst), (src))

#define vec2_diff_val(dst, src, x) do {     \
    (dst)[0] = ((src)[0] - (x));            \
    (dst)[1] = ((src)[1] - (x));            \
} while (0)
#define vec3_diff_val(dst, src, x) do {     \
    (dst)[0] = ((src)[0] - (x));            \
    (dst)[1] = ((src)[1] - (x));            \
    (dst)[2] = ((src)[2] - (x));            \
} while (0)
#define vec4_diff_val(dst, src, x) do {     \
    (dst)[0] = ((src)[0] - (x));            \
    (dst)[1] = ((src)[1] - (x));            \
    (dst)[2] = ((src)[2] - (x));            \
    (dst)[3] = ((src)[3] - (x));            \
} while (0)

#define vec2_sub_val(dst, x) vec2_diff_val((dst), (dst), (x))
#define vec3_sub_val(dst, x) vec3_diff_val((dst), (dst), (x))
#define vec4_sub_val(dst, x) vec4_diff_val((dst), (dst), (x))

#define vec2_prod(dst, src1, src2) do {     \
    (dst)[0] = ((src1)[0] * (src2)[0]);     \
    (dst)[1] = ((src1)[1] * (src2)[1]);     \
} while (0)
#define vec3_prod(dst, src1, src2) do {     \
    (dst)[0] = ((src1)[0] * (src2)[0]);     \
    (dst)[1] = ((src1)[1] * (src2)[1]);     \
    (dst)[2] = ((src1)[2] * (src2)[2]);     \
} while (0)
#define vec4_prod(dst, src1, src2) do {     \
    (dst)[0] = ((src1)[0] * (src2)[0]);     \
    (dst)[1] = ((src1)[1] * (src2)[1]);     \
    (dst)[2] = ((src1)[2] * (src2)[2]);     \
    (dst)[3] = ((src1)[3] * (src2)[3]);     \
} while (0)

#define vec2_mul(dst, src) vec2_prod((dst), (dst), (src))
#define vec3_mul(dst, src) vec3_prod((dst), (dst), (src))
#define vec4_mul(dst, src) vec4_prod((dst), (dst), (src))

#define vec2_prod_val(dst, src, x) do {     \
    (dst)[0] = ((src)[0] * (x));            \
    (dst)[1] = ((src)[1] * (x));            \
} while (0)
#define vec3_prod_val(dst, src, x) do {     \
    (dst)[0] = ((src)[0] * (x));            \
    (dst)[1] = ((src)[1] * (x));            \
    (dst)[2] = ((src)[2] * (x));            \
} while (0)
#define vec4_prod_val(dst, src, x) do {     \
    (dst)[0] = ((src)[0] * (x));            \
    (dst)[1] = ((src)[1] * (x));            \
    (dst)[2] = ((src)[2] * (x));            \
    (dst)[3] = ((src)[3] * (x));            \
} while (0)

#define vec2_mul_val(dst, x) vec2_prod_val(dst, dst, x)
#define vec3_mul_val(dst, x) vec3_prod_val(dst, dst, x)
#define vec4_mul_val(dst, x) vec4_prod_val(dst, dst, x)

#define vec2_quot(dst, src1, src2) do {     \
    (dst)[0] = ((src1)[0] / (src2)[0]);     \
    (dst)[1] = ((src1)[1] / (src2)[1]);     \
} while (0)
#define vec3_quot(dst, src1, src2) do {     \
    (dst)[0] = ((src1)[0] / (src2)[0]);     \
    (dst)[1] = ((src1)[1] / (src2)[1]);     \
    (dst)[2] = ((src1)[2] / (src2)[2]);     \
} while (0)
#define vec4_quot(dst, src1, src2) do {     \
    (dst)[0] = ((src1)[0] / (src2)[0]);     \
    (dst)[1] = ((src1)[1] / (src2)[1]);     \
    (dst)[2] = ((src1)[2] / (src2)[2]);     \
    (dst)[3] = ((src1)[3] / (src2)[3]);     \
} while (0)

#define vec2_div(dst, src) vec2_quot((dst), (dst), (src))
#define vec3_div(dst, src) vec3_quot((dst), (dst), (src))
#define vec4_div(dst, src) vec4_quot((dst), (dst), (src))

#define vec2_quot_val(dst, src, x) do {     \
    (dst)[0] = ((src)[0] / (x));            \
    (dst)[1] = ((src)[1] / (x));            \
} while (0)
#define vec3_quot_val(dst, src, x) do {     \
    (dst)[0] = ((src)[0] / (x));            \
    (dst)[1] = ((src)[1] / (x));            \
    (dst)[2] = ((src)[2] / (x));            \
} while (0)
#define vec4_quot_val(dst, src, x) do {     \
    (dst)[0] = ((src)[0] / (x));            \
    (dst)[1] = ((src)[1] / (x));            \
    (dst)[2] = ((src)[2] / (x));            \
    (dst)[3] = ((src)[3] / (x));            \
} while (0)

#define vec2_div_val(dst, x) vec2_quot_val((dst), (dst), (x))
#define vec3_div_val(dst, x) vec3_quot_val((dst), (dst), (x))
#define vec4_div_val(dst, x) vec4_quot_val((dst), (dst), (x))

#define MAT4_VEC_DOT_PROD(R, A, B, row, col) do {           \
    (R)[(row)][(col)]  = ((A)[(row)][0] * (B)[0][(col)]);   \
    (R)[(row)][(col)] += ((A)[(row)][1] * (B)[1][(col)]);   \
    (R)[(row)][(col)] += ((A)[(row)][2] * (B)[2][(col)]);   \
} while (0)
#define MAT4_DOT_PROD(R, A, B, row, col) do {               \
    (R)[(row)][(col)]  = ((A)[(row)][0] * (B)[0][(col)]);   \
    (R)[(row)][(col)] += ((A)[(row)][1] * (B)[1][(col)]);   \
    (R)[(row)][(col)] += ((A)[(row)][2] * (B)[2][(col)]);   \
    (R)[(row)][(col)] += ((A)[(row)][3] * (B)[3][(col)]);   \
} while (0)

#define MAT4_MULTIPLY(R, A, B) do {     \
    MAT4_DOT_PROD((R), (A), (B), 0, 0); \
    MAT4_DOT_PROD((R), (A), (B), 0, 1); \
    MAT4_DOT_PROD((R), (A), (B), 0, 2); \
    MAT4_DOT_PROD((R), (A), (B), 0, 3); \
    MAT4_DOT_PROD((R), (A), (B), 1, 0); \
    MAT4_DOT_PROD((R), (A), (B), 1, 1); \
    MAT4_DOT_PROD((R), (A), (B), 1, 2); \
    MAT4_DOT_PROD((R), (A), (B), 1, 3); \
    MAT4_DOT_PROD((R), (A), (B), 2, 0); \
    MAT4_DOT_PROD((R), (A), (B), 2, 1); \
    MAT4_DOT_PROD((R), (A), (B), 2, 2); \
    MAT4_DOT_PROD((R), (A), (B), 2, 3); \
    MAT4_DOT_PROD((R), (A), (B), 3, 0); \
    MAT4_DOT_PROD((R), (A), (B), 3, 1); \
    MAT4_DOT_PROD((R), (A), (B), 3, 2); \
    MAT4_DOT_PROD((R), (A), (B), 3, 3); \
} while (0)

#define MTXF_END(mtx) do {                      \
    (mtx)[0][3] = (mtx)[1][3] = (mtx)[2][3] = 0;\
    ((u32*)(mtx))[15] = FLOAT_ONE;              \
} while (0)

// Float lerp.
// Return the value between [a, b] based on f's value between [0.0, 1.0].
ALWAYS_INLINE f32 lerpf(f32 a, f32 b, f32 f) {
    return (a + (f * (b - a)));
}

// Precise float lerp.
// Return the value between [a, b] based on f's value between [0.0, 1.0].
// Sloer but more precise than the regular float lerp.
ALWAYS_INLINE f32 lerpf_precise(f32 a, f32 b, f32 f) {
    return ((a * (1.0f - f)) + (b * f));
}

// Integer lerp.
// Return the value between [a, b] based on f's value between [0, 256].
// Faster than float lerps.
ALWAYS_INLINE s32 lerpi(s32 a, s32 b, u32 f) {
    return (((f * (b - a)) >> 8) + a);
}

// Remaps a number from one range to another.
// Return the value between [fromB, toB] based on X's value between [fromA, toA].
// Equivalent to lerp but with a custom range for f.
ALWAYS_INLINE f32 remap(f32 f, f32 fromA, f32 toA, f32 fromB, f32 toB) {
    return ((((f - fromA) / (toA - fromA)) * (toB - fromB)) + fromB);
}

// Inline asm functions:

/// Load upper immediate.
ALWAYS_INLINE void lui(u32* dest, u32 val) {
    __asm__("lui %0, %1" : "=r"(*dest) : "K"(val));
}

/// OR immediate.
ALWAYS_INLINE void ori_self(u32* dest, u32 val) {
    __asm__("ori %0, $0, %1" : "+r"(*dest) : "K"(val));
}

/// Multiply two floats without a nop.
ALWAYS_INLINE float mul_without_nop(float a, float b) {
    float ret;
    __asm__("mul.s %0, %1, %2" : "=f"(ret) : "f"(a), "f"(b));
    return ret;
}

/// Write a 32 bit value into the low order bytes of a register.
ALWAYS_INLINE void swr(void* addr, int val, const int offset) {
    __asm__("swr %1, %2(%0)" : : "g"(addr), "g"(val), "I"(offset));
}

/// Write a 32 bit value into the high order bytes of a register.
ALWAYS_INLINE void swl(void* addr, int val, const int offset) {
    __asm__("swl %1, %2(%0)" : : "g"(addr), "g"(val), "I"(offset));
}

/// Convert a float to an int.
ALWAYS_INLINE int mfc1(float in) {
    int out;
    __asm__("mfc1 %0,%1" : "=r" (out) : "f" (in));
    return out;
}

/// Convert an int to a float.
ALWAYS_INLINE float mtc1(int in) {
    float out;
    __asm__("mtc1 %1, %0" : "=f"(out) : "r"(in));
    return out;
}

/// Rounds towards infinity
ALWAYS_INLINE s32 ceilf(const f32 in) {
    f32 tmp;
    __asm__("ceil.w.s %0,%1" : "=f" (tmp) : "f" (in));
    return mfc1(tmp);
}
/// Rounds towards negative infinity
ALWAYS_INLINE s32 floorf(const f32 in) {
    f32 tmp;
    __asm__("floor.w.s %0,%1" : "=f" (tmp) : "f" (in));
    return mfc1(tmp);
}
/// Rounds towards the nearest integer
extern s32 roundf(f32);
// ALWAYS_INLINE s32 roundf(const f32 in) {
//     f32 tmp;
//     __asm__("round.w.s %0,%1" : "=f" (tmp) : "f" (in));
//     return mfc1(tmp);
// }

/// Backwards compatibility
#define round_float(in) roundf(in)


//! TODO: Is ABS macro better for these?:
/// Absolute value of a float value
ALWAYS_INLINE f32 absf(const f32 in) {
    f32 out;
    __asm__("abs.s %0,%1" : "=f" (out) : "f" (in)); //? __builtin_fabsf(x)
    return out;
}
/// Absolute value of a double value
ALWAYS_INLINE f64 absd(const f64 in) {
    f64 out;
    __asm__("abs.d %0,%1" : "=f" (out) : "f" (in));
    return out;
}
/// Absolute value of an integer value
ALWAYS_INLINE s32 absi(const s32 in) {
    s32 t0 = (in >> 31);
    return ((in ^ t0) - t0);
}
/// Absolute value of a short value
ALWAYS_INLINE s32 abss(const s16 in) {
    s32 t0 = (in >> 31);
    return ((in ^ t0) - t0);
}
/// Backwards compatibility
#define absf_2 absf
#define ABS2   ABS


/// Constructs a float in registers, which can be faster than gcc's default of loading a float from rodata.
/// Especially fast for halfword floats, which get loaded with a `lui` + `mtc1`.
ALWAYS_INLINE float construct_float(const float f) {
    u32 r;
    u32 i = *(u32*)(&f);

    if (!__builtin_constant_p(i)) {
        return *(float*)(&i);
    }

    u32 upper = (i >> 16);
    u32 lower = (i & BITMASK(16));

    if ((i & BITMASK(16)) == 0) {
        lui(&r, upper);
    } else if ((i & (BITMASK(16) << 16)) == 0) {
        ori_self(&r, lower);
    } else {
        lui(&r, upper);
        ori_self(&r, lower);
    }

    return mtc1(r);
}


// On console, (x != 0) still returns true for denormalized floats,
// which will count as a division by zero when divided and crash.
// For console compatibility, use this check instead when avoiding a division by zero.
#define F32_IS_NONZERO(x) ((*(int*)(&(x))) & (BITMASK( 8) << 23))
#define F64_IS_NONZERO(x) ((*(int*)(&(x))) & (BITMASK(11) << 52))
// HackerSM64: Backwards compatibility
#define FLT_IS_NONZERO(x) F32_IS_NONZERO(x)
#define DBL_IS_NONZERO(x) F64_IS_NONZERO(x)

// RNG
u16 random_u16(void);
f32 random_float(void);
s32 random_sign(void);

f32 get_cycle(f32 cycleLength, f32 cycleOffset, u32 timer);

// Min/Max
f32  min_3f(    f32 a, f32 b, f32 c);
s32  min_3i(    s32 a, s32 b, s32 c);
s32  min_3s(    s16 a, s16 b, s16 c);
f32  max_3f(    f32 a, f32 b, f32 c);
s32  max_3i(    s32 a, s32 b, s32 c);
s32  max_3s(    s16 a, s16 b, s16 c);
void min_max_3f(f32 a, f32 b, f32 c, f32* min, f32* max);
void min_max_3i(s32 a, s32 b, s32 c, s32* min, s32* max);
void min_max_3s(s16 a, s16 b, s16 c, s16* min, s16* max);
// Vector comparison
Bool32 vec3f_compare_f32(Vec3f vec, f32 x, f32 y, f32 z);
Bool32 vec3f_compare(Vec3f a, Vec3f b);
// Vector copy
void vec3f_copy    (Vec3f dest, const Vec3f src);
void vec3i_copy    (Vec3i dest, const Vec3i src);
void vec3s_copy    (Vec3s dest, const Vec3s src);
void vec3s_to_vec3i(Vec3i dest, const Vec3s src);
void vec3s_to_vec3f(Vec3f dest, const Vec3s src);
void vec3i_to_vec3s(Vec3s dest, const Vec3i src);
void vec3i_to_vec3f(Vec3f dest, const Vec3i src);
void vec3f_to_vec3s(Vec3s dest, const Vec3f src);
void vec3f_to_vec3i(Vec3i dest, const Vec3f src);
// Special vector copy
void vec3f_copy_y_off(Vec3f dest, Vec3f src, f32 yOff);
void surface_normal_to_vec3f(Vec3f dest, struct Surface* surf);
// Vector set
void vec3f_set(Vec3f dest, const f32 x, const f32 y, const f32 z);
void vec3i_set(Vec3i dest, const s32 x, const s32 y, const s32 z);
void vec3s_set(Vec3s dest, const s16 x, const s16 y, const s16 z);
// Vector arithmetic
void vec3f_add (Vec3f dest, const Vec3f a               );
void vec3i_add (Vec3i dest, const Vec3i a               );
void vec3s_add (Vec3s dest, const Vec3s a               );
void vec3f_sum (Vec3f dest, const Vec3f a, const Vec3f b);
void vec3i_sum (Vec3i dest, const Vec3i a, const Vec3i b);
void vec3s_sum (Vec3s dest, const Vec3s a, const Vec3s b);
void vec3f_sub (Vec3f dest, const Vec3f a               );
void vec3i_sub (Vec3i dest, const Vec3i a               );
void vec3s_sub (Vec3s dest, const Vec3s a               );
void vec3f_diff(Vec3f dest, const Vec3f a, const Vec3f b);
void vec3i_diff(Vec3i dest, const Vec3i a, const Vec3i b);
void vec3s_diff(Vec3s dest, const Vec3s a, const Vec3s b);
void vec3f_mul (Vec3f dest, const Vec3f a               );
void vec3i_mul (Vec3i dest, const Vec3i a               );
void vec3s_mul (Vec3s dest, const Vec3s a               );
void vec3f_prod(Vec3f dest, const Vec3f a, const Vec3f b);
void vec3i_prod(Vec3i dest, const Vec3i a, const Vec3i b);
void vec3s_prod(Vec3s dest, const Vec3s a, const Vec3s b);
void vec3f_div (Vec3f dest, const Vec3f a               );
void vec3i_div (Vec3i dest, const Vec3i a               );
void vec3s_div (Vec3s dest, const Vec3s a               );
void vec3f_quot(Vec3f dest, const Vec3f a, const Vec3f b);
void vec3i_quot(Vec3i dest, const Vec3i a, const Vec3i b);
void vec3s_quot(Vec3s dest, const Vec3s a, const Vec3s b);
// Vector operations
f32  vec3f_dot(              const Vec3f a, const Vec3f b);
void vec3f_cross(Vec3f dest, const Vec3f a, const Vec3f b);
Bool32 vec3f_normalize_check(Vec3f dest);
Bool32 vec3f_normalize(Vec3f dest);
Bool32 vec3f_normalize_max(Vec3f dest, f32 max);
// Mtxf operations
void mtxf_copy(Mat4 dest, Mat4 src);
// Create specific matrices
void mtxf_identity(Mat4 mtx);
void mtxf_lookat(Mat4 mtx, Vec3f from, Vec3f to, s16 roll);
void mtxf_held_object(Mat4 dest, Mat4 src, Mat4 throwMatrix, Vec3f translation, Vec3f scale);
void mtxf_billboard(Mat4 dest, Mat4 mtx, Vec3f position, Vec3f scale, s16 roll);
void mtxf_shadow(Mat4 dest, Vec3f upDir, Vec3f pos, Vec3f scale, s16 yaw);
void mtxf_align_terrain_normal(Mat4 dest, Vec3f normal, Vec3f pos, s16 yaw);
void mtxf_align_terrain_triangle(Mat4 mtx, Vec3f pos, s16 yaw, f32 radius);
void create_transformation_from_matrices(Mat4 dst, Mat4 a1, Mat4 a2);
// Translation & rotation
void mtx_rotate_xy(Mtx* mtx, s16 angle);
void mtxf_translate(Mat4 dest, Vec3f b);
void mtxf_rotate_zxy_and_translate(Mat4 dest, Vec3f trans, Vec3s rot);
void mtxf_rotate_xyz_and_translate(Mat4 dest, Vec3f trans, Vec3s rot);
void mtxf_rotate_zxy_and_translate_and_mul(Vec3s rot, Vec3f trans, Mat4 dest, Mat4 src);
void mtxf_rotate_xyz_and_translate_and_mul(Vec3s rot, Vec3f trans, Mat4 dest, Mat4 src);
void mtxf_translate_local_vec3f(Mat4 mtx, Vec3f dest, Vec3f src);
// Matrix multiplication
void mtxf_mul(Mat4 dest, Mat4 a, Mat4 b);
void mtxf_scale_vec3f(Mat4 dest, Mat4 mtx, Vec3f s);
void mtxf_mul_vec3s(Mat4 mtx, Vec3s b);
void linear_mtxf_mul_vec3f(Mat4 mtx, Vec3f dest, Vec3f src);
void linear_mtxf_mul_vec3f_and_translate(Mat4 mtx, Vec3f dest, Vec3f src);
void linear_mtxf_transpose_mul_vec3f(Mat4 mtx, Vec3f dest, Vec3f src);
// Fixed point matrix conversions:
f32 mtx_get_float(Mtx* mtx, u32 xIndex, u32 yIndex);
void mtx_set_float(Mtx* mtx, f32 val, u32 xIndex, u32 yIndex);

extern void mtxf_to_mtx_fast(s16* dest, float* src);
ALWAYS_INLINE void mtxf_to_mtx(void* dest, void* src) {
    mtxf_to_mtx_fast((s16*)dest, (float*)src);
    // guMtxF2L(src, dest);
}

// Local/World pos conversions
void vec3f_local_pos_to_world_pos(Vec3f destWorldPos, Vec3f srcLocalPos, Vec3f originPos, Vec3s rotation);
void vec3f_world_pos_to_local_pos(Vec3f destLocalPos, Vec3f srcWorldPos, Vec3f originPos, Vec3s rotation);

// Vector get/set functions
void vec2f_get_lateral_dist(                   Vec2f from, Vec2f to,            f32* lateralDist                      );
void vec3f_get_lateral_dist(                   Vec3f from, Vec3f to,            f32* lateralDist                      );
void vec3f_get_lateral_dist_squared(           Vec3f from, Vec3f to,            f32* lateralDist                      );
void vec3f_get_dist(                           Vec3f from, Vec3f to, f32* dist                                        );
void vec3f_get_dist_squared(                   Vec3f from, Vec3f to, f32* dist                                        );
void vec3f_get_dist_and_yaw(                   Vec3f from, Vec3f to, f32* dist,                               s16* yaw);
void vec3f_get_pitch(                          Vec3f from, Vec3f to,                              s16* pitch          );
void vec3f_get_yaw(                            Vec3f from, Vec3f to,                                          s16* yaw);
void vec3f_get_angle(                          Vec3f from, Vec3f to,                              s16* pitch, s16* yaw);
void vec3f_get_lateral_dist_and_pitch(         Vec3f from, Vec3f to,            f32* lateralDist, s16* pitch          );
void vec3f_get_lateral_dist_and_yaw(           Vec3f from, Vec3f to,            f32* lateralDist,             s16* yaw);
void vec3f_get_lateral_dist_and_angle(         Vec3f from, Vec3f to,            f32* lateralDist, s16* pitch, s16* yaw);
void vec3f_get_dist_and_lateral_dist_and_angle(Vec3f from, Vec3f to, f32* dist, f32* lateralDist, s16* pitch, s16* yaw);
void vec3s_get_dist_and_angle(                 Vec3s from, Vec3s to, s16* dist,                   s16* pitch, s16* yaw);
void vec3f_get_dist_and_angle(                 Vec3f from, Vec3f to, f32* dist,                   s16* pitch, s16* yaw);
void vec3f_to_vec3s_get_dist_and_angle(        Vec3f from, Vec3s to, f32* dist,                   s16* pitch, s16* yaw);
void vec3s_set_dist_and_angle(                 Vec3s from, Vec3s to, s16  dist,                   s16  pitch, s16  yaw);
void vec3f_set_dist_and_angle(                 Vec3f from, Vec3f to, f32  dist,                   s16  pitch, s16  yaw);

// Approach value functions
s16 approach_angle(s16 current, s16 target, s16 inc);
s16 approach_s16(s16 current, s16 target, s16 inc, s16 dec);
s32 approach_s32(s32 current, s32 target, s32 inc, s32 dec);
f32 approach_f32(f32 current, f32 target, f32 inc, f32 dec);
Bool32 approach_angle_bool(s16* current, s16 target, s16 inc);
Bool32 approach_s16_bool(s16* current, s16 target, s16 inc, s16 dec);
Bool32 approach_s32_bool(s32* current, s32 target, s32 inc, s32 dec);
Bool32 approach_f32_bool(f32* current, f32 target, f32 inc, f32 dec);
#define approach_s16_symmetric(current, target, inc) approach_s16((current), (target), (inc), (inc))
#define approach_s32_symmetric(current, target, inc) approach_s32((current), (target), (inc), (inc))
#define approach_f32_symmetric(current, target, inc) approach_f32((current), (target), (inc), (inc))
#define approach_s16_symmetric_bool(current, target, inc) approach_s16_bool((current), (target), (inc), (inc))
#define approach_s32_symmetric_bool(current, target, inc) approach_s32_bool((current), (target), (inc), (inc))
#define approach_f32_symmetric_bool(current, target, inc) approach_f32_bool((current), (target), (inc), (inc))
Bool32 approach_f32_signed(f32* current, f32 target, f32 inc);
Bool32 approach_f32_asymptotic_bool(f32* current, f32 target, f32 multiplier);
f32    approach_f32_asymptotic(f32 current, f32 target, f32 multiplier);
Bool32 approach_s16_asymptotic_bool(s16* current, s16 target, s16 divisor);
s16    approach_s16_asymptotic(s16 current, s16 target, s16 divisor);
// Angles
s16 abs_angle_diff(s16 a0, s16 a1);
s16 atan2s(f32 y, f32 x);
f32 atan2f(f32 a, f32 b);
// Splines
void evaluate_cubic_spline(f32 progress, Vec3f pos, Vec3f spline1, Vec3f spline2, Vec3f spline3, Vec3f spline4);
void spline_get_weights(Vec4f result, f32 t, UNUSED s32 c);
void anim_spline_init(Vec4s* keyFrames);
s32  anim_spline_poll(Vec3f result);
// Raycasting
f32 find_surface_on_ray(Vec3f orig, Vec3f dir, struct Surface** hit_surface, Vec3f hit_pos, s32 flags);
f32 find_surface_on_ray_between_points(Vec3f pos1, Vec3f pos2, struct Surface** hit_surface, Vec3f hit_pos, s32 flags);

#endif // MATH_UTIL_H
