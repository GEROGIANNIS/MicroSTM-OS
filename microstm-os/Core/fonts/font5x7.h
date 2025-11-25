#pragma once
#include <stdint.h>

extern const uint8_t font5x7_raw[96][5];

typedef struct {
    uint8_t width;    // 5
    uint8_t height;   // 7
    const uint8_t *data; // 96 * 5 bytes (ASCII 0x20..0x7F), column-major, LSB=top
} Font5x7_t;

extern const Font5x7_t Font5x7;
