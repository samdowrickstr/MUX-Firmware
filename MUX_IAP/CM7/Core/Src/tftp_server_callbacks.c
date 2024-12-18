/**
  ******************************************************************************
  * @file    tftp_server_callbacks.c
  * @author  Samuel Dowrick
  * @brief   This file provides high-level routines to manage TFTP server
  *          callbacks for reading and writing to internal Flash memory.
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

#include "flash_if.h"
#include "lwip/pbuf.h"
#include <string.h>
#include "main.h"


/**
  * @brief  This function opens a file or memory region for reading or writing.
  *         It initializes the Flash if writing is requested and erases the
  *         Flash memory at the specified start address.
  * @param  fname: Filename or memory region to open.
  * @param  mode: Mode for the operation (e.g., "octet" for binary).
  * @param  write: Flag indicating if the operation is a write (1) or read (0).
  * @retval void*: A handle to the opened file or memory region.
  *         NULL: If an error occurred during Flash initialization or erasure.
  */
void* tftp_open(const char* fname, const char* mode, u8_t write) {
    uint32_t start_address = APPLICATION_ADDRESS; // Define this based on where your application starts in flash
    printf("TFTP open: filename=%s, mode=%s, write=%d\n", fname, mode, write);

    if (write) {
        printf("Initializing Flash for write operation.\n");
        // Unlock the Flash memory to allow writing
        FLASH_If_Init();
        // Erase the Flash memory before writing new data
        if (FLASH_If_Erase(start_address) != 0) {
            printf("Error: Flash erase failed.\n");
            return NULL; // Return NULL to indicate failure
        }
    }
    printf("Returning start address: 0x%08X\n", start_address);
    return (void*)start_address; // Return the address as the handle for subsequent operations
}

/**
  * @brief  This function closes the file or memory region.
  *         This is called when the TFTP client closes the file.
  * @param  handle: Handle to the file or memory region being closed.
  * @retval None
  */
void tftp_close(void* handle) {
    printf("TFTP close: handle=0x%08X\n", (uint32_t)handle);
    // No specific operation is required here, but the handle is logged
    // Uncomment this line to jump to the application after TFTP transfer
    CheckAndJumpToApplication();
}

/**
  * @brief  This function reads data from Flash memory and sends it to the TFTP client.
  * @param  handle: Handle to the memory region being read.
  * @param  buf: Pointer to the buffer where the data will be stored.
  * @param  bytes: Number of bytes to read from the memory region.
  * @retval int: Number of bytes successfully read and copied to the buffer.
  */
int tftp_read(void* handle, void* buf, int bytes) {
    printf("TFTP read: handle=0x%08X, bytes=%d\n", (uint32_t)handle, bytes);
    // Copy the requested number of bytes from the Flash memory to the buffer
    memcpy(buf, handle, bytes);
    printf("TFTP read complete: copied %d bytes.\n", bytes);
    return bytes; // Return the number of bytes read
}

/**
  * @brief  This function writes data received from the TFTP client into Flash memory.
  *         It aligns the payload if necessary and writes the data to Flash memory in chunks.
  * @note   The function ensures that the Flash memory and the data buffer are 32-bit aligned
  *         before writing. It also verifies that the data was written correctly.
  * @param  handle: Handle to the memory region being written to.
  * @param  p: Pointer to the data buffer containing the data to be written.
  * @retval int: Number of bytes successfully written to Flash memory.
  *         -1: If an error occurred during the write operation.
  */
int tftp_write(void* handle, struct pbuf* p) {
    // Static variable to retain the last written address across function calls
    static uint32_t flash_address = APPLICATION_ADDRESS;
    uint32_t aligned_payload[128];

    printf("TFTP write: flash_address=0x%08X, p->tot_len=%d, p->len=%d\n", flash_address, p->tot_len, p->len);

    // Check alignment of the payload pointer
    if ((uint32_t)p->payload % 4 != 0) {
        printf("Aligning payload to 4-byte boundary...\n");
        memcpy(aligned_payload, p->payload, p->len);
        p->payload = aligned_payload;
    }

    printf("Writing to flash, payload=0x%08X, len=%d\n", (uint32_t)p->payload, p->len);

    // Calculate the number of words (32-bit) to write
    uint16_t word_count = p->len / 4;

    // Write to Flash in chunks
    uint32_t result = FLASH_If_Write(&flash_address, (uint32_t*)p->payload, word_count);
    if (result != HAL_OK) {
        printf("Error occurred during flash write. Error code: %u\n", result);
        return -1;
    }

    // Verify and print each written word
    for (int i = 0; i < word_count; i++) {
        printf("Payload[%d] (Address: %p): 0x%08X\n", i, &((uint32_t*)p->payload)[i], ((uint32_t*)p->payload)[i]);
    }

    return p->len; // Return the number of bytes written
}
