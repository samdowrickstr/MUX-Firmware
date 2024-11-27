/**
  ******************************************************************************
  * @file    flash_if.c
  * @author  Samuel Dowrick
  * @brief   This file provides high level routines to manage internal Flash 
  *          programming (erase and write). 
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
#include "flash_if.h"
#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Unlocks Flash for write access.
  *         This function is necessary to allow writing to the Flash memory.
  * @param  None
  * @retval None
  */
void FLASH_If_Init(void)
{ 
   printf("Unlocking Flash for write access\n");
   HAL_FLASH_Unlock(); 
   printf("Flash unlocked successfully\n");
}

/**
  * @brief  This function erases the user-defined flash area.
  *         It starts erasing from the specified StartSector to the end of the user flash area.
  * @param  StartSector: The starting address of the flash area to be erased.
  * @retval 0: Flash area successfully erased.
  *         1: Error occurred during the erase operation.
  */
int8_t FLASH_If_Erase(uint32_t StartSector)
{
  uint32_t FlashAddress;
 
  FlashAddress = StartSector;
  printf("Starting Flash erase from address: 0x%08X\n", FlashAddress);

  /* Device voltage range is assumed to be [2.7V to 3.6V], the operation will be done by sector */

  if (FlashAddress <= (uint32_t) USER_FLASH_LAST_PAGE_ADDRESS)
  {
    FLASH_EraseInitTypeDef FLASH_EraseInitStruct;
    uint32_t sectornb = 0;
    
    // Configure the erase parameters
    FLASH_EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
    FLASH_EraseInitStruct.Sector = FLASH_SECTOR_3;  // Start sector (modify as needed)
    FLASH_EraseInitStruct.NbSectors = 5;            // Number of sectors to erase (modify as needed)
    FLASH_EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
    
    // Perform the erase operation
    if (HAL_FLASHEx_Erase(&FLASH_EraseInitStruct, &sectornb) != HAL_OK)
    {
      printf("Error erasing Flash\n");
      return (1);
    }
    else
    {
      printf("Flash erase completed successfully\n");
    }
  }
  else
  {
    printf("Flash address out of range: 0x%08X\n", FlashAddress);
    return (1);
  }

  return (0);
}

/**
  * @brief  This function writes a data buffer to Flash memory.
  *         The data buffer is assumed to be 32-bit aligned.
  * @note   After writing the data, the content of the Flash memory is checked to ensure the write was successful.
  * @param  FlashAddress: Start address for writing data buffer.
  * @param  Data: Pointer to the data buffer to be written.
  * @param  DataLength: Length of data buffer in 32-bit words.
  * @retval 0: Data successfully written to Flash memory.
  *         1: Error occurred while writing data to Flash memory.
  *         2: Written data in Flash memory does not match expected value.
  */
uint32_t FLASH_If_Write(__IO uint32_t* FlashAddress, uint32_t* Data, uint16_t DataLength) {
    uint32_t i = 0;
    printf("Starting Flash write at address: 0x%08X with length: %d words\n", *FlashAddress, DataLength);

    for (i = 0; (i < DataLength) && (*FlashAddress <= (USER_FLASH_END_ADDRESS-4)); i++) {
        printf("Attempting to write 0x%08X to flash address 0x%08X\n", *(Data + i), *FlashAddress);

        // Ensure the data is 4-byte aligned
        if (((uint32_t)(Data + i) & 0x3) != 0) {
            printf("Alignment Error: FlashAddress=0x%08X, Data=0x%08X\n", *FlashAddress, *(Data + i));
        }

        // Write data to Flash memory using HAL function
        printf("Before HAL_FLASH_Program\n");
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, *FlashAddress, *(Data + i)) == HAL_OK) {
            printf("After HAL_FLASH_Program\n");

            // Verify that the data was written correctly by reading it back
            if (*(uint32_t*)*FlashAddress != *(Data + i)) {
                printf("Verification error: written value 0x%08X does not match expected value 0x%08X\n", *(uint32_t*)*FlashAddress, *(Data + i));
                return(2);
            } else {
                printf("Verification successful for address: 0x%08X\n", *FlashAddress);
            }

            // Increment Flash destination address for the next word
            *FlashAddress += 4;
        } else {
            printf("Error writing to Flash at address: 0x%08X\n", *FlashAddress);
            return (1);
        }
    }

    printf("Flash write completed successfully\n");
    return (0);
}
