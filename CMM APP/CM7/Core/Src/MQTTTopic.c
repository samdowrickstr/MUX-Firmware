/**
  ******************************************************************************
  * @file    MQTTTopic.c
  * @author  Samuel Dowrick
  * @brief   This file provides functions to handle MQTT topic-specific logic,
  *          including subscribing to topics and processing messages received
  *          on those topics.
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

#include "MQTTTopic.h"
#include "MQTTClient.h"
#include "lwip/apps/mqtt.h"
#include "main.h"  // Include the header with the slot variable
#include <string.h>
#include <stdio.h>
#include "serial_comm.h"

/**
  * @brief  Handle the trigger state message.
  * @param  message: The payload of the message received.
  * @param  len: The length of the message.
  * @retval None
  */
void handle_trigger_state(const char *message, size_t len) {
    LWIP_PLATFORM_DIAG(("Handling trigger state: %.*s\n", (int)len, message));
    // Add logic here
}

/**
  * @brief  Handle the serial protocol message.
  * @param  message: The payload of the message received.
  * @param  len: The length of the message.
  * @retval None
  */
void handle_serial_protocol(const char *message, size_t len) {
    LWIP_PLATFORM_DIAG(("Handling serial protocol: %.*s\n", (int)len, message));

    // Compare message with supported protocols
    if (strncmp(message, "RS232", len) == 0) {
        Switch_RS_Mode(RS232_MODE);
        LWIP_PLATFORM_DIAG(("Switched to RS232 mode\n"));
    } else if (strncmp(message, "RS485", len) == 0) {
        Switch_RS_Mode(RS485_MODE);
        LWIP_PLATFORM_DIAG(("Switched to RS485 mode\n"));
    } else {
        LWIP_PLATFORM_DIAG(("Unknown serial protocol: %.*s\n", (int)len, message));
    }
}


/**
  * @brief  Handle the serial baudrate message.
  * @param  message: The payload of the message received.
  * @param  len: The length of the message.
  * @retval None
  */
void handle_serial_baudrate(const char *message, size_t len) {
    LWIP_PLATFORM_DIAG(("Handling serial baudrate: %.*s\n", (int)len, message));

    // Create a temporary buffer to store the message and ensure null-termination
    char baudrate_str[16];  // Adjust the size as necessary for your use case
    if (len < sizeof(baudrate_str)) {
        memcpy(baudrate_str, message, len);
        baudrate_str[len] = '\0';  // Null-terminate the string
    } else {
        LWIP_PLATFORM_DIAG(("Baudrate message too long\n"));
        return;
    }

    // Convert message to an integer (baudrate)
    int new_baudrate = atoi(baudrate_str);

    if (new_baudrate > 0) {
        // Update the baud rate dynamically for USART2 (RS232)
        huart2.Init.BaudRate = new_baudrate;
        if (HAL_UART_Init(&huart2) != HAL_OK) {
            LWIP_PLATFORM_DIAG(("Failed to set baudrate for USART2 (RS232): %d\n", new_baudrate));
        } else {
            LWIP_PLATFORM_DIAG(("Baudrate for USART2 (RS232) set to %d\n", new_baudrate));
        }

        // Update the baud rate dynamically for USART3 (RS485)
        huart3.Init.BaudRate = new_baudrate;
        if (HAL_UART_Init(&huart3) != HAL_OK) {
            LWIP_PLATFORM_DIAG(("Failed to set baudrate for USART3 (RS485): %d\n", new_baudrate));
        } else {
            LWIP_PLATFORM_DIAG(("Baudrate for USART3 (RS485) set to %d\n", new_baudrate));
        }

        // Adjust the buffer and UDP packet size based on the new baud rate
//        update_buffer_and_udp_size(new_baudrate);

    } else {
        LWIP_PLATFORM_DIAG(("Invalid baudrate: %.*s\n", (int)len, message));
    }
}




/**
  * @brief  Handle the serial flow control message.
  * @param  message: The payload of the message received.
  * @param  len: The length of the message.
  * @retval None
  */
void handle_serial_flowcontrol(const char *message, size_t len) {
    LWIP_PLATFORM_DIAG(("Handling serial flow control: %.*s\n", (int)len, message));
    // Add logic here
}

/**
  * @brief  Handle the serial data bits message.
  * @param  message: The payload of the message received.
  * @param  len: The length of the message.
  * @retval None
  */
void handle_serial_databits(const char *message, size_t len) {
    LWIP_PLATFORM_DIAG(("Handling serial data bits: %.*s\n", (int)len, message));

    if (strncmp(message, "7", len) == 0) {
        huart2.Init.WordLength = UART_WORDLENGTH_7B;
    } else if (strncmp(message, "8", len) == 0) {
        huart2.Init.WordLength = UART_WORDLENGTH_8B;
    } else if (strncmp(message, "9", len) == 0) {
        huart2.Init.WordLength = UART_WORDLENGTH_9B;
    } else {
        LWIP_PLATFORM_DIAG(("Unknown data bits: %.*s\n", (int)len, message));
        return;
    }

    if (HAL_UART_Init(&huart2) != HAL_OK) {
        LWIP_PLATFORM_DIAG(("Failed to set data bits\n"));
    } else {
        LWIP_PLATFORM_DIAG(("Data bits set successfully\n"));
    }
}


/**
  * @brief  Handle the serial parity message.
  * @param  message: The payload of the message received.
  * @param  len: The length of the message.
  * @retval None
  */
void handle_serial_parity(const char *message, size_t len) {
    LWIP_PLATFORM_DIAG(("Handling serial parity: %.*s\n", (int)len, message));

    if (strncmp(message, "NONE", len) == 0) {
        huart2.Init.Parity = UART_PARITY_NONE;
    } else if (strncmp(message, "EVEN", len) == 0) {
        huart2.Init.Parity = UART_PARITY_EVEN;
    } else if (strncmp(message, "ODD", len) == 0) {
        huart2.Init.Parity = UART_PARITY_ODD;
    } else {
        LWIP_PLATFORM_DIAG(("Unknown parity: %.*s\n", (int)len, message));
        return;
    }

    if (HAL_UART_Init(&huart2) != HAL_OK) {
        LWIP_PLATFORM_DIAG(("Failed to set parity\n"));
    } else {
        LWIP_PLATFORM_DIAG(("Parity set successfully\n"));
    }
}


/**
  * @brief  Handle the serial stop bits message.
  * @param  message: The payload of the message received.
  * @param  len: The length of the message.
  * @retval None
  */
void handle_serial_stopbits(const char *message, size_t len) {
    LWIP_PLATFORM_DIAG(("Handling serial stop bits: %.*s\n", (int)len, message));

    if (strncmp(message, "0.5", len) == 0) {
        huart2.Init.StopBits = UART_STOPBITS_0_5;
    } else if (strncmp(message, "1", len) == 0) {
        huart2.Init.StopBits = UART_STOPBITS_1;
    } else if (strncmp(message, "1.5", len) == 0) {
        huart2.Init.StopBits = UART_STOPBITS_1_5;
    } else if (strncmp(message, "2", len) == 0) {
        huart2.Init.StopBits = UART_STOPBITS_2;
    } else {
        LWIP_PLATFORM_DIAG(("Unknown stop bits: %.*s\n", (int)len, message));
        return;
    }

    if (HAL_UART_Init(&huart2) != HAL_OK) {
        LWIP_PLATFORM_DIAG(("Failed to set stop bits\n"));
    } else {
        LWIP_PLATFORM_DIAG(("Stop bits set successfully\n"));
    }
}


/**
  * @brief  Handle the Ethernet speed message.
  * @param  message: The payload of the message received.
  * @param  len: The length of the message.
  * @retval None
  */
void handle_ethernet_speed(const char *message, size_t len) {
    LWIP_PLATFORM_DIAG(("Handling Ethernet speed: %.*s\n", (int)len, message));
    // Add logic here
}

/**
  ******************************************************************************
  * @brief  Handles the "enable IAP" (In-Application Programming) message by
  *         jumping to the IAP code located at address 0x08000000.
  * @param  message: The received MQTT message indicating the IAP action.
  * @param  len: Length of the received message.
  * @retval None
  */
void handle_enable_iap(const char *message, size_t len) {
    // Print the received message for debugging
    LWIP_PLATFORM_DIAG(("Handling enable IAP: %.*s\n", (int)len, message));

    if (strncmp(message, "ENABLE", len) == 0) {
        // Write the IAP flag to SRAM4
        volatile uint32_t *iap_flag_address = (uint32_t *)IAP_FLAG_ADDRESS;
        *iap_flag_address = IAP_FLAG_VALUE;
        LWIP_PLATFORM_DIAG(("IAP flag set at address 0x%08X with value 0x%08X\n", (unsigned int)IAP_FLAG_ADDRESS, IAP_FLAG_VALUE));

        // Add a small delay to ensure the flag is written
        HAL_Delay(500);

        // Reset the system to jump to IAP
        HAL_NVIC_SystemReset();
    } else {
        LWIP_PLATFORM_DIAG(("Unknown IAP command: %.*s\n", (int)len, message));
    }
}

/**
  * @brief  Handle the Ethernet link status message.
  * @param  message: The payload of the message received.
  * @param  len: The length of the message.
  * @retval None
  */
void handle_status_ethernet_link(const char *message, size_t len) {
    LWIP_PLATFORM_DIAG(("Handling Ethernet link status: %.*s\n", (int)len, message));
    // Add logic here
}

/**
  * @brief  Handle the Ethernet activity status message.
  * @param  message: The payload of the message received.
  * @param  len: The length of the message.
  * @retval None
  */
void handle_status_ethernet_activity(const char *message, size_t len) {
    LWIP_PLATFORM_DIAG(("Handling Ethernet activity status: %.*s\n", (int)len, message));
    // Add logic here
}

/**
  * @brief  Handle the slot number status message.
  * @param  message: The payload of the message received.
  * @param  len: The length of the message.
  * @retval None
  */
void handle_status_slot_number(const char *message, size_t len) {
    LWIP_PLATFORM_DIAG(("Handling slot number status: %.*s\n", (int)len, message));
    // Add logic here
}

void handle_incoming_topic_data(const char *topic, const char *data, size_t len) {
    char topic_buffer[128];  // Buffer to hold the dynamically generated topic strings

    snprintf(topic_buffer, sizeof(topic_buffer), "CMB/slot%d/control/trigger/state", slot);
    if (strcmp(topic, topic_buffer) == 0) {
        handle_trigger_state(data, len);
        return;
    }

    snprintf(topic_buffer, sizeof(topic_buffer), "CMB/slot%d/control/serial/protocol", slot);
    if (strcmp(topic, topic_buffer) == 0) {
        handle_serial_protocol(data, len);
        return;
    }

    snprintf(topic_buffer, sizeof(topic_buffer), "CMB/slot%d/control/serial/baudrate", slot);
    if (strcmp(topic, topic_buffer) == 0) {
        handle_serial_baudrate(data, len);
        return;
    }

    snprintf(topic_buffer, sizeof(topic_buffer), "CMB/slot%d/control/serial/flowcontrol", slot);
    if (strcmp(topic, topic_buffer) == 0) {
        handle_serial_flowcontrol(data, len);
        return;
    }

    snprintf(topic_buffer, sizeof(topic_buffer), "CMB/slot%d/control/serial/databits", slot);
    if (strcmp(topic, topic_buffer) == 0) {
        handle_serial_databits(data, len);
        return;
    }

    snprintf(topic_buffer, sizeof(topic_buffer), "CMB/slot%d/control/serial/parity", slot);
    if (strcmp(topic, topic_buffer) == 0) {
        handle_serial_parity(data, len);
        return;
    }

    snprintf(topic_buffer, sizeof(topic_buffer), "CMB/slot%d/control/serial/stopbits", slot);
    if (strcmp(topic, topic_buffer) == 0) {
        handle_serial_stopbits(data, len);
        return;
    }

    snprintf(topic_buffer, sizeof(topic_buffer), "CMB/slot%d/control/ethernet/speed", slot);
    if (strcmp(topic, topic_buffer) == 0) {
        handle_ethernet_speed(data, len);
        return;
    }

    snprintf(topic_buffer, sizeof(topic_buffer), "CMB/slot%d/control/iap", slot);
    if (strcmp(topic, topic_buffer) == 0) {
        handle_enable_iap(data, len);
        return;
    }

    snprintf(topic_buffer, sizeof(topic_buffer), "CMB/slot%d/status/ethernet/link", slot);
    if (strcmp(topic, topic_buffer) == 0) {
        handle_status_ethernet_link(data, len);
        return;
    }

    snprintf(topic_buffer, sizeof(topic_buffer), "CMB/slot%d/status/ethernet/activity", slot);
    if (strcmp(topic, topic_buffer) == 0) {
        handle_status_ethernet_activity(data, len);
        return;
    }

    LWIP_PLATFORM_DIAG(("Unknown topic: %s\n", topic));
}


/**
  * @brief  Subscribes to all topics needed for the MQTT client.
  * @param  client: Pointer to the MQTT client.
  * @param  client_info: Pointer to the client-specific information.
  * @param  slot: Slot number for topic subscription.
  * @retval None
  */
void mqtt_subscribe_to_topics(mqtt_client_t *client, const struct mqtt_connect_client_info_t* client_info, int slot) {
    err_t err;
    char topic[128];

    // List of all the topics to subscribe to
    const char* topics[] = {
        "control/trigger/state",
        "control/serial/protocol",
        "control/serial/baudrate",
        "control/serial/flowcontrol",
        "control/serial/databits",
        "control/serial/parity",
        "control/serial/stopbits",
        "control/ethernet/speed",
        "control/iap",
        "status/ethernet/link",
        "status/ethernet/activity",
    };

    // Subscribe to each topic in the list with QoS 1
    for (size_t i = 0; i < sizeof(topics) / sizeof(topics[0]); i++) {
        snprintf(topic, sizeof(topic), "CMB/slot%d/%s", slot, topics[i]); // Build the topic string with slot
        err = mqtt_sub_unsub(client, topic, 1, mqtt_request_cb, LWIP_CONST_CAST(void*, client_info), 1);
        if (err != ERR_OK) {
            LWIP_PLATFORM_DIAG(("Failed to subscribe to %s, error %d\n", topic, err));
        } else {
            LWIP_PLATFORM_DIAG(("Successfully subscribed to %s\n", topic));
        }
    }
}
