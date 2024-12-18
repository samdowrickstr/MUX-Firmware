#ifndef UART_CALLBACKS_H
#define UART_CALLBACKS_H

#include "stm32h7xx_hal.h"

// Function prototypes for UART callbacks
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart);

#endif // UART_CALLBACKS_H
