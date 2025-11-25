#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t code;
    uint32_t offset;
} U8GlyphIndex;

typedef struct {
    uint8_t  width;
    uint8_t  height;
    uint8_t  stride;
    const uint8_t     *data;
    const U8GlyphIndex *index;  // sorted by 'code'
    uint16_t count;
    uint8_t  advance;
} U8Font;

// Από τον driver σου (υλοποιείται αλλού)
extern void ST7735_DrawPixel(uint16_t x, uint16_t y, uint16_t color);

#ifndef GFX_BG_NONE
#define GFX_BG_NONE (-1)
#endif

// ----- helpers -----

static inline int u8_binsearch(const U8GlyphIndex *idx, int n, uint32_t code)
{
    int lo = 0, hi = n - 1;
    while (lo <= hi) {
        int mid = (lo + hi) >> 1;
        uint32_t c = idx[mid].code;
        if (c == code) return mid;
        if (c < code)  lo = mid + 1; else hi = mid - 1;
    }
    return -1;
}

static inline uint32_t utf8_next(const char **pp)
{
    const unsigned char *p = (const unsigned char*)*pp;
    uint32_t c = p[0];
    if (c < 0x80) { (*pp)++; return c; }

    if ((c & 0xE0) == 0xC0) {
        uint32_t c2 = p[1] & 0x3F;
        *pp += 2; return ((c & 0x1F) << 6) | c2;
    }
    if ((c & 0xF0) == 0xE0) {
        uint32_t c2 = p[1] & 0x3F, c3 = p[2] & 0x3F;
        *pp += 3; return ((c & 0x0F) << 12) | (c2 << 6) | c3;
    }
    if ((c & 0xF8) == 0xF0) {
        uint32_t c2 = p[1] & 0x3F, c3 = p[2] & 0x3F, c4 = p[3] & 0x3F;
        *pp += 4; return ((c & 0x07) << 18) | (c2 << 12) | (c3 << 6) | c4;
    }
    (*pp)++; return 0xFFFD; // replacement char
}

static inline void U8_DrawGlyph(int16_t x, int16_t y, uint16_t color, int32_t bg,
                                const U8Font *f, const uint8_t *g)
{
    const int transp = (bg < 0);
    for (uint8_t row = 0; row < f->height; ++row) {
        const uint8_t *line = g + row * f->stride;
        for (uint8_t col = 0; col < f->width; ++col) {
            uint8_t byte = line[col >> 3];
            uint8_t bit  = 7 - (col & 7);
            int on = (byte >> bit) & 1;
            if (transp) { if (on) ST7735_DrawPixel(x+col, y+row, color); }
            else        { ST7735_DrawPixel(x+col, y+row, on ? color : (uint16_t)bg); }
        }
    }
    if (bg >= 0) {
        for (uint8_t r = 0; r < f->height; ++r)
            ST7735_DrawPixel(x + f->width, y + r, (uint16_t)bg); // 1px spacing
    }
}

static inline void GFX_UTF8_DrawString(int16_t x, int16_t y, const char *utf8,
                                       uint16_t color, int32_t bg, const U8Font *f)
{
    if (!f || !utf8) return;
    int16_t sx = x;
    const char *p = utf8;
    while (*p) {
        uint32_t cp = utf8_next(&p);
        if (cp == '\n') { y += f->height + 1; x = sx; continue; }

        int idx = u8_binsearch(f->index, f->count, cp);
        if (idx >= 0) {
            const uint8_t *g = f->data + f->index[idx].offset;
            U8_DrawGlyph(x, y, color, bg, f, g);
        } else {
            // άγνωστο -> μικρό πλαίσιο
            for (uint8_t r = 0; r < f->height; ++r)
                for (uint8_t c = 0; c < f->width; ++c)
                    if (r==0 || r==f->height-1 || c==0 || c==f->width-1)
                        ST7735_DrawPixel(x+c, y+r, color);
        }
        x += f->advance;
    }
}

#ifdef __cplusplus
}
#endif
