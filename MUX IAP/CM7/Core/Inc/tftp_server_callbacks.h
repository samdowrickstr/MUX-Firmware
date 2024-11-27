/**
  ******************************************************************************
  * @file    tftp_server_callbacks.h
  * @author  Samuel Dowrick
  * @brief   Header file for TFTP server callbacks.
  *          This file provides function prototypes for the TFTP server
  *          to handle file operations such as open, read, write, and close.
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

#ifndef __TFTP_SERVER_CALLBACKS_H
#define __TFTP_SERVER_CALLBACKS_H

/* Includes ------------------------------------------------------------------*/
#include "lwip/pbuf.h"  // LwIP's pbuf structure for handling packet buffers
#include <stdint.h>      // Standard integer types

/* Function Prototypes -------------------------------------------------------*/

/**
  * @brief  TFTP callback to open a file or memory region for reading or writing.
  * @param  fname: The name of the file to be opened.
  * @param  mode: The mode in which the file should be opened ("r" for read, "w" for write).
  * @param  write: Flag indicating if the file is being opened for writing (1) or reading (0).
  * @retval void*: A pointer to the file handle or memory region.
  */
void* tftp_open(const char* fname, const char* mode, u8_t write);

/**
  * @brief  TFTP callback to close the file or memory region.
  * @param  handle: The handle to the file or memory region that needs to be closed.
  * @retval None
  */
void tftp_close(void* handle);

/**
  * @brief  TFTP callback to read data from the memory.
  *         This function is used during file transfer from the device to a TFTP client.
  * @param  handle: The handle to the memory region to read from.
  * @param  buf: The buffer where the read data should be stored.
  * @param  bytes: The number of bytes to read.
  * @retval int: The number of bytes successfully read.
  */
int tftp_read(void* handle, void* buf, int bytes);

/**
  * @brief  TFTP callback to write data to flash memory.
  * @param  handle: The handle to the flash memory region to write to.
  * @param  p: The pbuf structure containing the data to be written.
  * @retval int: The number of bytes successfully written.
  */
int tftp_write(void* handle, struct pbuf* p);

#endif /* __TFTP_SERVER_CALLBACKS_H */
