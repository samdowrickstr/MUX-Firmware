/**
  ******************************************************************************
  * @file    miim_library.h
  * @author  Samuel Dowrick
  * @brief   Header file for MIIM (Management Data Input/Output) bit-banging
  *          library.
  *          This file provides function prototypes for controlling Ethernet PHY
  *          registers via the MIIM interface using software-based bit-banging.
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

#ifndef INC_MIIM_LIBRARY_H_
#define INC_MIIM_LIBRARY_H_

/* Function Prototypes -------------------------------------------------------*/

/**
  * @brief  Provides a short delay based on a loop counter.
  * @param  tens_of_us: The delay time in tens of microseconds (approximate).
  * @retval None
  */
void MDIO_WAIT(uint16_t tens_of_us);

/**
  * @brief  Delays execution for a specified number of microseconds.
  * @param  us: The delay time in microseconds.
  * @retval None
  */
void delay_us(uint16_t us);

/**
  * @brief  Configures the MDIO pin as an input.
  * @retval None
  */
static void GPIO_SET_MDIO_MODE_INPUT(void);

/**
  * @brief  Configures both MDIO and MDC pins as inputs.
  * @retval None
  */
static void GPIO_SET_MDIO_MDC_MODE_INPUT(void);

/**
  * @brief  Configures MDIO and MDC pins for normal operation (output mode).
  * @retval None
  */
static void GPIO_SET_MODE_NORMAL(void);

/**
  * @brief  Generates a clock pulse on the MDC pin.
  * @retval None
  */
void _MIIM_DRIVER_CLOCK_PULSE(void);

/**
  * @brief  Generates the MIIM start sequence (preamble and start frame).
  * @retval None
  */
void _MIIM_DRIVER_START(void);

/**
  * @brief  Sends the MIIM read operation code (0b10).
  * @retval None
  */
void _MIIM_DRIVER_OP_CODE_READ(void);

/**
  * @brief  Sends the MIIM write operation code (0b01).
  * @retval None
  */
void _MIIM_DRIVER_OP_CODE_WRITE(void);

/**
  * @brief  Outputs the PHY and register addresses to the MIIM bus.
  * @param  PHY: The PHY address (5 bits).
  * @param  REG: The register address (5 bits).
  * @retval None
  */
void _MIIM_DRIVER_PHY_REG_OUTPUT(uint8_t PHY, uint8_t REG);

/**
  * @brief  Sends the turn-around bits for a MIIM write operation.
  * @retval None
  */
void _MIIM_DRIVER_TA_WRITE(void);

/**
  * @brief  Sends the turn-around bits for a MIIM read operation and prepares
  *         for reading data.
  * @retval None
  */
void _MIIM_DRIVER_TA_READ(void);

/**
  * @brief  Writes a 16-bit data word to the MIIM bus.
  * @param  data: The 16-bit data to write.
  * @retval None
  */
void _MIIM_DRIVER_WRITE_DATA(uint16_t data);

/**
  * @brief  Reads a 16-bit data word from the MIIM bus.
  * @retval uint16_t: The 16-bit data read from the MIIM bus.
  */
uint16_t _MIIM_DRIVER_READ_DATA(void);

/**
  * @brief  Writes data to a specific PHY register on the MIIM bus.
  * @param  PHY: The PHY address (5 bits).
  * @param  REG: The register address (5 bits).
  * @param  DATA: The 16-bit data to write.
  * @retval None
  */
void MIIM_DRIVER_WRITE(uint8_t PHY, uint8_t REG, uint16_t DATA);

/**
  * @brief  Reads data from a specific PHY register on the MIIM bus.
  * @param  PHY: The PHY address (5 bits).
  * @param  REG: The register address (5 bits).
  * @retval uint16_t: The 16-bit data read from the specified PHY register.
  */
uint16_t MIIM_DRIVER_READ(uint8_t PHY, uint8_t REG);

#endif /* INC_MIIM_LIBRARY_H_ */
