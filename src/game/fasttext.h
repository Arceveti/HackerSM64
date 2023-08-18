#ifndef __FASTTEXT_H__
#define __FASTTEXT_H__

extern const Gfx dl_fasttext_begin[];
extern const Gfx dl_fasttext_end[];

int fasttext_get_str_width(const char* string);
void drawSmallString_impl(Gfx** dl, const int x, const int y, const char* string, const int r, const int g, const int b);

static inline void drawSmallString(Gfx** dl, const int x, const int y, const char* string) {
    drawSmallString_impl(dl, x, y, string, 255, 255, 255);
}

static inline void drawSmallStringCol(Gfx** dl, const int x, const int y, const char* string, int r, const int g, const int b) {
    drawSmallString_impl(dl, x, y, string, r, g, b);
}

static inline void drawSmallStringDL(const int x, const int y, const char* string) {
    Gfx* dlHead = gDisplayListHead;
    gSPDisplayList(dlHead++, dl_fasttext_begin);
    drawSmallString(&dlHead, x, y, string);
    gSPDisplayList(dlHead++, dl_fasttext_end);
    gDisplayListHead = dlHead;
}

static inline void drawSmallStringColDL(const int x, const int y, const char* string, const int r, const int g, const int b) {
    Gfx* dlHead = gDisplayListHead;
    gSPDisplayList(dlHead++, dl_fasttext_begin);
    drawSmallStringCol(&dlHead, x, y, string, r, g, b);
    gSPDisplayList(dlHead++, dl_fasttext_end);
    gDisplayListHead = dlHead;
}

#endif
