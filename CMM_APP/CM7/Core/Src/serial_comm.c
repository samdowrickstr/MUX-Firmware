#include "serial_comm.h"
#include "main.h"
#include "udpclient.h"
#include "lwip/sys.h"
#include "cmsis_os.h"  // Include FreeRTOS for priority definitions

#define DMA_RX_BUFFER_SIZE 30  // Adjust buffer size as needed

/* Defines for RS232 and RS485 modes */
#define RS232_MODE 0
#define RS485_MODE 1

/* DMA buffers for RS232 and RS485 UART reception */
uint8_t RS232_DMA_RxBuffer[DMA_RX_BUFFER_SIZE];
uint8_t RS485_DMA_RxBuffer[DMA_RX_BUFFER_SIZE];

/* Global variables */
uint8_t mode;  // Mode variable to store the current serial mode (RS232 or RS485)
extern UART_HandleTypeDef huart2;  // For RS232
extern UART_HandleTypeDef huart3;  // For RS485

/**
  * @brief  Initialize UART DMA reception.
  */
void UART_DMA_Init(void)
{
    // Start UART DMA reception in circular mode for RS232 (USART2) and RS485 (USART3)
    HAL_UART_Receive_DMA(&huart2, RS232_DMA_RxBuffer, DMA_RX_BUFFER_SIZE);  // RS232 DMA reception
    HAL_UART_Receive_DMA(&huart3, RS485_DMA_RxBuffer, DMA_RX_BUFFER_SIZE);  // RS485 DMA reception

    Switch_RS_Mode(RS485_MODE);  // Default to RS485 mode
}

/**
  * @brief  Callback function when DMA transfer is complete or half-complete.
  *         This function processes the data and sends it over UDP.
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART3)  // RS485
    {
        // Process received RS485 data (entire buffer received)
        Process_DMA_RxData(RS485_DMA_RxBuffer, DMA_RX_BUFFER_SIZE);
    }
    else if (huart->Instance == USART2)  // RS232
    {
        // Process received RS232 data (entire buffer received)
        Process_DMA_RxData(RS232_DMA_RxBuffer, DMA_RX_BUFFER_SIZE);
    }
}

/**
  * @brief  Callback function when DMA half-transfer is complete.
  *         This function processes half of the data.
  */
void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART3)  // RS485
    {
        // Process the first half of the RS485 buffer
        Process_DMA_RxData(RS485_DMA_RxBuffer, DMA_RX_BUFFER_SIZE / 2);
    }
    else if (huart->Instance == USART2)  // RS232
    {
        // Process the first half of the RS232 buffer
        Process_DMA_RxData(RS232_DMA_RxBuffer, DMA_RX_BUFFER_SIZE / 2);
    }
}

/**
  * @brief  Processes the received UART data from DMA buffer and sends it over UDP.
  * @param  data: Pointer to the DMA buffer containing received data.
  * @param  len: Length of the received data to process.
  */
void Process_DMA_RxData(uint8_t *data, uint16_t len)
{
    // Iterate through the received data and send each byte over UDP
    for (uint16_t i = 0; i < len; i++)
    {
        udpsend_serial((const char *)&data[i], 1);  // Send each byte as a separate UDP packet
    }
}

/**
  * @brief  Sends data over the currently active serial communication mode (RS232/RS485).
  * @param  data: Pointer to the data buffer to be sent.
  * @param  len: Length of the data buffer.
  * @retval None
  */
void Serial_Send(uint8_t *data, uint16_t len)
{
    if (mode == RS232_MODE)
    {
        // RS232 Mode (USART2 for both send and receive)
        HAL_UART_Transmit(&huart2, data, len, HAL_MAX_DELAY);
    }
    else if (mode == RS485_MODE)
    {
        // RS485 Mode (USART2 for send, USART3 for receive)
        // Enable RS485 DE (Drive Enable) pin for transmission
        HAL_GPIO_WritePin(RS485_DE_RE_GPIO_Port, RS485_DE_RE_Pin, GPIO_PIN_SET);

        // Send data over RS485 (USART2)
        HAL_UART_Transmit(&huart2, data, len, HAL_MAX_DELAY);

        // After transmission, disable RS485 DE pin (set to reception mode)
        HAL_GPIO_WritePin(RS485_DE_RE_GPIO_Port, RS485_DE_RE_Pin, GPIO_PIN_RESET);
    }
}

/**
  * @brief  Switches the mode of serial communication between RS232 and RS485.
  * @param  RS_Mode: Mode to switch to (RS232_MODE or RS485_MODE).
  */
void Switch_RS_Mode(uint8_t RS_Mode)
{
    mode = RS_Mode;  // Set the global mode variable

    if (RS_Mode == RS232_MODE)
    {
        HAL_GPIO_WritePin(RS_MODE_SELECT_GPIO_Port, RS_MODE_SELECT_Pin, GPIO_PIN_RESET);  // Switch to RS232
        HAL_UART_Receive_DMA(&huart2, RS232_DMA_RxBuffer, DMA_RX_BUFFER_SIZE);  // Restart DMA reception for RS232
        printf("Switched to RS232 mode\n");
    }
    else if (RS_Mode == RS485_MODE)
    {
        HAL_GPIO_WritePin(RS_MODE_SELECT_GPIO_Port, RS_MODE_SELECT_Pin, GPIO_PIN_SET);  // Switch to RS485
        HAL_UART_Receive_DMA(&huart3, RS485_DMA_RxBuffer, DMA_RX_BUFFER_SIZE);  // Restart DMA reception for RS485
        printf("Switched to RS485 mode\n");
    }
}
