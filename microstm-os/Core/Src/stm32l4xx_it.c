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

        if (esc_sequence_state == 2) { // ESC [ received, expect A or B
            esc_sequence_state = 0; // Reset state
            if (inByte == 'A') { // Up arrow
                if (historyCount > 0 && historyIndex > 0) {
                    historyIndex--;
                    // Clear current line
                    char clear_line[] = "\r\033[K";
                    HAL_UART_Transmit(&huart2, (uint8_t*)clear_line, sizeof(clear_line) - 1, 1000);

                    // Print prompt
                    extern void print_prompt(void);
                    print_prompt();

                    strncpy(inBuffer, commandHistory[historyIndex], MAX_COMMAND_LENGTH);
                    inPointer = strlen(inBuffer);
                    HAL_UART_Transmit(&huart2, (uint8_t*)inBuffer, inPointer, 1000);
                }
            } else if (inByte == 'B') { // Down arrow
                if (historyCount > 0 && historyIndex < historyCount -1) {
                    historyIndex++;
                    // Clear current line
                    char clear_line[] = "\r\033[K";
                    HAL_UART_Transmit(&huart2, (uint8_t*)clear_line, sizeof(clear_line) - 1, 1000);

                    // Print prompt
                    extern void print_prompt(void);
                    print_prompt();

                    strncpy(inBuffer, commandHistory[historyIndex], MAX_COMMAND_LENGTH);
                    inPointer = strlen(inBuffer);
                    HAL_UART_Transmit(&huart2, (uint8_t*)inBuffer, inPointer, 1000);
                } else if (historyIndex == historyCount -1) { // If at the last command, clear input
                    historyIndex = historyCount; // Move past the last command in history
                    // Clear current line
                    char clear_line[] = "\r\033[K";
                    HAL_UART_Transmit(&huart2, (uint8_t*)clear_line, sizeof(clear_line) - 1, 1000);

                    // Print prompt
                    extern void print_prompt(void);
                    print_prompt();

                    memset(inBuffer, 0, sizeof(inBuffer));
                    inPointer = 0;
                }
            }
        } else if (esc_sequence_state == 1) { // ESC received, expect [
            if (inByte == '[') {
                esc_sequence_state = 2;
            } else {
                esc_sequence_state = 0; // Not an escape sequence we care about
            }
        } else if (inByte == 27) { // ESC character
            esc_sequence_state = 1;
        } else if (inByte == '\r' || inByte == '\n') {
            // Echo newline
            char newline[] = "\r\n";
            HAL_UART_Transmit(&huart2, (uint8_t*)newline, sizeof(newline) - 1, 1000);

            // Save command to history if not empty and different from last command
            if (inPointer > 0 && (historyCount == 0 || strcmp(inBuffer, commandHistory[(historyCount - 1 + HISTORY_SIZE) % HISTORY_SIZE]) != 0)) {
                strncpy(commandHistory[historyCount], inBuffer, MAX_COMMAND_LENGTH - 1);
                commandHistory[historyCount][MAX_COMMAND_LENGTH - 1] = 0; // Ensure null termination
                historyCount = (historyCount + 1) % HISTORY_SIZE;
            }
            historyIndex = historyCount; // Reset history index to the latest command

            // Terminate the string and signal that data is ready
            inBuffer[inPointer] = 0;
            dataRxd = true;
            inPointer = 0;
            esc_sequence_state = 0; // Reset state
            } else if (inByte == '\b' || inByte == 127) {
                // Handle backspace
                if (inPointer > 0) {
                    inPointer--;
                    inBuffer[inPointer] = '\0'; // Null-terminate the string at the new end
                    // Erase character from terminal: backspace, space, backspace
                    char backspace_seq[] = "\b \b";
                    HAL_UART_Transmit(&huart2, (uint8_t*)backspace_seq, sizeof(backspace_seq) - 1, 1000);
                }
                esc_sequence_state = 0; // Reset state
            } else if (inByte == '\t') {
                tabCompletion = true;
                esc_sequence_state = 0; // Reset state
            } else {
                // Echo other characters and add to buffer
                if (inPointer < sizeof(inBuffer) - 1) {
                    HAL_UART_Transmit(&huart2, (uint8_t*)&inByte, 1, 1000);
                    inBuffer[inPointer++] = inByte;
                }
                esc_sequence_state = 0; // Reset state
            }
    }
  /* USER CODE END USART2_IRQn 0 */
  HAL_UART_IRQHandler(&huart2);
  /* USER CODE BEGIN USART2_IRQn 1 */

  /* USER CODE END USART2_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
