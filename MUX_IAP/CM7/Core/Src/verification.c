/**
  ******************************************************************************
  * @file    verification.c
  * @author  Samuel Dowrick
  * @brief   This file implements CRC32 and digital signature verification
  *          functions used to validate the integrity of the application.
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
#include "verification.h"
#include "flash_if.h"

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Calculates a CRC32 for a given data block.
  * @param  data: Pointer to the start of the data block.
  * @param  length: Length of the data block in 32-bit words.
  * @retval Calculated CRC32 value.
  */
uint32_t CalculateCRC32(uint32_t* data, uint32_t length) {
    uint32_t crc = 0xFFFFFFFF;
    for (uint32_t i = 0; i < length; i++) {
        uint32_t byte = data[i];
        for (uint8_t j = 0; j < 32; j++) {
            uint32_t mix = (crc ^ byte) & 0x1;
            crc = (crc >> 1) ^ (mix ? 0xEDB88320 : 0);
            byte >>= 1;
        }
    }
    return ~crc;
}

/**
  * @brief  Stores a CRC32 at a specific address in flash memory.
  *         This function writes the CRC32 to the specified address using
  *         the Flash interface.
  * @param  crc: The CRC32 value to be stored.
  * @param  address: The flash memory address where the CRC32 will be stored.
  * @retval None
  */
void StoreCRC32(uint32_t crc, uint32_t address) {
    FLASH_If_Write(&address, &crc, 1);
}

/**
  * @brief  Verifies the CRC32 of the application stored in flash memory.
  *         The function calculates the CRC32 of the application data and
  *         compares it with the stored CRC32 value.
  * @param  start_address: Start address of the application in flash memory.
  * @param  length: Length of the application in 32-bit words.
  * @retval 0: CRC32 is valid.
  *         -1: CRC32 is invalid.
  */
int VerifyCRC32(uint32_t start_address, uint32_t length) {
    uint32_t calculated_crc = CalculateCRC32((uint32_t*)start_address, length);
    uint32_t stored_crc = *(uint32_t*)(start_address + length);

    return (calculated_crc == stored_crc) ? 0 : -1;
}

/**
  * @brief  Verifies the digital signature of the application (if implemented).
  *         This is a placeholder function for actual digital signature
  *         verification logic, which can be added as per requirements.
  * @param  start_address: Start address of the application in flash memory.
  * @param  length: Length of the application in 32-bit words.
  * @param  signature: Pointer to the digital signature.
  * @retval 0: Digital signature is valid.
  *         -1: Digital signature is invalid.
  */
int VerifyDigitalSignature(uint32_t start_address, uint32_t length, uint8_t* signature) {
    // Digital signature verification logic goes here
    return 0;  // Return 0 for success, -1 for failure
}
