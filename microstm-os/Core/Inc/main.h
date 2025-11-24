/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
	#include "stdbool.h"
	#include "string.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN EV */

extern bool tabCompletion;
/* USER CODE END EV */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
extern void print_prompt(void);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define MAX_COMMAND_LENGTH 2048
#define HISTORY_SIZE 10

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

/* Exported variables --------------------------------------------------------*/
extern char commandHistory[HISTORY_SIZE][MAX_COMMAND_LENGTH];
extern int historyCount;
extern int historyIndex;
extern int esc_sequence_state;

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
