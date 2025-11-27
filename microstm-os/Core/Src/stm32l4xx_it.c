/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32l4xx_it.c
  * @brief   Interrupt Service Routines.
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
#include "stm32l4xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
	uint16_t inPointer=0;
	char inBuffer[2048];
	bool dataRxd=false;
    bool tabCompletion = false;
    bool char_received = false;

    char commandHistory[HISTORY_SIZE][MAX_COMMAND_LENGTH];
    int historyCount = 0;
    int historyIndex = 0;
    int esc_sequence_state = 0; // 0: normal, 1: ESC received, 2: ESC [ received
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern UART_HandleTypeDef huart2;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

extern char prompt_buffer[];

void redraw_input_line(void) {
    char full_line[2048 + MAX_PATH_SIZE + 3];
    snprintf(full_line, sizeof(full_line), "\r\033[K%s%s", prompt_buffer, inBuffer);
    HAL_UART_Transmit(&huart2, (uint8_t*)full_line, strlen(full_line), 1000);

    // Move cursor back to the correct position
    if (strlen(inBuffer) > inPointer) {
        char move_cursor_left[16];
        snprintf(move_cursor_left, sizeof(move_cursor_left), "\033[%dD", (int)(strlen(inBuffer) - inPointer));
        HAL_UART_Transmit(&huart2, (uint8_t*)move_cursor_left, strlen(move_cursor_left), 1000);
    }
    
    char_received = true;
}

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
   while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32L4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32l4xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles USART2 global interrupt.
  */
void USART2_IRQHandler(void)
{
  /* USER CODE BEGIN USART2_IRQn 0 */
    uint32_t isr = USART2->ISR;
    if (isr & USART_ISR_RXNE) {
        char inByte = USART2->RDR;

        if (esc_sequence_state == 2) { // We received ESC [
            esc_sequence_state = 0; // Reset state after handling
            switch (inByte) {
                case 'A': // Up arrow
                    if (historyCount > 0 && historyIndex > 0) {
                        historyIndex--;
                        strncpy(inBuffer, commandHistory[historyIndex], MAX_COMMAND_LENGTH);
                        inPointer = strlen(inBuffer);
                        redraw_input_line();
                    }
                    break;
                case 'B': // Down arrow
                    if (historyCount > 0 && historyIndex < historyCount - 1) {
                        historyIndex++;
                        strncpy(inBuffer, commandHistory[historyIndex], MAX_COMMAND_LENGTH);
                        inPointer = strlen(inBuffer);
                        redraw_input_line();
                    } else if (historyIndex == historyCount - 1) {
                        historyIndex = historyCount;
                        memset(inBuffer, 0, sizeof(inBuffer));
                        inPointer = 0;
                        redraw_input_line();
                    }
                    break;
                case 'C': // Right arrow
                    if (inPointer < strlen(inBuffer)) {
                        inPointer++;
                        redraw_input_line();
                    }
                    break;
                case 'D': // Left arrow
                    if (inPointer > 0) {
                        inPointer--;
                        redraw_input_line();
                    }
                    break;
            }
        } else if (esc_sequence_state == 1) { // We received ESC
            if (inByte == '[') {
                esc_sequence_state = 2;
            } else {
                esc_sequence_state = 0;
            }
        } else if (inByte == 27) { // ESC char
            esc_sequence_state = 1;
        } else if (inByte == '\r' || inByte == '\n') {
            // Echo newline
            char newline[] = "\r\n";
            HAL_UART_Transmit(&huart2, (uint8_t*)newline, sizeof(newline) - 1, 1000);

            // Save command to history
            if (inPointer > 0 && (historyCount == 0 || strcmp(inBuffer, commandHistory[(historyCount - 1 + HISTORY_SIZE) % HISTORY_SIZE]) != 0)) {
                strncpy(commandHistory[historyCount], inBuffer, MAX_COMMAND_LENGTH - 1);
                commandHistory[historyCount][MAX_COMMAND_LENGTH - 1] = 0; // Ensure null termination
                historyCount = (historyCount + 1) % HISTORY_SIZE;
            }
            historyIndex = historyCount;

            inPointer = strlen(inBuffer);
            inBuffer[inPointer] = 0;
            dataRxd = true;
            inPointer = 0;
            esc_sequence_state = 0;
        } else if (inByte == '\b' || inByte == 127) { // Backspace
            if (inPointer > 0) {
                memmove(&inBuffer[inPointer - 1], &inBuffer[inPointer], strlen(inBuffer) - inPointer + 1);
                inPointer--;
                redraw_input_line();
            }
        } else if (inByte == '\t') {
            tabCompletion = true;
            esc_sequence_state = 0;
        } else if (inPointer < sizeof(inBuffer) - 1) { // Regular character
            memmove(&inBuffer[inPointer + 1], &inBuffer[inPointer], strlen(inBuffer) - inPointer + 1);
            inBuffer[inPointer] = inByte;
            inPointer++;
            redraw_input_line();
            esc_sequence_state = 0;
        }
    }
  /* USER CODE END USART2_IRQn 0 */
  HAL_UART_IRQHandler(&huart2);
  /* USER CODE BEGIN USART2_IRQn 1 */

  /* USER CODE END USART2_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
