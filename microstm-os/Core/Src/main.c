/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
//#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "st7735.h"
#include "gfx_text.h"
#include "u8text.h"
#include "u8font_unscii16.h"
#include "u8font_unscii8.h"
#include "string.h"
#include "stdio.h"
#include "math.h"
#include "stdlib.h"
#include "main.h"
#include "filesystem.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define COLOR_RGB565(r,g,b) ( ((r&0x1F)<<11) | ((g&0x3F)<<5) | ((b&0x1F)) )
#define COLOR_BLACK 0x0000
#define COLOR_WHITE 0xFFFF
#define COLOR_RED   0xF800
#define COLOR_GREEN 0x07E0
#define COLOR_BLUE  0x001F
#define TEXT_BG_TRANSPARENT 0xFFFF
#define WHITE 0xFFFF

#define LCD_LINE_HEIGHT 9  // 7 (font height) + 2 (offset)
#define LCD_TEXT_OFFSET 2 // 2 pixel offset between lines
#define LCD_TOP_OFFSET 10 // 10 pixel offset from the top
#define LCD_MAX_LINES 12 // Assuming 5x7 font and 160 height, ~160/7 = 22 lines. Let's use 12 for now.
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim4;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
	extern uint16_t inPointer;
	extern char inBuffer[2048];
	extern bool dataRxd;
	extern bool tabCompletion;

    char prompt_buffer[MAX_PATH_SIZE + 3]; // Global prompt buffer
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM4_Init(void);
void LCD_Print(const char* text, uint16_t color);
void handle_serial_and_display(char* command, char* args, char* outBuffer, size_t outBufferSize);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
    char* command;
    char* args;
    char outBuffer[2048];
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_SPI1_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
  ST7735_Init(ST7735_SPI);
  ST7735_SetInversion(false);

  ST7735_SetRotation(ST7735_ROTATION_90);
  ST7735_FillScreen(COLOR_BLACK);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);


  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
    filesystem_init();
	const char initMsg[] = "MicroSTM-OS initialized\r\n";
	HAL_UART_Transmit(&huart2, (uint8_t *)initMsg, (uint16_t)strlen(initMsg), 1000);
    
    print_prompt(); // Initial prompt

  	  USART2->CR1 |= USART_CR1_RXNEIE;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    while (1)
    {
        handle_serial_and_display(command, args, outBuffer, sizeof(outBuffer));
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_4BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 0;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 65535;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */
  HAL_TIM_MspPostInit(&htim4);

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LCD_RST_Pin|LCD_DC_Pin|LCD_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : PC6 PC7 PC8 PC9 */
  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : LCD_RST_Pin LCD_DC_Pin LCD_CS_Pin */
  GPIO_InitStruct.Pin = LCD_RST_Pin|LCD_DC_Pin|LCD_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
// Global variable to keep track of the current line on the LCD
static uint8_t lcd_current_line = 0;

void LCD_Print(const char* text, uint16_t color) {
    // Clear the current line before drawing new text
    ST7735_FillRect(0, LCD_TOP_OFFSET + lcd_current_line * LCD_LINE_HEIGHT, ST7735_Width(), LCD_LINE_HEIGHT, COLOR_BLACK);
    GFX_DrawString(4, LCD_TOP_OFFSET + lcd_current_line * LCD_LINE_HEIGHT, (char*)text, color, COLOR_BLACK, 1, &Font5x7);
    lcd_current_line++;
    if (lcd_current_line >= LCD_MAX_LINES) {
        lcd_current_line = 0; // Wrap around or scroll up
        // For simplicity, just wrap around. A more advanced implementation would scroll.
        ST7735_FillRect(0, LCD_TOP_OFFSET, ST7735_Width(), ST7735_Height() - LCD_TOP_OFFSET, COLOR_BLACK); // Clear relevant part of screen on wrap around for now
    }
}

void print_prompt(void) {
    fs_get_cwd_path(prompt_buffer, MAX_PATH_SIZE);
    strcat(prompt_buffer, "> ");
    HAL_UART_Transmit(&huart2, (uint8_t*)prompt_buffer, (uint16_t)strlen(prompt_buffer), 1000);
    // Also display prompt on LCD
    LCD_Print(prompt_buffer, COLOR_WHITE);
}

void handle_serial_and_display(char* command, char* args, char* outBuffer, size_t outBufferSize) {
    if (dataRxd == true){
        dataRxd = false;

        parse_command(inBuffer, &command, &args);

        if (command != NULL && strlen(command) > 0) {
            // Debug print to serial
            snprintf(outBuffer, outBufferSize, "Command: '%s', Args: '%s'\r\n", command, args ? args : "");
            HAL_UART_Transmit(&huart2, (uint8_t*)outBuffer, strlen(outBuffer), 1000);
            // Also display on LCD
            LCD_Print(outBuffer, COLOR_WHITE);

            if (strcmp(command, "BLUE") == 0) {
                if (args != NULL && strcmp(args, "ON") == 0) {
                    GPIOC->ODR |= (1 << 6);
                    LCD_Print("BLUE ON", COLOR_GREEN);
                } else if (args != NULL && strcmp(args, "OFF") == 0) {
                    GPIOC->ODR &= ~(1 << 6);
                    LCD_Print("BLUE OFF", COLOR_RED);
                }
            } else if (strcmp(command, "RED") == 0) {
                if (args != NULL && strcmp(args, "ON") == 0) {
                    GPIOC->ODR |= (1 << 7);
                    LCD_Print("RED ON", COLOR_GREEN);
                } else if (args != NULL && strcmp(args, "OFF") == 0) {
                    GPIOC->ODR &= ~(1 << 7);
                    LCD_Print("RED OFF", COLOR_RED);
                }
            } else if (strcmp(command, "GREEN") == 0) {
                if (args != NULL && strcmp(args, "ON") == 0) {
                    GPIOC->ODR |= (1 << 8);
                    LCD_Print("GREEN ON", COLOR_GREEN);
                } else if (args != NULL && strcmp(args, "OFF") == 0) {
                    GPIOC->ODR &= ~(1 << 8);
                    LCD_Print("GREEN OFF", COLOR_RED);
                }
            } else if (strcmp(command, "WHITE") == 0) {
                if (args != NULL && strcmp(args, "ON") == 0) {
                    GPIOC->ODR |= (1 << 9);
                    LCD_Print("WHITE ON", COLOR_GREEN);
                } else if (args != NULL && strcmp(args, "OFF") == 0) {
                    GPIOC->ODR &= ~(1 << 9);
                    LCD_Print("WHITE OFF", COLOR_RED);
                }
            } else if (strcmp(command, "help") == 0) {
                const char helpMsg[] =
                    "Available commands:\r\n"
                    "  help                - Show this message\r\n"
                    "  ls, pwd, cd         - File navigation\r\n"
                    "  mkdir, rmdir        - Create/remove directories\r\n"
                    "  rm, cp, mv          - Manage files\r\n"
                    "  touch, find         - Create/search files\r\n"
                    "  cat, head, tail     - View file contents\r\n"
                    "  BLUE ON / BLUE OFF  - Control onboard BLUE LED\r\n"
                    "  RED ON / RED OFF    - Control onboard RED LED\r\n"
                    "  GREEN ON / GREEN OFF- Control onboard GREEN LED\r\n"
                    "  WHITE ON / WHITE OFF- Control onboard WHITE LED\r\n"
                    "  exit                - Close the connection\r\n";
                HAL_UART_Transmit(&huart2, (uint8_t*)helpMsg, (uint16_t)strlen(helpMsg), 1000);
                LCD_Print("Help message sent to serial.", COLOR_WHITE);
            } else if (strcmp(command, "pwd") == 0) {
                fs_pwd(outBuffer, outBufferSize);
                HAL_UART_Transmit(&huart2, (uint8_t*)outBuffer, (uint16_t)strlen(outBuffer), 1000);
                LCD_Print(outBuffer, COLOR_WHITE);
            } else if (strcmp(command, "ls") == 0) {
                fs_ls(outBuffer, outBufferSize);
                HAL_UART_Transmit(&huart2, (uint8_t*)outBuffer, (uint16_t)strlen(outBuffer), 1000);
                LCD_Print(outBuffer, COLOR_WHITE);
            } else if (strcmp(command, "mkdir") == 0) {
                if (fs_mkdir(args) != 0) {
                    const char errMsg[] = "mkdir failed\r\n";
                    HAL_UART_Transmit(&huart2, (uint8_t*)errMsg, (uint16_t)strlen(errMsg), 1000);
                    LCD_Print(errMsg, COLOR_RED);
                } else {
                    snprintf(outBuffer, outBufferSize, "mkdir %s success\r\n", args);
                    LCD_Print(outBuffer, COLOR_GREEN);
                }
            } else if (strcmp(command, "cd") == 0) {
                if (fs_cd(args) != 0) {
                    const char errMsg[] = "cd failed\r\n";
                    HAL_UART_Transmit(&huart2, (uint8_t*)errMsg, (uint16_t)strlen(errMsg), 1000);
                    LCD_Print(errMsg, COLOR_RED);
                } else {
                    snprintf(outBuffer, outBufferSize, "cd %s success\r\n", args);
                    LCD_Print(outBuffer, COLOR_GREEN);
                }
            } else if (strcmp(command, "rmdir") == 0) {
                if (fs_rmdir(args) != 0) {
                    const char errMsg[] = "rmdir failed\r\n";
                    HAL_UART_Transmit(&huart2, (uint8_t*)errMsg, (uint16_t)strlen(errMsg), 1000);
                    LCD_Print(errMsg, COLOR_RED);
                } else {
                    snprintf(outBuffer, outBufferSize, "rmdir %s success\r\n", args);
                    LCD_Print(outBuffer, COLOR_GREEN);
                }
            } else if (strcmp(command, "clear") == 0) {
                const char clearScreen[] = "\033[2J\033[H";
                HAL_UART_Transmit(&huart2, (uint8_t*)clearScreen, strlen(clearScreen), 1000);
                ST7735_FillScreen(COLOR_BLACK);
                lcd_current_line = 0;
                LCD_Print("LCD Cleared", COLOR_WHITE);
            } else if (strcmp(command, "touch") == 0) {
                if (fs_touch(args) != 0) {
                    const char errMsg[] = "touch failed\r\n";
                    HAL_UART_Transmit(&huart2, (uint8_t*)errMsg, (uint16_t)strlen(errMsg), 1000);
                    LCD_Print(errMsg, COLOR_RED);
                } else {
                    snprintf(outBuffer, outBufferSize, "touch %s success\r\n", args);
                    LCD_Print(outBuffer, COLOR_GREEN);
                }
            } else if (strcmp(command, "cat") == 0) {
                if (fs_cat(args) != 0) {
                    const char errMsg[] = "cat failed\r\n";
                    HAL_UART_Transmit(&huart2, (uint8_t*)errMsg, (uint16_t)strlen(errMsg), 1000);
                    LCD_Print(errMsg, COLOR_RED);
                } else {
                    // fs_cat already transmits to UART. For LCD, it's more complex as it can be large.
                    // For now, just indicate that cat command was executed.
                    snprintf(outBuffer, outBufferSize, "cat %s executed. Output on serial.\r\n", args);
                    LCD_Print(outBuffer, COLOR_WHITE);
                }
            } else if (strcmp(command, "rm") == 0) {
                if (fs_rm(args) != 0) {
                    const char errMsg[] = "rm failed\r\n";
                    HAL_UART_Transmit(&huart2, (uint8_t*)errMsg, (uint16_t)strlen(errMsg), 1000);
                    LCD_Print(errMsg, COLOR_RED);
                } else {
                    snprintf(outBuffer, outBufferSize, "rm %s success\r\n", args);
                    LCD_Print(outBuffer, COLOR_GREEN);
                }
            } else if (strcmp(command, "exit") == 0) {
                const char exitMsg[] = "Goodbye!\r\n";
                HAL_UART_Transmit(&huart2, (uint8_t*)exitMsg, (uint16_t)strlen(exitMsg), 1000);
                HAL_UART_DeInit(&huart2);
                LCD_Print("Goodbye!", COLOR_WHITE);
                while(1);
            } else {
                snprintf(outBuffer, outBufferSize, "Unknown command: %s\r\n", command);
                HAL_UART_Transmit(&huart2, (uint8_t*)outBuffer, strlen(outBuffer), 1000);
                LCD_Print(outBuffer, COLOR_RED);
            }
        }
        
        // Clear the input buffer for the next command
        memset(inBuffer, 0, sizeof(inBuffer));

        print_prompt();
    }

    if (tabCompletion == true) {
        tabCompletion = false;
        char completion_buffer[MAX_FILENAME_SIZE];
        
        // Find the start of the current word to autocomplete
        int current_word_start = inPointer;
        while (current_word_start > 0 && inBuffer[current_word_start - 1] != ' ') {
            current_word_start--;
        }
        
        int partial_len = inPointer - current_word_start;
        
        if (partial_len > 0) {
            int completed_len = fs_autocomplete(inBuffer + current_word_start, partial_len, completion_buffer);
            if (completed_len > 0) {
                HAL_UART_Transmit(&huart2, (uint8_t*)completion_buffer, completed_len, 1000);
                strncpy(inBuffer + inPointer, completion_buffer, completed_len);
                inPointer += completed_len;
                inBuffer[inPointer] = 0;
            }
        }
    }
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
