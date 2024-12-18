// In uart_callbacks.c
#include "main.h"
#include "PMB_comm.h"
#include "serial_comm.h"
#include "uart_callbacks.h"


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2 || huart->Instance == USART3) {
        // Call the function from serial_comm.c
        Serial_UART_RxCpltCallback(huart);
    } else if (huart->Instance == UART4) {
        // Call the function from PMB_comm.c
        PMB_UART_RxCpltCallback(huart);
    }
}

void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2 || huart->Instance == USART3) {
        // Call the function from serial_comm.c
        Serial_UART_RxHalfCpltCallback(huart);
    } else if (huart->Instance == UART4) {
        // If PMB_comm.c doesn't use this callback, you can leave this empty or handle accordingly
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2 || huart->Instance == USART3) {
        // Call the function from serial_comm.c
        Serial_UART_ErrorCallback(huart);
    } else if (huart->Instance == UART4) {
        // Call the function from PMB_comm.c
        PMB_UART_ErrorCallback(huart);
    }
}
