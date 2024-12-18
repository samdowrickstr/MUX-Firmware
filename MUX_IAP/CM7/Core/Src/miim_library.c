/**
  ******************************************************************************
  * @file    miim_library.c
  * @author  Samuel Dowrick
  * @brief   This file provides low-level routines for managing the MDIO/MDC
  *          communication with Ethernet PHYs, including reading and writing
  *          to PHY registers using bit-banging.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "miim_library.h"

/* Private variables ---------------------------------------------------------*/
static GPIO_InitTypeDef GPIO_InitStruct = {0, 0, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  This function provides a precise microsecond delay.
  * @note   This is used for timing in the MDIO communication.
  * @param  us: The delay duration in microseconds.
  * @retval None
  */
void delay_us(uint16_t us)
{
    __HAL_TIM_SET_COUNTER(&htim1, 0);  // Reset the timer counter
    while (__HAL_TIM_GET_COUNTER(&htim1) < us);  // Wait until the counter reaches the desired value
}

/**
  * @brief  Configures the MDIO pin as input.
  * @note   This is used when reading data from the PHY.
  * @retval None
  */
static void GPIO_SET_MDIO_MODE_INPUT()
{
    HAL_GPIO_DeInit(MIIM_MDIO_GPIO_Port, MIIM_MDIO_Pin);

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    HAL_GPIO_WritePin(MIIM_MDC_GPIO_Port, MIIM_MDC_Pin, GPIO_PIN_RESET);

    GPIO_InitStruct.Pin = MIIM_MDC_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = MIIM_MDIO_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init(MIIM_MDIO_GPIO_Port, &GPIO_InitStruct);
}

/**
  * @brief  Configures both MDIO and MDC pins as input.
  * @note   This is used when switching between reading and writing modes.
  * @retval None
  */
static void GPIO_SET_MDIO_MDC_MODE_INPUT()
{
    HAL_GPIO_DeInit(MIIM_MDIO_GPIO_Port, MIIM_MDIO_Pin);
    HAL_GPIO_DeInit(MIIM_MDC_GPIO_Port, MIIM_MDC_Pin);

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitStruct.Pin = MIIM_MDIO_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init(MIIM_MDIO_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = MIIM_MDC_Pin;
    HAL_GPIO_Init(MIIM_MDC_GPIO_Port, &GPIO_InitStruct);
}

/**
  * @brief  Configures the MDIO and MDC pins for normal operation (output mode).
  * @note   This is used when driving the MDIO and MDC lines.
  * @retval None
  */
static void GPIO_SET_MODE_NORMAL()
{
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    HAL_GPIO_WritePin(MIIM_MDC_GPIO_Port, MIIM_MDC_Pin | MIIM_MDIO_Pin, GPIO_PIN_RESET);

    GPIO_InitStruct.Pin = MIIM_MDC_Pin | MIIM_MDIO_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/**
  * @brief  Generates a clock pulse on the MDC line.
  * @note   This is part of the bit-banging process for MDIO communication.
  * @retval None
  */
void _MIIM_DRIVER_CLOCK_PULSE()
{
    HAL_GPIO_WritePin(MIIM_MDC_GPIO_Port, MIIM_MDC_Pin, GPIO_PIN_RESET);
    delay_us(1);
    HAL_GPIO_WritePin(MIIM_MDC_GPIO_Port, MIIM_MDC_Pin, GPIO_PIN_SET);
    delay_us(1);
}

/**
  * @brief  Sends the start signal for an MDIO transaction.
  * @note   This includes the preamble and start bits.
  * @retval None
  */
void _MIIM_DRIVER_START()
{
    HAL_GPIO_WritePin(MIIM_MDIO_GPIO_Port, MIIM_MDIO_Pin, GPIO_PIN_SET);

    for (uint8_t bitnum = 0; bitnum < 32; ++bitnum)
    {
        _MIIM_DRIVER_CLOCK_PULSE();
    }

    HAL_GPIO_WritePin(MIIM_MDIO_GPIO_Port, MIIM_MDIO_Pin, GPIO_PIN_RESET);
    _MIIM_DRIVER_CLOCK_PULSE();
    HAL_GPIO_WritePin(MIIM_MDIO_GPIO_Port, MIIM_MDIO_Pin, GPIO_PIN_SET);
    _MIIM_DRIVER_CLOCK_PULSE();
}

/**
  * @brief  Sends the operation code for an MDIO read transaction.
  * @note   This is used to instruct the PHY to prepare for a read operation.
  * @retval None
  */
void _MIIM_DRIVER_OP_CODE_READ()
{
    HAL_GPIO_WritePin(MIIM_MDIO_GPIO_Port, MIIM_MDIO_Pin, GPIO_PIN_SET);
    _MIIM_DRIVER_CLOCK_PULSE();
    HAL_GPIO_WritePin(MIIM_MDIO_GPIO_Port, MIIM_MDIO_Pin, GPIO_PIN_RESET);
    _MIIM_DRIVER_CLOCK_PULSE();
}

/**
  * @brief  Sends the operation code for an MDIO write transaction.
  * @note   This is used to instruct the PHY to prepare for a write operation.
  * @retval None
  */
void _MIIM_DRIVER_OP_CODE_WRITE()
{
    HAL_GPIO_WritePin(MIIM_MDIO_GPIO_Port, MIIM_MDIO_Pin, GPIO_PIN_RESET);
    _MIIM_DRIVER_CLOCK_PULSE();
    HAL_GPIO_WritePin(MIIM_MDIO_GPIO_Port, MIIM_MDIO_Pin, GPIO_PIN_SET);
    _MIIM_DRIVER_CLOCK_PULSE();
}

/**
  * @brief  Outputs the PHY and register address on the MDIO line.
  * @note   This is part of the MDIO protocol for addressing specific PHY registers.
  * @param  PHY: The PHY address (5 bits).
  * @param  REG: The register address (5 bits).
  * @retval None
  */
void _MIIM_DRIVER_PHY_REG_OUTPUT(uint8_t PHY, uint8_t REG)
{
    for (uint8_t bitnum = 0; bitnum <= 4; ++bitnum)
    {
        HAL_GPIO_WritePin(MIIM_MDIO_GPIO_Port, MIIM_MDIO_Pin, (PHY & (1 << (4-bitnum))) == 0 ? GPIO_PIN_RESET : GPIO_PIN_SET);
        _MIIM_DRIVER_CLOCK_PULSE();
    }

    for (uint8_t bitnum = 0; bitnum <= 4; ++bitnum)
    {
        HAL_GPIO_WritePin(MIIM_MDIO_GPIO_Port, MIIM_MDIO_Pin, (REG & (1 << (4-bitnum))) == 0 ? GPIO_PIN_RESET : GPIO_PIN_SET);
        _MIIM_DRIVER_CLOCK_PULSE();
    }
}

/**
  * @brief  Sends the turnaround bits for an MDIO write operation.
  * @note   This is part of the MDIO protocol signaling the start of data transfer.
  * @retval None
  */
void _MIIM_DRIVER_TA_WRITE()
{
    _MIIM_DRIVER_OP_CODE_READ();
}

/**
  * @brief  Handles the turnaround period for an MDIO read operation.
  * @note   Switches the MDIO line to input mode to allow data reading from the PHY.
  * @retval None
  */
void _MIIM_DRIVER_TA_READ()
{
    GPIO_SET_MDIO_MODE_INPUT();
    _MIIM_DRIVER_CLOCK_PULSE();
    GPIO_SET_MODE_NORMAL();
    HAL_GPIO_WritePin(MIIM_MDIO_GPIO_Port, MIIM_MDIO_Pin, GPIO_PIN_RESET);
    _MIIM_DRIVER_CLOCK_PULSE();
}

/**
  * @brief  Writes a 16-bit data value to the MDIO line.
  * @note   This is used to transmit data to a PHY register.
  * @param  data: The 16-bit data value to be written.
  * @retval None
  */
void _MIIM_DRIVER_WRITE_DATA(uint16_t data)
{
    for (uint8_t bitnum = 0; bitnum <= 15; ++bitnum)
    {
        HAL_GPIO_WritePin(MIIM_MDIO_GPIO_Port, MIIM_MDIO_Pin, (data & (1 << (15-bitnum))) == 0 ? GPIO_PIN_RESET : GPIO_PIN_SET);
        _MIIM_DRIVER_CLOCK_PULSE();
    }

    _MIIM_DRIVER_CLOCK_PULSE();
    HAL_GPIO_WritePin(MIIM_MDIO_GPIO_Port, MIIM_MDIO_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(MIIM_MDC_GPIO_Port, MIIM_MDC_Pin, GPIO_PIN_RESET);
}

/**
  * @brief  Reads a 16-bit data value from the MDIO line.
  * @note   This is used to retrieve data from a PHY register.
  * @retval The 16-bit data value read from the PHY register.
  */
uint16_t _MIIM_DRIVER_READ_DATA()
{
    GPIO_SET_MDIO_MODE_INPUT();
    uint16_t data = 0;
    for (uint8_t bitnum = 0; bitnum <= 15; ++bitnum)
    {
        data = data + (HAL_GPIO_ReadPin(MIIM_MDIO_GPIO_Port, MIIM_MDIO_Pin) << (15-bitnum));
        _MIIM_DRIVER_CLOCK_PULSE();
    }

    _MIIM_DRIVER_CLOCK_PULSE();
    GPIO_SET_MODE_NORMAL();
    HAL_GPIO_WritePin(MIIM_MDIO_GPIO_Port, MIIM_MDIO_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(MIIM_MDC_GPIO_Port, MIIM_MDC_Pin, GPIO_PIN_RESET);
    return data;
}

/**
  * @brief  Writes a 16-bit data value to a specified PHY register.
  * @param  PHY: The PHY address (5 bits).
  * @param  REG: The register address (5 bits).
  * @param  DATA: The 16-bit data value to be written.
  * @retval None
  */
void MIIM_DRIVER_WRITE(uint8_t PHY, uint8_t REG, uint16_t DATA)
{
    _MIIM_DRIVER_START();
    _MIIM_DRIVER_OP_CODE_WRITE();
    _MIIM_DRIVER_PHY_REG_OUTPUT(PHY, REG);
    _MIIM_DRIVER_TA_WRITE();
    _MIIM_DRIVER_WRITE_DATA(DATA);
}

/**
  * @brief  Reads a 16-bit data value from a specified PHY register.
  * @param  PHY: The PHY address (5 bits).
  * @param  REG: The register address (5 bits).
  * @retval The 16-bit data value read from the PHY register.
  */
uint16_t MIIM_DRIVER_READ(uint8_t PHY, uint8_t REG)
{
    _MIIM_DRIVER_START();
    _MIIM_DRIVER_OP_CODE_READ();
    _MIIM_DRIVER_PHY_REG_OUTPUT(PHY, REG);
    _MIIM_DRIVER_TA_READ();
    return _MIIM_DRIVER_READ_DATA();
}
