#include "gfx_text.h"

// Δήλωσε τα primitives του driver σου:
void ST7735_DrawPixel(int16_t x, int16_t y, uint16_t color);

static inline int is_printable(char c) {
    return (c >= 0x20 && c <= 0x7F);
}

void GFX_DrawChar(int16_t x, int16_t y, char c,
                  uint16_t color, uint16_t bg, uint8_t scale,
                  const Font5x7_t *font)
{
    if (!is_printable(c)) c = '?';
    const uint8_t *glyph = font->data + (c - 0x20) * font->width;

    // Ζωγράφισε background αν ζητήθηκε (bg != transparent)
    const uint8_t w = font->width, h = font->height;

    for (uint8_t col = 0; col < w; col++) {
        uint8_t bits = glyph[col];
        for (uint8_t row = 0; row < h; row++) {
            uint16_t pix = (bits & 0x01) ? color : bg;
            if (bg != 0xFFFFu) { // χρησιμοποίησε 0xFFFF ως "transparent" flag
                // ορατό background
                for (uint8_t dx = 0; dx < scale; dx++)
                    for (uint8_t dy = 0; dy < scale; dy++)
                        ST7735_DrawPixel(x + col*scale + dx, y + row*scale + dy, pix);
            } else {
                // διαφανές background: μόνο τα foreground pixels
                if (pix == color) {
                    for (uint8_t dx = 0; dx < scale; dx++)
                        for (uint8_t dy = 0; dy < scale; dy++)
                            ST7735_DrawPixel(x + col*scale + dx, y + row*scale + dy, color);
                }
            }
            bits >>= 1;
        }
    }

    // 1 στήλη κενό ως διάκενο
    if (bg != 0xFFFFu) {
        for (uint8_t row = 0; row < h*scale; row++)
            for (uint8_t dx = 0; dx < scale; dx++)
                ST7735_DrawPixel(x + w*scale + dx, y + row, bg);
    }
}

void GFX_DrawString(int16_t x, int16_t y, const char *s,
                    uint16_t color, uint16_t bg, uint8_t scale,
                    const Font5x7_t *font)
{
    int16_t cx = x;
    for (const char *p = s; *p; ++p) {
        if (*p == '\n') {
            y += (font->height + 1) * scale;
            cx = x;
            continue;
        }
        GFX_DrawChar(cx, y, *p, color, bg, scale, font);
        cx += (font->width + 1) * scale; // +1 για το διάκενο στήλης
    }
}

int16_t GFX_DrawStringWrap(int16_t x, int16_t y, int16_t maxWidth,
                           const char *s, uint16_t color, uint16_t bg,
                           uint8_t scale, const Font5x7_t *font)
{
    int16_t cx = x;
    const int16_t adv = (font->width + 1) * scale;
    const int16_t lineH = (font->height + 1) * scale;

    for (const char *p = s; *p; ++p) {
        if (*p == '\n') { y += lineH; cx = x; continue; }
        if ((cx + adv - x) > maxWidth) { y += lineH; cx = x; }
        GFX_DrawChar(cx, y, *p, color, bg, scale, font);
        cx += adv;
    }
    return y + lineH; // next baseline
}
