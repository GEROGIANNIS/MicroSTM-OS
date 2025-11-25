// st7735_gfx_adapter.c
#include <stdint.h>
#include "st7735.h"  // βάλε το header του δικού σου driver

// Αν ο driver σου θέλει big-endian (MSB-first) στο SPI, άνοιξε το define
//#define ST7735_SWAP_BYTES 1

static inline uint16_t swap16(uint16_t v) { return (v << 8) | (v >> 8); }

void ST7735_DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
    // Προαιρετικά clip στα όρια οθόνης
    // if (x < 0 || y < 0 || x >= ST7735_WIDTH() || y >= ST7735_HEIGHT()) return;

    ST7735_SetAddressWindow(x, y, x, y);

    #ifdef ST7735_SWAP_BYTES
    uint16_t c = swap16(color);
    ST7735_WritePixels(&c, 1);
    #else
    ST7735_WritePixels(&color, 1);
    #endif
}
