#include "st7735.h"
#include <string.h>

#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

#ifndef SWAP16
#define SWAP16(a,b) do { int16_t _t = (a); (a) = (b); (b) = _t; } while(0)
#endif

static inline int16_t iabs16(int16_t v) { return (v < 0) ? -v : v; }


static SPI_HandleTypeDef* s_hspi = NULL;
static volatile bool s_dma_busy = false;

static uint16_t s_lcd_w = ST7735_LCD_WIDTH;
static uint16_t s_lcd_h = ST7735_LCD_HEIGHT;
static uint16_t s_x_offset = ST7735_X_OFFSET;
static uint16_t s_y_offset = ST7735_Y_OFFSET;

static inline void CS_Select(void)   { HAL_GPIO_WritePin(ST7735_CS_GPIO_Port,  ST7735_CS_Pin,  GPIO_PIN_RESET); }
static inline void CS_Unselect(void) { HAL_GPIO_WritePin(ST7735_CS_GPIO_Port,  ST7735_CS_Pin,  GPIO_PIN_SET);   }
static inline void DC_Cmd(void)      { HAL_GPIO_WritePin(ST7735_DC_GPIO_Port,  ST7735_DC_Pin,  GPIO_PIN_RESET); }
static inline void DC_Data(void)     { HAL_GPIO_WritePin(ST7735_DC_GPIO_Port,  ST7735_DC_Pin,  GPIO_PIN_SET);   }

// Switch SPI data size on-the-fly (8b for commands/params, 16b for pixels)
static void spi_set_datasize(uint32_t datasize)
{
  if (s_hspi->Init.DataSize == datasize) return;
  __HAL_SPI_DISABLE(s_hspi);
  s_hspi->Init.DataSize = datasize;
  HAL_SPI_Init(s_hspi); // reconfigures CR2 DS bits
}

// --- Low-level helpers (8-bit command/param TX) ---
static HAL_StatusTypeDef tx8(const uint8_t* data, uint16_t len) {
  spi_set_datasize(SPI_DATASIZE_8BIT);
  return HAL_SPI_Transmit(s_hspi, (uint8_t*)data, len, HAL_MAX_DELAY);
}

static void hw_reset(void)
{
#if ST7735_USE_SOFT_RESET_ONLY
  CS_Select();
  DC_Cmd(); uint8_t cmd = ST7735_SWRESET; tx8(&cmd,1);
  CS_Unselect();
  HAL_Delay(150);
#else
  HAL_GPIO_WritePin(ST7735_RST_GPIO_Port, ST7735_RST_Pin, GPIO_PIN_RESET);
  HAL_Delay(20);
  HAL_GPIO_WritePin(ST7735_RST_GPIO_Port, ST7735_RST_Pin, GPIO_PIN_SET);
  HAL_Delay(150);
#endif
}

static void cmd_param(uint8_t cmd, const uint8_t* params, uint16_t len)
{
#if ST7735_KEEP_CS_ASSERTED
  CS_Select();
  DC_Cmd(); tx8(&cmd,1);
  if (len) { DC_Data(); tx8(params, len); }
  CS_Unselect();
#else
  CS_Select(); DC_Cmd(); tx8(&cmd,1); CS_Unselect();
  if (len) { CS_Select(); DC_Data(); tx8(params, len); CS_Unselect(); }
#endif
}

static void set_addr_window16(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
  x0 += s_x_offset; x1 += s_x_offset;
  y0 += s_y_offset; y1 += s_y_offset;

  uint8_t col[4] = { (uint8_t)(x0>>8), (uint8_t)(x0&0xFF), (uint8_t)(x1>>8), (uint8_t)(x1&0xFF) };
  uint8_t row[4] = { (uint8_t)(y0>>8), (uint8_t)(y0&0xFF), (uint8_t)(y1>>8), (uint8_t)(y1&0xFF) };
  cmd_param(ST7735_CASET, col, 4);
  cmd_param(ST7735_RASET, row, 4);

#if ST7735_KEEP_CS_ASSERTED
  CS_Select();
  DC_Cmd(); uint8_t cmd = ST7735_RAMWR; tx8(&cmd,1);
  DC_Data(); // next: 16-bit stream
#else
  uint8_t cmd = ST7735_RAMWR;
  cmd_param(cmd, NULL, 0);
#endif
}

// --- Public API ---
void ST7735_Init(SPI_HandleTypeDef* hspi)
{
  s_hspi = hspi;

#ifdef ST7735_BL_GPIO_Port
  HAL_GPIO_WritePin(ST7735_BL_GPIO_Port, ST7735_BL_Pin, GPIO_PIN_RESET);
#endif

  hw_reset();

  cmd_param(ST7735_SLPOUT, NULL, 0);
  HAL_Delay(120);

  uint8_t colmod = ST7735_COLMOD_16BIT;
  cmd_param(ST7735_COLMOD, &colmod, 1);
  HAL_Delay(10);

  /*uint8_t frmctr1[] = {0x01, 0x2C, 0x2D};
  uint8_t frmctr2[] = {0x01, 0x2C, 0x2D};
  uint8_t frmctr3[] = {0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D};
  cmd_param(ST7735_FRMCTR1, frmctr1, sizeof(frmctr1));
  cmd_param(ST7735_FRMCTR2, frmctr2, sizeof(frmctr2));
  cmd_param(ST7735_FRMCTR3, frmctr3, sizeof(frmctr3));*/

  uint8_t frm1[] = {0x00, 0x06, 0x03};
  uint8_t frm2[] = {0x00, 0x06, 0x03};
  uint8_t frm3[] = {0x00, 0x06, 0x03, 0x00, 0x06, 0x03};
  cmd_param(ST7735_FRMCTR1, frm1, sizeof(frm1));
  cmd_param(ST7735_FRMCTR2, frm2, sizeof(frm2));
  cmd_param(ST7735_FRMCTR3, frm3, sizeof(frm3));


  uint8_t invctr = 0x07;
  cmd_param(ST7735_INVCTR, &invctr, 1);

  uint8_t pwctr1[] = {0xA2, 0x02, 0x84};
  uint8_t pwctr2 = 0xC5;
  uint8_t pwctr3[] = {0x0A, 0x00};
  uint8_t pwctr4[] = {0x8A, 0x2A};
  uint8_t pwctr5[] = {0x8A, 0xEE};
  cmd_param(ST7735_PWCTR1, pwctr1, sizeof(pwctr1));
  cmd_param(ST7735_PWCTR2, &pwctr2, 1);
  cmd_param(ST7735_PWCTR3, pwctr3, sizeof(pwctr3));
  cmd_param(ST7735_PWCTR4, pwctr4, sizeof(pwctr4));
  cmd_param(ST7735_PWCTR5, pwctr5, sizeof(pwctr5));

  uint8_t vmctr1 = 0x0E;
  cmd_param(ST7735_VMCTR1, &vmctr1, 1);

#if ST7735_USE_BGR
  uint8_t mad = ST7735_MADCTL_BGR;
#else
  uint8_t mad = ST7735_MADCTL_RGB;
#endif
  cmd_param(ST7735_MADCTL, &mad, 1);

  cmd_param(ST7735_INVOFF, NULL, 0);
  HAL_Delay(10);

  cmd_param(ST7735_DISPON, NULL, 0);
  HAL_Delay(100);
}

void ST7735_SetInversion(bool enable)
{
  if (enable) cmd_param(ST7735_INVON, NULL, 0);
  else        cmd_param(ST7735_INVOFF, NULL, 0);
  HAL_Delay(10);
}

void ST7735_SetRotation(ST7735_Rotation_t r)
{
#if ST7735_USE_BGR
  const uint8_t base = ST7735_MADCTL_BGR;
#else
  const uint8_t base = ST7735_MADCTL_RGB;
#endif
  uint8_t mad = base;
  switch (r)
  {
    case ST7735_ROTATION_0:   mad = base | ST7735_MADCTL_MX | ST7735_MADCTL_MY; s_lcd_w = ST7735_LCD_WIDTH;  s_lcd_h = ST7735_LCD_HEIGHT; break;
    case ST7735_ROTATION_90:  mad = base | ST7735_MADCTL_MY | ST7735_MADCTL_MV; s_lcd_w = ST7735_LCD_HEIGHT; s_lcd_h = ST7735_LCD_WIDTH;  break;
    case ST7735_ROTATION_180: mad = base;                                       s_lcd_w = ST7735_LCD_WIDTH;  s_lcd_h = ST7735_LCD_HEIGHT; break;
    case ST7735_ROTATION_270: mad = base | ST7735_MADCTL_MX | ST7735_MADCTL_MV; s_lcd_w = ST7735_LCD_HEIGHT; s_lcd_h = ST7735_LCD_WIDTH;  break;
  }
  cmd_param(ST7735_MADCTL, &mad, 1);
}

void ST7735_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
  if (x1 < x0 || y1 < y0) return;
  if (x1 >= s_lcd_w) x1 = s_lcd_w - 1;
  if (y1 >= s_lcd_h) y1 = s_lcd_h - 1;
  set_addr_window16(x0, y0, x1, y1);
}

void ST7735_WritePixels(const uint16_t* pixels, size_t count)
{
  // switch to 16-bit for pixel stream
  spi_set_datasize(SPI_DATASIZE_16BIT);
#if ST7735_KEEP_CS_ASSERTED
  HAL_SPI_Transmit(s_hspi, (uint8_t*)pixels, (uint16_t)count, HAL_MAX_DELAY);
#else
  CS_Select(); DC_Data();
  HAL_SPI_Transmit(s_hspi, (uint8_t*)pixels, (uint16_t)count, HAL_MAX_DELAY);
  CS_Unselect();
#endif
}

HAL_StatusTypeDef ST7735_WritePixelsDMA(const uint16_t* pixels, size_t count)
{
  if (s_dma_busy) return HAL_BUSY;
  s_dma_busy = true;
  spi_set_datasize(SPI_DATASIZE_16BIT);
#if ST7735_KEEP_CS_ASSERTED
  return HAL_SPI_Transmit_DMA(s_hspi, (uint8_t*)pixels, (uint16_t)count);
#else
  CS_Select(); DC_Data();
  return HAL_SPI_Transmit_DMA(s_hspi, (uint8_t*)pixels, (uint16_t)count);
#endif
}

void ST7735_WaitDMADone(void)
{
  while (s_dma_busy) { __WFE(); }
}

void ST7735_SPI_TxCpltCallback(SPI_HandleTypeDef* hspi)
{
  if (hspi == s_hspi)
  {
#if !ST7735_KEEP_CS_ASSERTED
    CS_Unselect();
#endif
    s_dma_busy = false;
  }
}

uint16_t ST7735_Width(void)  { return s_lcd_w; }
uint16_t ST7735_Height(void) { return s_lcd_h; }
//void ST7735_DrawPixel(uint16_t x, uint16_t y, uint16_t rgb565)
//{
//  if (x >= s_lcd_w || y >= s_lcd_h) return;
//
//  ST7735_SetAddressWindow(x, y, x, y);
//  ST7735_WritePixels(&rgb565, 1);
//#if ST7735_KEEP_CS_ASSERTED
//  CS_Unselect();
//#endif
//}

/**
 * Bresenham line (integer only).
 * Δέχεται σημεία (x0,y0) -> (x1,y1) και ενώνει με γραμμή χρώματος rgb565.
 * Τα σημεία μπορούν να είναι αρνητικά – γίνεται bounds check για κάθε pixel.
 */
void ST7735_DrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t rgb565)
{
  // Fast paths για οριζόντια/κάθετη
  if (y0 == y1) {
    if (x1 < x0) SWAP16(x0, x1);
    if (x1 < 0 || y0 < 0 || y0 >= (int16_t)s_lcd_h) return;
    uint16_t xs = (x0 < 0) ? 0 : (uint16_t)x0;
    uint16_t xe = (x1 >= (int16_t)s_lcd_w) ? (s_lcd_w - 1) : (uint16_t)x1;
    if (xe >= xs) ST7735_FillRect(xs, (uint16_t)y0, (xe - xs + 1), 1, rgb565);
    return;
  }
  if (x0 == x1) {
    if (y1 < y0) SWAP16(y0, y1);
    if (x0 < 0 || x0 >= (int16_t)s_lcd_w || y1 < 0) return;
    uint16_t ys = (y0 < 0) ? 0 : (uint16_t)y0;
    uint16_t ye = (y1 >= (int16_t)s_lcd_h) ? (s_lcd_h - 1) : (uint16_t)y1;
    if (ye >= ys) ST7735_FillRect((uint16_t)x0, ys, 1, (ye - ys + 1), rgb565);
    return;
  }

  // Γενική περίπτωση Bresenham
  int16_t dx = iabs16(x1 - x0);
  int16_t sx = (x0 < x1) ? 1 : -1;
  int16_t dy = -iabs16(y1 - y0);
  int16_t sy = (y0 < y1) ? 1 : -1;
  int16_t err = dx + dy;

  for (;;) {
    if (x0 >= 0 && y0 >= 0 && x0 < (int16_t)s_lcd_w && y0 < (int16_t)s_lcd_h)
      ST7735_DrawPixel((uint16_t)x0, (uint16_t)y0, rgb565);

    if (x0 == x1 && y0 == y1) break;

    int16_t e2 = (int16_t)(2 * err);
    if (e2 >= dy) { err += dy; x0 += sx; }
    if (e2 <= dx) { err += dx; y0 += sy; }
  }
}

void ST7735_FillScreen(uint16_t rgb565)
{
  ST7735_FillRect(0, 0, s_lcd_w, s_lcd_h, rgb565);
}

void ST7735_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t rgb565)
{
  if (x >= s_lcd_w || y >= s_lcd_h) return;
  if ((x + w) > s_lcd_w) w = s_lcd_w - x;
  if ((y + h) > s_lcd_h) h = s_lcd_h - y;

  ST7735_SetAddressWindow(x, y, x + w - 1, y + h - 1);

  uint16_t lineBuf[64];
  for (size_t i = 0; i < sizeof(lineBuf)/sizeof(lineBuf[0]); ++i)
    lineBuf[i] = rgb565;

  size_t total = (size_t)w * h;
  while (total)
  {
    size_t chunk = MIN(total, (size_t)(sizeof(lineBuf)/sizeof(lineBuf[0])));
    ST7735_WritePixels(lineBuf, chunk);
    total -= chunk;
  }

#if ST7735_KEEP_CS_ASSERTED
  CS_Unselect();
#endif
}

void ST7735_DrawBitmap16(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* pixels)
{
  if (x >= s_lcd_w || y >= s_lcd_h) return;
  if (x + w > s_lcd_w) w = s_lcd_w - x;
  if (y + h > s_lcd_h) h = s_lcd_h - y;

  for (uint16_t row = 0; row < h; ++row)
  {
    ST7735_SetAddressWindow(x, y + row, x + w - 1, y + row);
    const uint16_t* line = pixels + (row * w);
    ST7735_WritePixels(line, w);
#if ST7735_KEEP_CS_ASSERTED
    CS_Unselect();
#endif
  }
}
