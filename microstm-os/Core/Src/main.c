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
#include "main.h"
#include "filesystem.h"
#include <string.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
	extern uint16_t inPointer;
	extern char inBuffer[2048];
	extern bool dataRxd;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
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
    char* command = NULL;
    char* args = NULL;
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
  /* USER CODE BEGIN 2 */
    filesystem_init();
	const char initMsg[] = "MicroSTM-OS initialized\r\n";
	HAL_UART_Transmit(&huart2, (uint8_t *)initMsg, (uint16_t)strlen(initMsg), 1000);
    
    char prompt[MAX_PATH_SIZE + 3];
    fs_get_cwd_path(prompt, MAX_PATH_SIZE);
    strcat(prompt, "> ");
    HAL_UART_Transmit(&huart2, (uint8_t*)prompt, (uint16_t)strlen(prompt), 1000);

  	  USART2->CR1 |= USART_CR1_RXNEIE;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    while (1)
    {
  	  if (dataRxd == true){
  		  dataRxd = false;
  
            parse_command(inBuffer, &command, &args);
  
            if (command != NULL && strlen(command) > 0) {
                // Debug print
                snprintf(outBuffer, sizeof(outBuffer), "Command: '%s', Args: '%s'\r\n", command, args ? args : "");
                HAL_UART_Transmit(&huart2, (uint8_t*)outBuffer, strlen(outBuffer), 1000);
  
                if (strcmp(command, "BLUE") == 0) {
                    if (args != NULL && strcmp(args, "ON") == 0) {
                        GPIOC->ODR |= (1 << 6);
                    } else if (args != NULL && strcmp(args, "OFF") == 0) {
                        GPIOC->ODR &= ~(1 << 6);
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
                        "  BLUE ON / BLUE OFF  - Control onboard LED\r\n";
                    HAL_UART_Transmit(&huart2, (uint8_t*)helpMsg, (uint16_t)strlen(helpMsg), 1000);
                } else if (strcmp(command, "pwd") == 0) {
                    fs_pwd(outBuffer, sizeof(outBuffer));
                    HAL_UART_Transmit(&huart2, (uint8_t*)outBuffer, (uint16_t)strlen(outBuffer), 1000);
                } else if (strcmp(command, "ls") == 0) {
                    fs_ls(outBuffer, sizeof(outBuffer));
                    HAL_UART_Transmit(&huart2, (uint8_t*)outBuffer, (uint16_t)strlen(outBuffer), 1000);
                } else if (strcmp(command, "mkdir") == 0) {
                    if (fs_mkdir(args) != 0) {
                        const char errMsg[] = "mkdir failed\r\n";
                        HAL_UART_Transmit(&huart2, (uint8_t*)errMsg, (uint16_t)strlen(errMsg), 1000);
                    }
                } else if (strcmp(command, "cd") == 0) {
                    if (fs_cd(args) != 0) {
                        const char errMsg[] = "cd failed\r-n";
                        HAL_UART_Transmit(&huart2, (uint8_t*)errMsg, (uint16_t)strlen(errMsg), 1000);
                    }
                } else if (strcmp(command, "rmdir") == 0) {
                    if (fs_rmdir(args) != 0) {
                        const char errMsg[] = "rmdir failed\r\n";
                        HAL_UART_Transmit(&huart2, (uint8_t*)errMsg, (uint16_t)strlen(errMsg), 1000);
                    }
                } else if (strcmp(command, "clear") == 0) {
                    const char clearScreen[] = "\033[2J\033[H";
                    HAL_UART_Transmit(&huart2, (uint8_t*)clearScreen, strlen(clearScreen), 1000);
                } else if (strcmp(command, "touch") == 0) {
                    if (fs_touch(args) != 0) {
                        const char errMsg[] = "touch failed\r\n";
                        HAL_UART_Transmit(&huart2, (uint8_t*)errMsg, (uint16_t)strlen(errMsg), 1000);
                    }
                } else if (strcmp(command, "cat") == 0) {
                    if (fs_cat(args) != 0) {
                        const char errMsg[] = "cat failed\r\n";
                        HAL_UART_Transmit(&huart2, (uint8_t*)errMsg, (uint16_t)strlen(errMsg), 1000);
                    }
                } else if (strcmp(command, "rm") == 0) {
                    if (fs_rm(args) != 0) {
                        const char errMsg[] = "rm failed\r\n";
                        HAL_UART_Transmit(&huart2, (uint8_t*)errMsg, (uint16_t)strlen(errMsg), 1000);
                    }
                } else {
                    snprintf(outBuffer, sizeof(outBuffer), "Unknown command: %s\r\n", command);
                    HAL_UART_Transmit(&huart2, (uint8_t*)outBuffer, strlen(outBuffer), 1000);
  			  }
            }
            
            // Clear the input buffer for the next command
            memset(inBuffer, 0, sizeof(inBuffer));
  
            fs_get_cwd_path(prompt, MAX_PATH_SIZE);
            strcat(prompt, "> ");
            HAL_UART_Transmit(&huart2, (uint8_t*)prompt, (uint16_t)strlen(prompt), 1000);
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

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pins : PC6 PC7 PC8 PC9 */
  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

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
