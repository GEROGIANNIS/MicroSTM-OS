#ifndef ST7735_H
#define ST7735_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "st7735_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ST7735_NOP       0x00
#define ST7735_SWRESET   0x01
#define ST7735_SLPIN     0x10
#define ST7735_SLPOUT    0x11
#define ST7735_INVOFF    0x20
#define ST7735_INVON     0x21
#define ST7735_GAMSET    0x26
#define ST7735_DISPOFF   0x28
#define ST7735_DISPON    0x29
#define ST7735_CASET     0x2A
#define ST7735_RASET     0x2B
#define ST7735_RAMWR     0x2C
#define ST7735_MADCTL    0x36
#define ST7735_COLMOD    0x3A
#define ST7735_FRMCTR1   0xB1
#define ST7735_FRMCTR2   0xB2
#define ST7735_FRMCTR3   0xB3
#define ST7735_INVCTR    0xB4
#define ST7735_PWCTR1    0xC0
#define ST7735_PWCTR2    0xC1
#define ST7735_PWCTR3    0xC2
#define ST7735_PWCTR4    0xC3
#define ST7735_PWCTR5    0xC4
#define ST7735_VMCTR1    0xC5
#define ST7735_GMCTRP1   0xE0
#define ST7735_GMCTRN1   0xE1

#define ST7735_MADCTL_MY  0x80
#define ST7735_MADCTL_MX  0x40
#define ST7735_MADCTL_MV  0x20
#define ST7735_MADCTL_ML  0x10
#define ST7735_MADCTL_RGB 0x00
#define ST7735_MADCTL_BGR 0x08

#define ST7735_COLMOD_16BIT 0x05  // 16bpp RGB565

typedef enum {
  ST7735_ROTATION_0 = 0,
  ST7735_ROTATION_90,
  ST7735_ROTATION_180,
  ST7735_ROTATION_270
} ST7735_Rotation_t;

void ST7735_Init(SPI_HandleTypeDef* hspi);
void ST7735_SetRotation(ST7735_Rotation_t r);
void ST7735_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void ST7735_SetInversion(bool enable);

void ST7735_FillScreen(uint16_t rgb565);
void ST7735_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t rgb565);
void ST7735_DrawBitmap16(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* pixels);

void ST7735_WritePixels(const uint16_t* pixels, size_t count);
HAL_StatusTypeDef ST7735_WritePixelsDMA(const uint16_t* pixels, size_t count);
void ST7735_WaitDMADone(void);
void ST7735_SPI_TxCpltCallback(SPI_HandleTypeDef* hspi);

uint16_t ST7735_Width(void);
uint16_t ST7735_Height(void);

void ST7735_DrawPixel(uint16_t x, uint16_t y, uint16_t rgb565);
void ST7735_DrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t rgb565);

#ifdef __cplusplus
}
#endif

#endif // ST7735_H
