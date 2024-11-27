/**
  ******************************************************************************
  * @file    verification.h
  * @author  Samuel Dowrick
  * @brief   Header file for checksum and digital signature verification.
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

#ifndef __VERIFICATION_H
#define __VERIFICATION_H

#include <stdint.h>

/* Function Prototypes */
uint32_t CalculateCRC32(uint32_t* data, uint32_t length);
void StoreCRC32(uint32_t crc, uint32_t address);
int VerifyCRC32(uint32_t start_address, uint32_t length);
int VerifyDigitalSignature(uint32_t start_address, uint32_t length, uint8_t* signature);

#endif /* __VERIFICATION_H */
