#pragma once
#include <stdint.h>
#include <stddef.h>
#include "font5x7.h"

#ifdef __cplusplus
extern "C" {
#endif

void GFX_DrawChar(int16_t x, int16_t y, char c,
                  uint16_t color, uint16_t bg, uint8_t scale,
                  const Font5x7_t *font);

void GFX_DrawString(int16_t x, int16_t y, const char *s,
                    uint16_t color, uint16_t bg, uint8_t scale,
                    const Font5x7_t *font);

// Προαιρετικό, για αλλαγή γραμμής & απλή αναδίπλωση
int16_t GFX_DrawStringWrap(int16_t x, int16_t y, int16_t maxWidth,
                           const char *s, uint16_t color, uint16_t bg,
                           uint8_t scale, const Font5x7_t *font);

#ifdef __cplusplus
}
#endif
