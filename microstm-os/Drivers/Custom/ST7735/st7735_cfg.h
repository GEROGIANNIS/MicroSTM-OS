#ifndef ST7735_CFG_H
#define ST7735_CFG_H

#include "stm32l4xx_hal.h"
#include <stdint.h>
#include <stdbool.h>

#ifndef ST7735_LCD_WIDTH
#define ST7735_LCD_WIDTH   128
#endif
#ifndef ST7735_LCD_HEIGHT
#define ST7735_LCD_HEIGHT  160
#endif

#ifndef ST7735_X_OFFSET
#define ST7735_X_OFFSET    0
#endif
#ifndef ST7735_Y_OFFSET
#define ST7735_Y_OFFSET    0
#endif

extern SPI_HandleTypeDef hspi1;
#define ST7735_SPI   (&hspi1)

#define ST7735_CS_GPIO_Port   GPIOB
#define ST7735_CS_Pin         GPIO_PIN_6
#define ST7735_DC_GPIO_Port   GPIOB
#define ST7735_DC_Pin         GPIO_PIN_7
#define ST7735_RST_GPIO_Port  GPIOB
#define ST7735_RST_Pin        GPIO_PIN_8
#define ST7735_BL_GPIO_Port   GPIOB
#define ST7735_BL_Pin         GPIO_PIN_9

#ifndef ST7735_USE_SOFT_RESET_ONLY
#define ST7735_USE_SOFT_RESET_ONLY  0
#endif

#ifndef ST7735_USE_BGR
#define ST7735_USE_BGR  0
#endif

#ifndef ST7735_KEEP_CS_ASSERTED
#define ST7735_KEEP_CS_ASSERTED  1
#endif

#endif // ST7735_CFG_H
