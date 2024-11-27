#ifndef SERIAL_COMM_H
#define SERIAL_COMM_H

#include "stm32h7xx_hal.h"

/* Mode definitions */
#define RS232_MODE 0
#define RS485_MODE 1

#define RX_BUFFER_SIZE 1 // Since we're processing one byte at a time, buffer size is 1

void UART_DMA_Init(void);
void Serial_Send(uint8_t *data, uint16_t len);
void Switch_RS_Mode(uint8_t RS_Mode);

// Callback function prototypes
void Serial_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void Serial_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart);
void Serial_UART_ErrorCallback(UART_HandleTypeDef *huart);

#endif // SERIAL_COMM_H
