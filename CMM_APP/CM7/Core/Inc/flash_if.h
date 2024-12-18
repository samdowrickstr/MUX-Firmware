/**
  ******************************************************************************
  * @file    flash_if.h
  * @author  Samuel Dowrick
  * @brief   Header file for flash interface module.
  *          This file provides function prototypes for managing internal Flash
  *          memory operations such as initialization, erasure, and writing.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FLASH_IF_H
#define __FLASH_IF_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Exported types ------------------------------------------------------------*/
/* No exported types in this file */

/* Exported constants --------------------------------------------------------*/
/**
  * @brief  Defines the size of the user Flash area.
  *         The size is calculated as the difference between the end and start
  *         addresses of the user Flash area.
  */
#define USER_FLASH_SIZE   (USER_FLASH_END_ADDRESS - USER_FLASH_FIRST_PAGE_ADDRESS)

/* Exported macro ------------------------------------------------------------*/
/* No exported macros in this file */

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  Writes a data buffer to Flash memory.
  * @note   This function assumes that the data buffer is 32-bit aligned.
  * @param  Address: Pointer to the start address for writing the data buffer.
  * @param  Data: Pointer to the data buffer to be written.
  * @param  DataLength: Length of the data buffer in 32-bit words.
  * @retval 0: Data successfully written to Flash memory.
  *         1: Error occurred while writing data to Flash memory.
  *         2: Written data in Flash memory does not match expected value.
  */
uint32_t FLASH_If_Write(__IO uint32_t* Address, uint32_t* Data, uint16_t DataLength);

/**
  * @brief  Erases the specified Flash sector.
  * @param  StartSector: Start address of the sector to be erased.
  * @retval 0: Sector successfully erased.
  *         1: Error occurred while erasing the sector.
  */
int8_t FLASH_If_Erase(uint32_t StartSector);

/**
  * @brief  Initializes the Flash interface for write operations.
  * @note   This function unlocks the Flash memory for write access.
  * @retval None
  */
void FLASH_If_Init(void);

#endif /* __FLASH_IF_H */
