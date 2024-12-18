/**
  ******************************************************************************
  * @file    tws_driver.c
  * @author  Samuel Dowrick
  * @brief   This file provides functions to interface with TWS hardware,
  *          handling the timing, GPIO configurations, and communication.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 Subsea Technology & Rentals
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#include "main.h"
#include "tws_driver.h"
#include <stdio.h>

static GPIO_InitTypeDef GPIO_InitStruct = {0, 0, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0};

/**
  * @brief  Delays execution for the specified number of microseconds.
  * @param  us: Number of microseconds to delay.
  * @retval None
  */
void TWS_delay_us(uint16_t us) {
    __HAL_TIM_SET_COUNTER(&htim1, 0);
    while (__HAL_TIM_GET_COUNTER(&htim1) < us);
}

/**
  * @brief  Configures the TWS_DATA pin to input mode for reading data.
  * @retval None
  */
/**
  * @brief  Configures the TWS_DATA pin to input mode for reading data.
  * @retval None
  */
static void GPIO_SET_TWS_MODE_INPUT() {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = TWS_DATA_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(TWS_DATA_GPIO_Port, &GPIO_InitStruct);
}

/**
  * @brief  Configures the TWS_DATA pin to output mode for normal operation.
  * @retval None
  */
static void GPIO_SET_MODE_NORMAL() {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = TWS_DATA_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; // Use high speed for faster switching
    HAL_GPIO_Init(TWS_DATA_GPIO_Port, &GPIO_InitStruct);
}


/**
  * @brief  Generates a clock pulse on the TWS_CLK pin.
  * @retval None
  */
void _TWS_DRIVER_CLOCK_PULSE() {
    HAL_GPIO_WritePin(TWS_CLK_GPIO_Port, TWS_CLK_Pin, GPIO_PIN_RESET);
    TWS_delay_us(10);
    HAL_GPIO_WritePin(TWS_CLK_GPIO_Port, TWS_CLK_Pin, GPIO_PIN_SET);
    TWS_delay_us(10);
}

/**
  * @brief  Generates the start sequence for the TWS communication protocol.
  * @retval None
  */
void _TWS_DRIVER_START() {
    HAL_GPIO_WritePin(TWS_DATA_GPIO_Port, TWS_DATA_Pin, GPIO_PIN_SET);

    for (uint8_t bitnum = 0; bitnum < 8; ++bitnum) {
        _TWS_DRIVER_CLOCK_PULSE();
    }

    HAL_GPIO_WritePin(TWS_DATA_GPIO_Port, TWS_DATA_Pin, GPIO_PIN_RESET);
    _TWS_DRIVER_CLOCK_PULSE();
    HAL_GPIO_WritePin(TWS_DATA_GPIO_Port, TWS_DATA_Pin, GPIO_PIN_SET);
    _TWS_DRIVER_CLOCK_PULSE();
}

/**
  * @brief  Sends the operation code for a read operation.
  * @retval None
  */
void _TWS_DRIVER_OP_CODE_READ() {
    HAL_GPIO_WritePin(TWS_DATA_GPIO_Port, TWS_DATA_Pin, GPIO_PIN_SET);
    _TWS_DRIVER_CLOCK_PULSE();
    HAL_GPIO_WritePin(TWS_DATA_GPIO_Port, TWS_DATA_Pin, GPIO_PIN_RESET);
    _TWS_DRIVER_CLOCK_PULSE();
}

/**
  * @brief  Sends the operation code for a write operation.
  * @retval None
  */
void _TWS_DRIVER_OP_CODE_WRITE() {
    HAL_GPIO_WritePin(TWS_DATA_GPIO_Port, TWS_DATA_Pin, GPIO_PIN_RESET);
    _TWS_DRIVER_CLOCK_PULSE();
    HAL_GPIO_WritePin(TWS_DATA_GPIO_Port, TWS_DATA_Pin, GPIO_PIN_SET);
    _TWS_DRIVER_CLOCK_PULSE();
}

/**
  * @brief  Outputs the switch ID and register address to the TWS hardware.
  * @param  switch_id: ID of the switch to be communicated with.
  * @param  REG: Register address to be communicated with.
  * @retval None
  */
void _TWS_DRIVER_SWITCH_REG_OUTPUT(uint8_t switch_id, uint8_t REG) {
    HAL_GPIO_WritePin(TWS_DATA_GPIO_Port, TWS_DATA_Pin, GPIO_PIN_RESET); // 0 bit
    _TWS_DRIVER_CLOCK_PULSE();
    HAL_GPIO_WritePin(TWS_DATA_GPIO_Port, TWS_DATA_Pin, GPIO_PIN_RESET); // 0 bit
    _TWS_DRIVER_CLOCK_PULSE();

    for (uint8_t bitnum = 0; bitnum < 8; ++bitnum) {
        if ((REG & (1 << (7 - bitnum))) == 0) {
            HAL_GPIO_WritePin(TWS_DATA_GPIO_Port, TWS_DATA_Pin, GPIO_PIN_RESET);
        } else {
            HAL_GPIO_WritePin(TWS_DATA_GPIO_Port, TWS_DATA_Pin, GPIO_PIN_SET);
        }
        _TWS_DRIVER_CLOCK_PULSE();
    }
}

/**
  * @brief  Sends the turnaround bits for a write operation.
  * @retval None
  */
void _TWS_DRIVER_TA_WRITE() {
    _TWS_DRIVER_OP_CODE_READ();
}

/**
  * @brief  Sends the turnaround bits for a read operation and switches the TWS_DATA pin to input mode.
  * @retval None
  */
void _TWS_DRIVER_TA_READ() {
    GPIO_SET_TWS_MODE_INPUT();
    _TWS_DRIVER_CLOCK_PULSE();
//    GPIO_SET_MODE_NORMAL();
//    HAL_GPIO_WritePin(TWS_DATA_GPIO_Port, TWS_DATA_Pin, GPIO_PIN_RESET);
    _TWS_DRIVER_CLOCK_PULSE();
//    GPIO_SET_TWS_MODE_INPUT();
}

/**
  * @brief  Writes a 16-bit data value to the TWS hardware.
  * @param  data: 16-bit data to be written.
  * @retval None
  */
void _TWS_DRIVER_WRITE_DATA(uint16_t data) {
    for (uint8_t bitnum = 0; bitnum <= 15; ++bitnum) {
        HAL_GPIO_WritePin(TWS_DATA_GPIO_Port, TWS_DATA_Pin, (data & (1 << (15 - bitnum))) == 0 ? GPIO_PIN_RESET : GPIO_PIN_SET);
        _TWS_DRIVER_CLOCK_PULSE();
    }
    _TWS_DRIVER_CLOCK_PULSE();
    HAL_GPIO_WritePin(TWS_DATA_GPIO_Port, TWS_DATA_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(TWS_CLK_GPIO_Port, TWS_CLK_Pin, GPIO_PIN_RESET);
}

/**
  * @brief  Reads a 16-bit data value from the TWS hardware.
  * @retval The 16-bit data value read from the TWS hardware.
  */
uint16_t _TWS_DRIVER_READ_DATA() {
    uint16_t data = 0;
    uint8_t receivedBits[16] = {0};  // Array to store the 16 data bits
    // Now, read the 16 data bits
    for (uint8_t bitnum = 0; bitnum <= 15; ++bitnum) {
        receivedBits[bitnum] = HAL_GPIO_ReadPin(TWS_DATA_GPIO_Port, TWS_DATA_Pin);  // Store the data bit
        data |= (receivedBits[bitnum] << (15 - bitnum));  // Accumulate the data
        _TWS_DRIVER_CLOCK_PULSE();
    }

    _TWS_DRIVER_CLOCK_PULSE();  // Final clock pulse after reading data
    GPIO_SET_MODE_NORMAL();     // Return the data pin to its normal state
    HAL_GPIO_WritePin(TWS_DATA_GPIO_Port, TWS_DATA_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(TWS_CLK_GPIO_Port, TWS_CLK_Pin, GPIO_PIN_RESET);

    // Print the received bits
    printf("Received bits: ");
    for (uint8_t i = 0; i < 16; i++) {
        printf("%d", receivedBits[i]);
    }
    printf("\n");

    return data;
}



/**
  * @brief  Writes a data value to a specific register on the TWS hardware.
  * @param  REG: Register address to write to.
  * @param  DATA: Data to be written to the register.
  * @retval None
  */
void TWS_DRIVER_WRITE(uint8_t REG, uint16_t DATA) {
    _TWS_DRIVER_START();
    _TWS_DRIVER_OP_CODE_WRITE();
    _TWS_DRIVER_SWITCH_REG_OUTPUT(0x00, REG);
    _TWS_DRIVER_TA_WRITE();
    _TWS_DRIVER_WRITE_DATA(DATA);
}

/**
  * @brief  Reads a data value from a specific register on the TWS hardware.
  * @param  REG: Register address to read from.
  * @retval The 16-bit data value read from the register.
  */
uint16_t TWS_DRIVER_READ(uint8_t REG) {
    _TWS_DRIVER_START();
    _TWS_DRIVER_OP_CODE_READ();
    _TWS_DRIVER_SWITCH_REG_OUTPUT(0x00, REG);
    _TWS_DRIVER_TA_READ();
    uint16_t data = _TWS_DRIVER_READ_DATA();
    GPIO_SET_MODE_NORMAL();
    return data;
}

/**
  * @brief  Reads a data value from a specific MII register on a specified PHY device.
  * @param  startIdle: Specifies whether to start the command (1) or set to idle (0).
  * @param  miiRegAddr: The MII register address to be accessed. Range: 0-31.
  * @param  phyAddr: The address of the PHY device to be accessed. Range: 0-31.
  * @retval The 16-bit data read from the specified PHY register.
  * @note   This function initiates a read operation on the specified PHY and MII register,
  *         waits for the operation to complete, and then returns the read data.
  */
uint16_t TWS_PHY_READ(uint8_t startIdle, uint8_t miiRegAddr, uint8_t phyAddr) {
    uint16_t command = 0;
    uint16_t readValue;

    // Construct the command for register 0x13
    // Bits [4:0]  : phyAddr
    // Bits [9:5]  : miiRegAddr
    // Bit 10      : writeAll (1 = write to all ports, 0 = specific PHY)
    // Bit 14      : 0 = read operation
    // Bit 15      : startIdle (1 = start command, 0 = idle)

    command |= (phyAddr & 0x1F);               // PHY address (5 bits)
    command |= ((miiRegAddr & 0x1F) << 5);     // MII register address (5 bits)
    command |= ((startIdle & 0x01) << 15);     // Start/Idle command (1 bit)

    // Write to register 0x13 to initiate the read command
    HAL_Delay(200);
    TWS_DRIVER_WRITE(0x13, command);
    HAL_Delay(200);
    // Read the value from register 0x14
    readValue = TWS_DRIVER_READ(0x14);
    // Return the read value
    return readValue;
}

/**
  * @brief  Writes a data value to a specific MII register on a specified PHY device.
  * @param  startIdle: Specifies whether to start the command (1) or set to idle (0).
  * @param  writeAll: Specifies whether to write to all ports (1) or a specific PHY (0).
  * @param  miiRegAddr: The MII register address to be accessed. Range: 0-31.
  * @param  phyAddr: The address of the PHY device to be accessed. Range: 0-31.
  * @param  data: The 16-bit data to be written to the specified PHY register.
  * @retval None
  * @note   This function initiates a write operation on the specified PHY and MII register,
  *         waits for the operation to complete, and then writes the provided data.
  */
void TWS_PHY_WRITE(uint8_t startIdle, uint8_t writeAll, uint8_t miiRegAddr, uint8_t phyAddr, uint16_t data) {
    uint16_t command = 0;

    // Construct the command for register 0x13
    // Bits [4:0]  : phyAddr
    // Bits [9:5]  : miiRegAddr
    // Bit 10      : writeAll (1 = write to all ports, 0 = specific PHY)
    // Bit 14      : 1 = write operation
    // Bit 15      : startIdle (1 = start command, 0 = idle)

    command |= (phyAddr & 0x1F);               // PHY address (5 bits)
    command |= ((miiRegAddr & 0x1F) << 5);     // MII register address (5 bits)
    command |= ((writeAll & 0x01) << 10);      // Write all TP (1 bit)
    command |= (1 << 14);                      // Write operation (1 bit)
    command |= ((startIdle & 0x01) << 15);     // Start/Idle command (1 bit)

    // Write the data to be written to register 0x14
    TWS_DRIVER_WRITE(0x14, data);

    // Write to register 0x13 to initiate the write command
    TWS_DRIVER_WRITE(0x13, command);
}
