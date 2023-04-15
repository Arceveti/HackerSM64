// This file is a modification of a file from https://github.com/danbolt/n64-jam-1, which was licensed under the MPL-2.0 License
// See the original repo for more details.

#include <ultra64.h>
#include "config.h"
#include "macros.h"
#include "types.h"

#define FONT_IMG_CHAR_WIDTH          8
#define FONT_IMG_CHAR_HEIGHT        12
#define FONT_CHAR_SPACING_WIDTH     FONT_IMG_CHAR_WIDTH
#define FONT_CHAR_SPACING_HEIGHT    FONT_IMG_CHAR_HEIGHT
#define FONT_IMG_NUM_CHARS          84
#define FONT_IMG_WIDTH              (FONT_IMG_CHAR_WIDTH * FONT_IMG_NUM_CHARS) // 672
#define FONT_IMG_HEIGHT             FONT_IMG_CHAR_HEIGHT

#define TEX_ASCII_START '!'
#define TAB_WIDTH 16

#define G_CC_TEXT PRIMITIVE, 0, TEXEL0, 0, 0, 0, 0, TEXEL0

INCBIN(Texture, fast_font, "src/game/newfont2_swapped.bin", 16);

// Sets of characters to skip (in order):
#define SKIP1 "<=>"
#define SKIP2 "@"
#define SKIP3 "[\\]"
#define SKIP4 "_`"
#define SKIP5 "{|}"

static const char skip1[] = SKIP1;
static const char skip2[] = SKIP1 SKIP2;
static const char skip3[] = SKIP1 SKIP2 SKIP3;
static const char skip4[] = SKIP1 SKIP2 SKIP3 SKIP4;
static const char skip5[] = SKIP1 SKIP2 SKIP3 SKIP4 SKIP5;

int computeS(unsigned char letter) {
    int idx = letter;

    if (letter > STR_LAST_CHAR(skip5)) {
        idx -= STRLEN(skip5);
    } else if (letter > STR_LAST_CHAR(skip4)) {
        idx -= STRLEN(skip4);
    } else if (letter > STR_LAST_CHAR(skip3)) {
        idx -= STRLEN(skip3);
    } else if (letter > STR_LAST_CHAR(skip2)) {
        idx -= STRLEN(skip2);
    } else if (letter > STR_LAST_CHAR(skip1)) {
        idx -= STRLEN(skip1);
    }

    return ((idx - TEX_ASCII_START) * FONT_IMG_CHAR_WIDTH);
}

static const u8 fast_text_font_kerning[] = {
    /* */ 2, /*!*/ 3, /*"*/ 5, /*#*/ 7, /*$*/ 6, /*%*/ 6, /*&*/ 7, /*'*/ 2, /*(*/ 5, /*)*/ 5, /***/ 4, /*+*/ 7, /*,*/ 3, /*-*/ 7, /*.*/ 3, /*/*/ 7,
    /*0*/ 7, /*1*/ 7, /*2*/ 7, /*3*/ 7, /*4*/ 7, /*5*/ 7, /*6*/ 7, /*7*/ 7, /*8*/ 7, /*9*/ 7, /*:*/ 3, /*;*/ 3, /*<*/ 0, /*=*/ 0, /*>*/ 0, /*?*/ 7,
    /*@*/ 0, /*A*/ 7, /*B*/ 7, /*C*/ 7, /*D*/ 7, /*E*/ 7, /*F*/ 7, /*G*/ 7, /*H*/ 7, /*I*/ 6, /*J*/ 7, /*K*/ 7, /*L*/ 7, /*M*/ 7, /*N*/ 7, /*O*/ 7,
    /*P*/ 7, /*Q*/ 7, /*R*/ 7, /*S*/ 7, /*T*/ 7, /*U*/ 7, /*V*/ 7, /*W*/ 7, /*X*/ 7, /*Y*/ 7, /*Z*/ 7, /*[*/ 0, /*\*/ 0, /*]*/ 0, /*^*/ 6, /*_*/ 0,
    /*`*/ 0, /*a*/ 6, /*b*/ 6, /*c*/ 6, /*d*/ 6, /*e*/ 6, /*f*/ 6, /*g*/ 6, /*h*/ 6, /*i*/ 2, /*j*/ 6, /*k*/ 5, /*l*/ 3, /*m*/ 6, /*n*/ 6, /*o*/ 6,
    /*p*/ 6, /*q*/ 6, /*r*/ 6, /*s*/ 6, /*t*/ 6, /*u*/ 6, /*v*/ 6, /*w*/ 6, /*x*/ 6, /*y*/ 6, /*z*/ 6, /*{*/ 0, /*|*/ 0, /*}*/ 0, /*~*/ 7,
};

void drawSmallString_impl(Gfx** dl, const int x, const int y, const char* string, const int r, const int g, const int b) {
    Gfx* dlHead = *dl;

    int xPos = x;
    int yPos = y;

    gDPLoadTextureBlock_4bS(dlHead++,
        fast_font, G_IM_FMT_IA,
        FONT_IMG_WIDTH, FONT_IMG_HEIGHT,
        0,
        (G_TX_MIRROR | G_TX_WRAP), (G_TX_MIRROR | G_TX_WRAP),
        G_TX_NOMASK, G_TX_NOMASK,  G_TX_NOLOD, G_TX_NOLOD
    );
    gDPSetPrimColor(dlHead++, 0, 0, r, g, b, 255);
    gDPPipeSync(dlHead++);

    for (int i = 0; string[i] != '\0'; i++) {
        unsigned char cur_char = string[i];

        if (cur_char == '\n') { // New line.
            xPos = x;
            yPos += FONT_CHAR_SPACING_HEIGHT;
        } else if (cur_char == '\t') { // Tab.
            int xDist = (xPos - x) + 1;
            int tabCount = ((xDist + TAB_WIDTH) - 1) / TAB_WIDTH;
            xPos = (tabCount * TAB_WIDTH) + x;
        } else { // Character.
            if (cur_char != ' ') { // Non-space.
                int s = computeS(cur_char);
                gSPTextureRectangle(dlHead++,
                    ((xPos + 0                  ) << 2), ((yPos + 0                   ) << 2),
                    ((xPos + FONT_IMG_CHAR_WIDTH) << 2), ((yPos + FONT_IMG_CHAR_HEIGHT) << 2),
                    G_TX_RENDERTILE,
                    ((s << 5) + (s == (256 >> 5))), 0, // Hack to fix a rendering bug.
                    (1 << 10), (1 << 10)
                );
            }
            xPos += fast_text_font_kerning[cur_char - ' '] + (FONT_CHAR_SPACING_WIDTH - FONT_IMG_CHAR_WIDTH);
        }
    }

    gDPSetPrimColor(dlHead++, 0, 0, 255, 255, 255, 255);
    gDPPipeSync(dlHead++);

    *dl = dlHead;
}

const Gfx dl_fasttext_begin[] = {
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_1CYCLE),
    gsDPSetRenderMode(G_RM_TEX_EDGE, G_RM_TEX_EDGE2),
    gsDPSetTexturePersp(G_TP_NONE),
    gsDPSetTextureFilter(G_TF_POINT),
    gsDPSetTextureLUT(G_TT_NONE),
    gsDPSetCombineMode(G_CC_TEXT, G_CC_TEXT),
    gsSPEndDisplayList(),
};

const Gfx dl_fasttext_end[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsDPSetRenderMode(G_RM_AA_ZB_OPA_SURF, G_RM_AA_ZB_OPA_SURF2),
    gsDPSetTexturePersp(G_TP_PERSP),
    gsDPSetTextureFilter(G_TF_BILERP),
    gsSPEndDisplayList(),
};
