// Functions for editing colors & textures (Arceveti)

#include <PR/ultratypes.h>

#include "math_util.h"
#include "colors.h"

// RGBA16

/**
 * A fast lerp function between two RGBA16 colors.
 */
RGBA16 rgba16_blend(RGBA16 a, RGBA16 b, Alpha fac) {
    RGBA16 ds, d = MSK_RGBA16_A;
    RGBA16 s = (MSK_RGBA16_C << SIZ_RGBA16_A);
    for (s32 i = 0; i < 3; i++) {
        ds = (a & s);
        d |= ((((fac * ((b & s) - ds)) >> 8) + ds) & s);
        s <<= SIZ_RGBA16_C;
    }

    return d;
}

// ColorRGB

void rgba16_to_colorRGB(ColorRGB dst, RGBA16 src) {
    dst[0] = COMPOSITE_TO_COLOR(src, MSK_RGBA16_C, IDX_RGBA16_R);
    dst[1] = COMPOSITE_TO_COLOR(src, MSK_RGBA16_C, IDX_RGBA16_G);
    dst[2] = COMPOSITE_TO_COLOR(src, MSK_RGBA16_C, IDX_RGBA16_B);
}

void rgba16_to_colorRGBA(ColorRGBA dst, RGBA16 src) {
    rgba16_to_colorRGB(dst, src);
    dst[3] = COMPOSITE_TO_COLOR(src, MSK_RGBA16_A, IDX_RGBA16_A);
}

void rgba32_to_colorRGB(ColorRGBA dst, RGBA32 src) {
    dst[0] = COMPOSITE_TO_COLOR(src, MSK_RGBA32_C, IDX_RGBA32_R);
    dst[1] = COMPOSITE_TO_COLOR(src, MSK_RGBA32_C, IDX_RGBA32_G);
    dst[2] = COMPOSITE_TO_COLOR(src, MSK_RGBA32_C, IDX_RGBA32_B);
}

void rgba32_to_colorRGBA(ColorRGBA dst, RGBA32 src) {
    rgba32_to_colorRGB(dst, src);
    dst[3] = COMPOSITE_TO_COLOR(src, MSK_RGBA32_A, IDX_RGBA32_A);
}

RGBA16Return32 colorRGB_to_rgba16(ColorRGB src) {
    return (COLOR_TO_COMPOSITE(src[0], MSK_RGBA16_C, IDX_RGBA16_R)
          | COLOR_TO_COMPOSITE(src[1], MSK_RGBA16_C, IDX_RGBA16_G)
          | COLOR_TO_COMPOSITE(src[2], MSK_RGBA16_C, IDX_RGBA16_B)
          | MSK_RGBA16_A);
}

RGBA16Return32 colorRGBA_to_rgba16(ColorRGBA src) {
    return (COLOR_TO_COMPOSITE(src[0], MSK_RGBA16_C, IDX_RGBA16_R)
          | COLOR_TO_COMPOSITE(src[1], MSK_RGBA16_C, IDX_RGBA16_G)
          | COLOR_TO_COMPOSITE(src[2], MSK_RGBA16_C, IDX_RGBA16_B)
          | COLOR_TO_COMPOSITE(src[2], MSK_RGBA16_A, IDX_RGBA16_A));
}

RGBA32 colorRGB_to_rgba32(ColorRGB src) {
    return (COLOR_TO_COMPOSITE(src[0], MSK_RGBA32_C, IDX_RGBA32_R)
          | COLOR_TO_COMPOSITE(src[1], MSK_RGBA32_C, IDX_RGBA32_G)
          | COLOR_TO_COMPOSITE(src[2], MSK_RGBA32_C, IDX_RGBA32_B)
          | MSK_RGBA32_A);
}

RGBA32 colorRGBA_to_rgba32(ColorRGBA src) {
    return (COLOR_TO_COMPOSITE(src[0], MSK_RGBA32_C, IDX_RGBA32_R)
          | COLOR_TO_COMPOSITE(src[1], MSK_RGBA32_C, IDX_RGBA32_G)
          | COLOR_TO_COMPOSITE(src[2], MSK_RGBA32_C, IDX_RGBA32_B)
          | COLOR_TO_COMPOSITE(src[2], MSK_RGBA32_A, IDX_RGBA32_A));
}

// ColorRGBf

void rgba16_to_colorRGBf(ColorRGBf dst, RGBA16 src) {
    dst[0] = COMPOSITE_TO_COLORF(src, MSK_RGBA16_C, IDX_RGBA16_R);
    dst[1] = COMPOSITE_TO_COLORF(src, MSK_RGBA16_C, IDX_RGBA16_G);
    dst[2] = COMPOSITE_TO_COLORF(src, MSK_RGBA16_C, IDX_RGBA16_B);
}

void rgba16_to_colorRGBAf(ColorRGBAf dst, RGBA16 src) {
    rgba16_to_colorRGBf(dst, src);
    dst[3] = COMPOSITE_TO_COLORF(src, MSK_RGBA16_A, IDX_RGBA16_A);
}

void rgba32_to_colorRGBf(ColorRGBf dst, RGBA32 src) {
    dst[0] = COMPOSITE_TO_COLORF(src, MSK_RGBA32_C, IDX_RGBA32_R);
    dst[1] = COMPOSITE_TO_COLORF(src, MSK_RGBA32_C, IDX_RGBA32_G);
    dst[2] = COMPOSITE_TO_COLORF(src, MSK_RGBA32_C, IDX_RGBA32_B);
}

void rgba32_to_colorRGBAf(ColorRGBAf dst, RGBA32 src) {
    rgba32_to_colorRGBf(dst, src);
    dst[3] = COMPOSITE_TO_COLORF(src, MSK_RGBA32_A, IDX_RGBA32_A);
}

void colorRGB_to_colorRGBf(ColorRGBf dst, ColorRGB src) { vec3_quot_val(dst, src, 255.0f); }
void colorRGBf_to_colorRGB(ColorRGB dst, ColorRGBf src) { vec3_prod_val(dst, src, 255.0f); }

RGBA16Return32 colorRGBf_to_rgba16(ColorRGBf src) {
    return (COLORF_TO_COMPOSITE(src[0], MSK_RGBA16_C, IDX_RGBA16_R)
          | COLORF_TO_COMPOSITE(src[1], MSK_RGBA16_C, IDX_RGBA16_G)
          | COLORF_TO_COMPOSITE(src[2], MSK_RGBA16_C, IDX_RGBA16_B)
          | MSK_RGBA16_A);
}

RGBA16Return32 colorRGBAf_to_rgba16(ColorRGBAf src) {
    return (COLORF_TO_COMPOSITE(src[0], MSK_RGBA16_C, IDX_RGBA16_R)
          | COLORF_TO_COMPOSITE(src[1], MSK_RGBA16_C, IDX_RGBA16_G)
          | COLORF_TO_COMPOSITE(src[2], MSK_RGBA16_C, IDX_RGBA16_B)
          | COLORF_TO_COMPOSITE(src[2], MSK_RGBA16_A, IDX_RGBA16_A));
}

RGBA32 colorRGBf_to_rgba32(ColorRGBf src) {
    return (COLORF_TO_COMPOSITE(src[0], MSK_RGBA32_C, IDX_RGBA32_R)
          | COLORF_TO_COMPOSITE(src[1], MSK_RGBA32_C, IDX_RGBA32_G)
          | COLORF_TO_COMPOSITE(src[2], MSK_RGBA32_C, IDX_RGBA32_B)
          | MSK_RGBA32_A);
}

RGBA32 colorRGBAf_to_rgba32(ColorRGBAf src) {
    return (COLORF_TO_COMPOSITE(src[0], MSK_RGBA32_C, IDX_RGBA32_R)
          | COLORF_TO_COMPOSITE(src[1], MSK_RGBA32_C, IDX_RGBA32_G)
          | COLORF_TO_COMPOSITE(src[2], MSK_RGBA32_C, IDX_RGBA32_B)
          | COLORF_TO_COMPOSITE(src[2], MSK_RGBA32_A, IDX_RGBA32_A));
}

Bool32 colorRGBA_average_2(ColorRGBA dst, ColorRGBA c1, ColorRGBA c2) {
    dst[3] = (c1[3] + c2[3]);

    if (dst[3] > 0) {
        for (s32 i = 0; i < 3; i++) {
            dst[i] = (((c1[i] * c1[3]) + (c2[i] * c2[3])) / dst[3]);
        }

        return TRUE;
    }

    return FALSE;
}

Bool32 colorRGBA_average_3(ColorRGBA dst, ColorRGBA c1, ColorRGBA c2, ColorRGBA c3) {
    dst[3] = (c1[3] + c2[3] + c3[3]);

    if (dst[3] > 0) {
        for (s32 i = 0; i < 3; i++) {
            dst[i] = (((c1[i] * c1[3]) + (c2[i] * c2[3]) + (c3[i] * c3[3])) / dst[3]);
        }

        return TRUE;
    }

    return FALSE;
}

RGBA16Return32 rgba16_make_grayscale(RGBA16 rgba) {
    ColorRGBf color;
    rgba16_to_colorRGBf(color, rgba);
    ColorF avg = vec3_average(color);
    vec3_same(color, avg);
    return colorRGBf_to_rgba16(color);
}

void colorRGB_add_hue(ColorRGB color, Color hueAdd, Color s) {
    f32 hue = 0.0f;
    if ((color[0] < color[1]) || (color[0] < color[2])) {
        if (color[1] < color[2]) {
            hue = (4.0f + (color[0] - color[1]) / (f32)(color[2] - MIN(color[0], color[1]))); // blue
        } else {
            hue = (2.0f + (color[2] - color[0]) / (f32)(color[1] - MIN(color[0], color[2]))); // green
        }
    } else {
        hue = (0.0f + (color[1] - color[2]) / (f32)(color[0] - MIN(color[1], color[2]))); // red
    }
    if (hue < 0.0f) hue += 6.0f;
    // this is the algorithm to convert from RGB to HSV:
    Color h  = (((u8)((hue * (128.0f / 3.0f)) + hueAdd) >> 2) * 3); // needs to u8 cycle before multiplying. 0..191
    Color i  =  (h >> 5);                                           // 0..5
    Color f  = ((h & BITMASK(5)) << 3);                             // 'fractional' part of 'i' 0..248 in jumps
    Color pv = (0xFF -   s                    );                    // pv will be in range 0..255
    Color qv = (0xFF - ((s *         f ) >> 8));
    Color tv = (0xFF - ((s * (0xFF - f)) >> 8));
    switch (i) {
        case 0: color[0] = 0xFF; color[1] =   tv; color[2] =   pv; break;
        case 1: color[0] =   qv; color[1] = 0xFF; color[2] =   pv; break;
        case 2: color[0] =   pv; color[1] = 0xFF; color[2] =   tv; break;
        case 3: color[0] =   pv; color[1] =   qv; color[2] = 0xFF; break;
        case 4: color[0] =   tv; color[1] =   pv; color[2] = 0xFF; break;
        case 5: color[0] = 0xFF; color[1] =   pv; color[2] =   qv; break;
    }
}

// texrect_rgba16

const Gfx dl_texrect_rgba16_begin[] = {
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_COPY),
    gsDPSetTexturePersp(G_TP_NONE),
    gsDPSetTextureFilter(G_TF_POINT),
    gsDPSetRenderMode(G_RM_NOOP, G_RM_NOOP2),
    gsDPSetAlphaCompare(G_AC_THRESHOLD),
    gsSPEndDisplayList(),
};

const Gfx dl_texrect_rgba16_end[] = {
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_1CYCLE),
    gsDPSetTexturePersp(G_TP_PERSP),
    gsDPSetTextureFilter(G_TF_BILERP),
    gsDPSetRenderMode(G_RM_AA_ZB_OPA_SURF, G_RM_AA_ZB_OPA_SURF2),
    gsDPSetAlphaCompare(G_AC_NONE),
    gsSPEndDisplayList(),
};

// Must be used with the above display lists.
void texrect_rgba16(Gfx** dlIter, Texture* texture, s16 texW, s16 texH, s16 x, s16 y, s16 w, s16 h) {
    gDPLoadTextureTile((*dlIter)++,
        texture,
        G_IM_FMT_RGBA, G_IM_SIZ_16b,
        texW, texH,
        0, 0,
        (texW - 1), (texH - 1),
        0,
        (G_TX_NOMIRROR | G_TX_CLAMP),
        (G_TX_NOMIRROR | G_TX_CLAMP),
        G_TX_NOMASK, G_TX_NOMASK,
        G_TX_NOLOD,  G_TX_NOLOD
    );
    gSPTextureRectangle((*dlIter)++,
        (x << 2), (y << 2),
        (((x + w) - 1) << 2),
        (((y + h) - 1) << 2),
        G_TX_RENDERTILE,
        0, 0,
        (4 << 10), (1 << 10)
    );
}
