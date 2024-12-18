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
//#include "serial_comm.h"
#include "cJSON.h"  // Include cJSON library for JSON handling
#include "PMB_comm.h"  // Replace with the actual header file name where the functions are declared

typedef  void (*pFunction)(void);
void CheckAndJumpToApplication(void);
void configureSMIRegisters(void);

__attribute__((section(".noinit"))) volatile uint32_t iap_flag;

typedef struct {
    uint8_t state;          // 1 for On, 0 for Off
    float current_limit;    // Current limit with up to 2 decimal points
} ACPWR_Channel_Config;

typedef struct {
    uint8_t state;          // 1 for On, 0 for Off
    float current_limit;    // Current limit with up to 2 decimal points
} DCPWR_Channel_Config;

// Example arrays to hold configurations
ACPWR_Channel_Config acpwr_configs[2]; // Channels 0 and 1
DCPWR_Channel_Config dc_pwr_configs[12][2]; // Slots 0-11, Channels 0-1

// Initialize configurations as needed
void publish_dc_switch_current(int slot_num, int channel_num, float current_a) {
    char topic[128];
    char payload[32];
    // Clamp negative currents to 0A
    if (current_a < 0.02f) {
        current_a = 0.0f;
    }

    // Format the MQTT topic
    snprintf(topic, sizeof(topic), "DCPWR/slot%d/channel%d/status/current_consumption", slot_num, channel_num);

    // Format the payload to include the current in amperes with 2 decimal places
    snprintf(payload, sizeof(payload), "%.2f", current_a);

    // Publish the message
    err_t err = mqtt_publish(mqtt_client, topic, payload, strlen(payload), 1, 0, NULL, NULL);
    if (err != ERR_OK) {
//        LWIP_PLATFORM_DIAG(("Failed to publish to %s, error %d\n", topic, err));
    } else {
//        LWIP_PLATFORM_DIAG(("Published current consumption to %s: %s A\n", topic, payload));
    }
}
/**
  * @brief  Publishes the configuration of comms and power channels to MQTT.
  * @param  client: Pointer to the MQTT client.
  * @param  slot: Slot number for dynamic topic creation.
  * @retval None
  */

void publish_channel_config(mqtt_client_t *client, int slot) {
    char topic[128];
    snprintf(topic, sizeof(topic), "CMB/slot%d/config/channels_info", slot);

    // Create JSON root object
    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "connector_count", 10);

    // Create connectors object
    cJSON *connectors = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "connectors", connectors);

    // Connector 1
    cJSON *connector_1 = cJSON_CreateObject();
    cJSON_AddItemToObject(connectors, "connector 1", connector_1);

    cJSON *comms_1 = cJSON_CreateObject();
    cJSON_AddStringToObject(comms_1, "function", "GigE");
    cJSON_AddNumberToObject(comms_1, "pcb_port", 9);
    cJSON_AddItemToObject(connector_1, "comms", comms_1);

    cJSON *power_1 = cJSON_CreateObject();
    cJSON_AddItemToObject(connector_1, "power", power_1);
    cJSON *power_channel_1_1 = cJSON_CreateObject();
    cJSON_AddStringToObject(power_channel_1_1, "type", "48V");
    cJSON_AddNumberToObject(power_channel_1_1, "slot", 2);
    cJSON_AddNumberToObject(power_channel_1_1, "channel", 0);
    cJSON_AddItemToObject(power_1, "channel 1", power_channel_1_1);

    cJSON *power_channel_1_2 = cJSON_CreateObject();
    cJSON_AddStringToObject(power_channel_1_2, "type", "48V High Power");
    cJSON_AddNumberToObject(power_channel_1_2, "slot", 0);
    cJSON_AddNumberToObject(power_channel_1_2, "channel", 0);
    cJSON_AddItemToObject(power_1, "channel 2", power_channel_1_2);

    // Connector 2
    cJSON *connector_2 = cJSON_CreateObject();
    cJSON_AddItemToObject(connectors, "connector 2", connector_2);

    cJSON *comms_2 = cJSON_CreateObject();
    cJSON_AddStringToObject(comms_2, "function", "GigE");
    cJSON_AddNumberToObject(comms_2, "pcb_port", 10);
    cJSON_AddItemToObject(connector_2, "comms", comms_2);

    cJSON *power_2 = cJSON_CreateObject();
    cJSON_AddItemToObject(connector_2, "power", power_2);
    cJSON *power_channel_2_1 = cJSON_CreateObject();
    cJSON_AddStringToObject(power_channel_2_1, "type", "24V");
    cJSON_AddNumberToObject(power_channel_2_1, "slot", 2);
    cJSON_AddNumberToObject(power_channel_2_1, "channel", 1);
    cJSON_AddItemToObject(power_2, "channel 1", power_channel_2_1);

    cJSON *power_channel_2_2 = cJSON_CreateObject();
    cJSON_AddStringToObject(power_channel_2_2, "type", "48V High Power");
    cJSON_AddNumberToObject(power_channel_2_2, "slot", 0);
    cJSON_AddNumberToObject(power_channel_2_2, "channel", 1);
    cJSON_AddItemToObject(power_2, "channel 2", power_channel_2_2);

    // Connector 3
    cJSON *connector_3 = cJSON_CreateObject();
    cJSON_AddItemToObject(connectors, "connector 3", connector_3);

    cJSON *comms_3 = cJSON_CreateObject();
    cJSON_AddStringToObject(comms_3, "function", "GigE");
    cJSON_AddNumberToObject(comms_3, "pcb_port", 11);
    cJSON_AddItemToObject(connector_3, "comms", comms_3);

    cJSON *power_3 = cJSON_CreateObject();
    cJSON_AddItemToObject(connector_3, "power", power_3);
    cJSON *power_channel_3_1 = cJSON_CreateObject();
    cJSON_AddStringToObject(power_channel_3_1, "type", "24V");
    cJSON_AddNumberToObject(power_channel_3_1, "slot", 3);
    cJSON_AddNumberToObject(power_channel_3_1, "channel", 1);
    cJSON_AddItemToObject(power_3, "channel 1", power_channel_3_1);

    cJSON *power_channel_3_2 = cJSON_CreateObject();
    cJSON_AddStringToObject(power_channel_3_2, "type", "48V High Power");
    cJSON_AddNumberToObject(power_channel_3_2, "slot", 1);
    cJSON_AddNumberToObject(power_channel_3_2, "channel", 0);
    cJSON_AddItemToObject(power_3, "channel 2", power_channel_3_2);

    // Connector 4
    cJSON *connector_4 = cJSON_CreateObject();
    cJSON_AddItemToObject(connectors, "connector 4", connector_4);

    cJSON *comms_4 = cJSON_CreateObject();
    cJSON_AddStringToObject(comms_4, "function", "FastE, RS232, RS485, Trig In, Trig Out");
    cJSON_AddNumberToObject(comms_4, "pcb_port", 7);
    cJSON_AddItemToObject(connector_4, "comms", comms_4);

    cJSON *power_4 = cJSON_CreateObject();
    cJSON_AddItemToObject(connector_4, "power", power_4);
    cJSON *power_channel_4_1 = cJSON_CreateObject();
    cJSON_AddStringToObject(power_channel_4_1, "type", "24V");
    cJSON_AddNumberToObject(power_channel_4_1, "slot", 3);
    cJSON_AddNumberToObject(power_channel_4_1, "channel", 1);
    cJSON_AddItemToObject(power_4, "channel 1", power_channel_4_1);

    cJSON *power_channel_4_2 = cJSON_CreateObject();
    cJSON_AddStringToObject(power_channel_4_2, "type", "48V");
    cJSON_AddNumberToObject(power_channel_4_2, "slot", 4);
    cJSON_AddNumberToObject(power_channel_4_2, "channel", 0);
    cJSON_AddItemToObject(power_4, "channel 2", power_channel_4_2);

    // Connector 5
    cJSON *connector_5 = cJSON_CreateObject();
    cJSON_AddItemToObject(connectors, "connector 5", connector_5);

    cJSON *comms_5 = cJSON_CreateObject();
    cJSON_AddStringToObject(comms_5, "function", "FastE, RS232, RS485, Trig In, Trig Out");
    cJSON_AddNumberToObject(comms_5, "pcb_port", 2);
    cJSON_AddItemToObject(connector_5, "comms", comms_5);

    cJSON *power_5 = cJSON_CreateObject();
    cJSON_AddItemToObject(connector_5, "power", power_5);
    cJSON *power_channel_5_1 = cJSON_CreateObject();
    cJSON_AddStringToObject(power_channel_5_1, "type", "24V");
    cJSON_AddNumberToObject(power_channel_5_1, "slot", 9);
    cJSON_AddNumberToObject(power_channel_5_1, "channel", 0);
    cJSON_AddItemToObject(power_5, "channel 1", power_channel_5_1);

    cJSON *power_channel_5_2 = cJSON_CreateObject();
    cJSON_AddStringToObject(power_channel_5_2, "type", "48V");
    cJSON_AddNumberToObject(power_channel_5_2, "slot", 11);
    cJSON_AddNumberToObject(power_channel_5_2, "channel", 0);
    cJSON_AddItemToObject(power_5, "channel 2", power_channel_5_2);

    // Connector 6
    cJSON *connector_6 = cJSON_CreateObject();
    cJSON_AddItemToObject(connectors, "connector 6", connector_6);

    cJSON *comms_6 = cJSON_CreateObject();
    cJSON_AddStringToObject(comms_6, "function", "FastE, RS232, RS485, Trig In, Trig Out");
    cJSON_AddNumberToObject(comms_6, "pcb_port", 3);
    cJSON_AddItemToObject(connector_6, "comms", comms_6);

    cJSON *power_6 = cJSON_CreateObject();
    cJSON_AddItemToObject(connector_6, "power", power_6);
    cJSON *power_channel_6_1 = cJSON_CreateObject();
    cJSON_AddStringToObject(power_channel_6_1, "type", "24V");
    cJSON_AddNumberToObject(power_channel_6_1, "slot", 9);
    cJSON_AddNumberToObject(power_channel_6_1, "channel", 1);
    cJSON_AddItemToObject(power_6, "channel 1", power_channel_6_1);

    cJSON *power_channel_6_2 = cJSON_CreateObject();
    cJSON_AddStringToObject(power_channel_6_2, "type", "48V");
    cJSON_AddNumberToObject(power_channel_6_2, "slot", 11);
    cJSON_AddNumberToObject(power_channel_6_2, "channel", 1);
    cJSON_AddItemToObject(power_6, "channel 2", power_channel_6_2);

    // Connector 7
    cJSON *connector_7 = cJSON_CreateObject();
    cJSON_AddItemToObject(connectors, "connector 7", connector_7);

    cJSON *comms_7 = cJSON_CreateObject();
    cJSON_AddStringToObject(comms_7, "function", "FastE, RS232, RS485, Trig In, Trig Out");
    cJSON_AddNumberToObject(comms_7, "pcb_port", 4);
    cJSON_AddItemToObject(connector_7, "comms", comms_7);

    cJSON *power_7 = cJSON_CreateObject();
    cJSON_AddItemToObject(connector_7, "power", power_7);
    cJSON *power_channel_7_1 = cJSON_CreateObject();
    cJSON_AddStringToObject(power_channel_7_1, "type", "24V");
    cJSON_AddNumberToObject(power_channel_7_1, "slot", 8);
    cJSON_AddNumberToObject(power_channel_7_1, "channel", 0);
    cJSON_AddItemToObject(power_7, "channel 1", power_channel_7_1);

    cJSON *power_channel_7_2 = cJSON_CreateObject();
    cJSON_AddStringToObject(power_channel_7_2, "type", "48V");
    cJSON_AddNumberToObject(power_channel_7_2, "slot", 10);
    cJSON_AddNumberToObject(power_channel_7_2, "channel", 0);
    cJSON_AddItemToObject(power_7, "channel 2", power_channel_7_2);

    // Connector 8
    cJSON *connector_8 = cJSON_CreateObject();
    cJSON_AddItemToObject(connectors, "connector 8", connector_8);

    cJSON *comms_8 = cJSON_CreateObject();
    cJSON_AddStringToObject(comms_8, "function", "FastE, RS232, RS485, Trig In, Trig Out");
    cJSON_AddNumberToObject(comms_8, "pcb_port", 8);
    cJSON_AddItemToObject(connector_8, "comms", comms_8);

    cJSON *power_8 = cJSON_CreateObject();
    cJSON_AddItemToObject(connector_8, "power", power_8);
    cJSON *power_channel_8_1 = cJSON_CreateObject();
    cJSON_AddStringToObject(power_channel_8_1, "type", "24V");
    cJSON_AddNumberToObject(power_channel_8_1, "slot", 5);
    cJSON_AddNumberToObject(power_channel_8_1, "channel", 1);
    cJSON_AddItemToObject(power_8, "channel 1", power_channel_8_1);

    cJSON *power_channel_8_2 = cJSON_CreateObject();
    cJSON_AddStringToObject(power_channel_8_2, "type", "48V");
    cJSON_AddNumberToObject(power_channel_8_2, "slot", 10);
    cJSON_AddNumberToObject(power_channel_8_2, "channel", 1);
    cJSON_AddItemToObject(power_8, "channel 2", power_channel_8_2);

    // Connector 9
    cJSON *connector_9 = cJSON_CreateObject();
    cJSON_AddItemToObject(connectors, "connector 9", connector_9);

    cJSON *comms_9 = cJSON_CreateObject();
    cJSON_AddStringToObject(comms_9, "function", "FastE, RS232, RS485, Trig In, Trig Out");
    cJSON_AddNumberToObject(comms_9, "pcb_port", 6);
    cJSON_AddItemToObject(connector_9, "comms", comms_9);

    cJSON *power_9 = cJSON_CreateObject();
    cJSON_AddItemToObject(connector_9, "power", power_9);
    cJSON *power_channel_9_1 = cJSON_CreateObject();
    cJSON_AddStringToObject(power_channel_9_1, "type", "24V");
    cJSON_AddNumberToObject(power_channel_9_1, "slot", 5);
    cJSON_AddNumberToObject(power_channel_9_1, "channel", 0);
    cJSON_AddItemToObject(power_9, "channel 1", power_channel_9_1);

    cJSON *power_channel_9_2 = cJSON_CreateObject();
    cJSON_AddStringToObject(power_channel_9_2, "type", "48V");
    cJSON_AddNumberToObject(power_channel_9_2, "slot", 4);
    cJSON_AddNumberToObject(power_channel_9_2, "channel", 1);
    cJSON_AddItemToObject(power_9, "channel 2", power_channel_9_2);

    // Connector AC Out
    cJSON *connector_ac_out = cJSON_CreateObject();
    cJSON_AddItemToObject(connectors, "connector AC Out", connector_ac_out);

    cJSON *comms_ac = cJSON_CreateObject();
    cJSON_AddStringToObject(comms_ac, "function", "RS232, RS485");
    cJSON_AddNumberToObject(comms_ac, "pcb_port", 1);
    cJSON_AddItemToObject(connector_ac_out, "comms", comms_ac);

    cJSON *power_ac = cJSON_CreateObject();
    cJSON_AddItemToObject(connector_ac_out, "power", power_ac);
    cJSON *power_channel_ac_1 = cJSON_CreateObject();
    cJSON_AddStringToObject(power_channel_ac_1, "type", "Vac");
    cJSON_AddNumberToObject(power_channel_ac_1, "channel", 1);
    cJSON_AddItemToObject(power_ac, "channel 1", power_channel_ac_1);

    // Convert JSON object to string
    char *json_string = cJSON_PrintUnformatted(root);
    if (json_string == NULL) {
//        LWIP_PLATFORM_DIAG(("Failed to create JSON string\n"));
        cJSON_Delete(root);
        return;
    }

    // Publish JSON data with retain flag set to 1
    err_t err = mqtt_publish(client, topic, json_string, strlen(json_string), 1, 1, mqtt_request_cb, NULL);
    if (err != ERR_OK) {
//        LWIP_PLATFORM_DIAG(("Failed to publish configuration, error %d\n", err));
    } else {
//        LWIP_PLATFORM_DIAG(("Published configuration to topic %s\n", topic));
    }

    // Clean up
    cJSON_free(json_string);
    cJSON_Delete(root);
}


/**
  * @brief  Handle the trigger state message.
  * @param  message: The payload of the message received.
  * @param  len: The length of the message.
  * @retval None
  */
void handle_trigger_state(const char *message, size_t len) {
    LWIP_PLATFORM_DIAG(("Handling trigger state: %.*s\n", (int)len, message));
    // Check if message contains "On" or "Off" to set Trigge_Switch
    if (strstr(message, "On") != NULL) {
    	Trigger_Switch = 1;
//        LWIP_PLATFORM_DIAG(("Trigger_Switch set to 1 (On)\n"));
    } else if (strstr(message, "Off") != NULL) {
    	Trigger_Switch = 0;
//        LWIP_PLATFORM_DIAG(("Trigger_Switch set to 0 (Off)\n"));
    }
}

/**
  * @brief  Handle the serial protocol message.
  * @param  message: The payload of the message received.
  * @param  len: The length of the message.
  * @retval None
  */
void handle_serial_protocol(const char *message, size_t len) {
//    LWIP_PLATFORM_DIAG(("Handling serial protocol: %.*s\n", (int)len, message));

    // Compare message with supported protocols
    if (strncmp(message, "RS232", len) == 0) {
        Switch_RS_Mode(RS232_MODE);
//        LWIP_PLATFORM_DIAG(("Switched to RS232 mode\n"));
    } else if (strncmp(message, "RS485", len) == 0) {
        Switch_RS_Mode(RS485_MODE);
//        LWIP_PLATFORM_DIAG(("Switched to RS485 mode\n"));
    } else {
//        LWIP_PLATFORM_DIAG(("Unknown serial protocol: %.*s\n", (int)len, message));
    }
}


/**
  * @brief  Handle the serial baudrate message.
  * @param  message: The payload of the message received.
  * @param  len: The length of the message.
  * @retval None
  */
void handle_serial_baudrate(const char *message, size_t len) {
//    LWIP_PLATFORM_DIAG(("Handling serial baudrate: %.*s\n", (int)len, message));

    // Create a temporary buffer to store the message and ensure null-termination
    char baudrate_str[16];  // Adjust the size as necessary for your use case
    if (len < sizeof(baudrate_str)) {
        memcpy(baudrate_str, message, len);
        baudrate_str[len] = '\0';  // Null-terminate the string
    } else {
//        LWIP_PLATFORM_DIAG(("Baudrate message too long\n"));
        return;
    }

    // Convert message to an integer (baudrate)
    int new_baudrate = atoi(baudrate_str);

    if (new_baudrate > 0) {
        // Update the baud rate dynamically for USART2 (RS232)
        huart2.Init.BaudRate = new_baudrate;
        if (HAL_UART_Init(&huart2) != HAL_OK) {
//            LWIP_PLATFORM_DIAG(("Failed to set baudrate for USART2 (RS232): %d\n", new_baudrate));
        } else {
//            LWIP_PLATFORM_DIAG(("Baudrate for USART2 (RS232) set to %d\n", new_baudrate));
        }

        // Update the baud rate dynamically for USART3 (RS485)
        huart3.Init.BaudRate = new_baudrate;
        if (HAL_UART_Init(&huart3) != HAL_OK) {
//            LWIP_PLATFORM_DIAG(("Failed to set baudrate for USART3 (RS485): %d\n", new_baudrate));
        } else {
//            LWIP_PLATFORM_DIAG(("Baudrate for USART3 (RS485) set to %d\n", new_baudrate));
        }

        // Adjust the buffer and UDP packet size based on the new baud rate
//        update_buffer_and_udp_size(new_baudrate);

    } else {
//        LWIP_PLATFORM_DIAG(("Invalid baudrate: %.*s\n", (int)len, message));
    }
}




/**
  * @brief  Handle the serial flow control message.
  * @param  message: The payload of the message received.
  * @param  len: The length of the message.
  * @retval None
  */
void handle_serial_flowcontrol(const char *message, size_t len) {
//    LWIP_PLATFORM_DIAG(("Handling serial flow control: %.*s\n", (int)len, message));
    // Add logic here
}

/**
  * @brief  Handle the serial data bits message.
  * @param  message: The payload of the message received.
  * @param  len: The length of the message.
  * @retval None
  */
void handle_serial_databits(const char *message, size_t len) {
//    LWIP_PLATFORM_DIAG(("Handling serial data bits: %.*s\n", (int)len, message));

    if (strncmp(message, "7", len) == 0) {
        huart2.Init.WordLength = UART_WORDLENGTH_7B;
    } else if (strncmp(message, "8", len) == 0) {
        huart2.Init.WordLength = UART_WORDLENGTH_8B;
    } else if (strncmp(message, "9", len) == 0) {
        huart2.Init.WordLength = UART_WORDLENGTH_9B;
    } else {
//        LWIP_PLATFORM_DIAG(("Unknown data bits: %.*s\n", (int)len, message));
        return;
    }

    if (HAL_UART_Init(&huart2) != HAL_OK) {
//        LWIP_PLATFORM_DIAG(("Failed to set data bits\n"));
    } else {
//        LWIP_PLATFORM_DIAG(("Data bits set successfully\n"));
    }
}


/**
  * @brief  Handle the serial parity message.
  * @param  message: The payload of the message received.
  * @param  len: The length of the message.
  * @retval None
  */
void handle_serial_parity(const char *message, size_t len) {
//    LWIP_PLATFORM_DIAG(("Handling serial parity: %.*s\n", (int)len, message));

    if (strncmp(message, "NONE", len) == 0) {
        huart2.Init.Parity = UART_PARITY_NONE;
    } else if (strncmp(message, "EVEN", len) == 0) {
        huart2.Init.Parity = UART_PARITY_EVEN;
    } else if (strncmp(message, "ODD", len) == 0) {
        huart2.Init.Parity = UART_PARITY_ODD;
    } else {
//        LWIP_PLATFORM_DIAG(("Unknown parity: %.*s\n", (int)len, message));
        return;
    }

    if (HAL_UART_Init(&huart2) != HAL_OK) {
//        LWIP_PLATFORM_DIAG(("Failed to set parity\n"));
    } else {
//        LWIP_PLATFORM_DIAG(("Parity set successfully\n"));
    }
}


/**
  * @brief  Handle the serial stop bits message.
  * @param  message: The payload of the message received.
  * @param  len: The length of the message.
  * @retval None
  */
void handle_serial_stopbits(const char *message, size_t len) {
//    LWIP_PLATFORM_DIAG(("Handling serial stop bits: %.*s\n", (int)len, message));

    if (strncmp(message, "0.5", len) == 0) {
        huart2.Init.StopBits = UART_STOPBITS_0_5;
    } else if (strncmp(message, "1", len) == 0) {
        huart2.Init.StopBits = UART_STOPBITS_1;
    } else if (strncmp(message, "1.5", len) == 0) {
        huart2.Init.StopBits = UART_STOPBITS_1_5;
    } else if (strncmp(message, "2", len) == 0) {
        huart2.Init.StopBits = UART_STOPBITS_2;
    } else {
//        LWIP_PLATFORM_DIAG(("Unknown stop bits: %.*s\n", (int)len, message));
        return;
    }

    if (HAL_UART_Init(&huart2) != HAL_OK) {
//        LWIP_PLATFORM_DIAG(("Failed to set stop bits\n"));
    } else {
//        LWIP_PLATFORM_DIAG(("Stop bits set successfully\n"));
    }
}


/**
  * @brief  Handle the Ethernet speed message.
  * @param  message: The payload of the message received.
  * @param  len: The length of the message.
  * @retval None
  */
void handle_ethernet_speed(const char *message, size_t len) {
//    LWIP_PLATFORM_DIAG(("Handling Ethernet speed: %.*s\n", (int)len, message));
    // Add logic here
}

/**
  * @brief  Handles the "enable IAP" (In-Application Programming) message by
  *         setting the IAP flag and resetting the system to jump into the
  *         IAP code located at address 0x08000000.
  * @param  message: The received MQTT message indicating the IAP action.
  * @param  len: Length of the received message.
  * @retval None
  */
void handle_enable_iap(const char *message, size_t len) {
    // Print the received message for debugging
    LWIP_PLATFORM_DIAG(("Handling enable IAP: %.*s\n", (int)len, message));

    if (strncmp(message, "ENABLE", len) == 0) {
        // Write the IAP flag directly to the variable in .noinit section
        iap_flag = IAP_FLAG_VALUE;
        LWIP_PLATFORM_DIAG(("IAP flag set with value 0x%08X\n", IAP_FLAG_VALUE));

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
//    LWIP_PLATFORM_DIAG(("Handling Ethernet link status: %.*s\n", (int)len, message));
    // Add logic here
}

/**
  * @brief  Handle the Ethernet activity status message.
  * @param  message: The payload of the message received.
  * @param  len: The length of the message.
  * @retval None
  */
void handle_status_ethernet_activity(const char *message, size_t len) {
//    LWIP_PLATFORM_DIAG(("Handling Ethernet activity status: %.*s\n", (int)len, message));
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

    // Handle ACPWR topics: ACPWR/+/control/current and ACPWR/+/control/state
	if (strncmp(topic, "ACPWR/", 6) == 0) {
		// Expected topic format: ACPWR/channelX/control/<type>
		uint8_t channel;
		char control_type[16];
		// Parse the topic to extract channel and control type
		if (sscanf(topic, "ACPWR/channel%hhu/control/%15s", &channel, control_type) == 2) {
			if (strcmp(control_type, "state") == 0) {
				// Handle state message ("On" or "Off")
				uint8_t state;
				if (strncmp(data, "On", len) == 2) {
					state = 1;
				} else if (strncmp(data, "Off", len) == 3) {
					state = 0;
				} else {
//					LWIP_PLATFORM_DIAG(("Invalid ACPWR state value: %.*s\n", (int)len, data));
					return;
				}

				// Here, you might want to store the state and current_limit if needed
				// For simplicity, assuming current_limit remains unchanged or set to a default
				uint32_t current_limit = 0; // Replace with actual current limit if available

				// Call the configuration function
				PMB_Comm_ConfigureACSwitch(channel, state, current_limit);
//				LWIP_PLATFORM_DIAG(("Configured ACPWR channel %d: State=%s, Current Limit=%u\n", channel, state ? "On" : "Off", current_limit));
			}
			else if (strcmp(control_type, "current") == 0) {
				// Handle current message (integer with up to 2 decimal points)
				float current_f;
				if (sscanf(data, "%f", &current_f) != 1) {
//					LWIP_PLATFORM_DIAG(("Invalid ACPWR current value: %.*s\n", (int)len, data));
					return;
				}

				// Convert float to integer (e.g., 12.34 -> 1234)
				uint32_t current_limit = (uint32_t)(current_f * 100);

				// Here, you might want to store the current_limit and state if needed
				// For simplicity, assuming state remains unchanged or set to a default
				uint8_t state = 0; // Replace with actual state if available

				// Call the configuration function
				PMB_Comm_ConfigureACSwitch(channel, state, current_limit);
//				LWIP_PLATFORM_DIAG(("Configured ACPWR channel %d: State=%s, Current Limit=%u\n", channel, state ? "On" : "Off", current_limit));
			}
			else {
//				LWIP_PLATFORM_DIAG(("Unknown ACPWR control type: %s\n", control_type));
			}
		}
		else {
//			LWIP_PLATFORM_DIAG(("Failed to parse ACPWR topic: %s\n", topic));
		}
		return;
	}

	if (strncmp(topic, "DCPWR/", 6) == 0) {
	        // **Diagnostic Prints**
	        printf("Received DCPWR topic: %s\n", topic);
	        printf("Received message: %.*s\n", (int)len, data);
	        printf("Topic length: %lu\n", (unsigned long)strlen(topic));
	        printf("Topic bytes:\n");
	        for (size_t i = 0; i < strlen(topic); i++) {
	            printf("0x%02X ", (unsigned char)topic[i]);
	        }
	        printf("\n");

	        // Print data bytes for further debugging
	        printf("Data bytes:\n");
	        for (size_t i = 0; i < len; i++) {
	            printf("0x%02X ", (unsigned char)data[i]);
	        }
	        printf("\n");

	        // Create a mutable copy for strtok
	        char topic_copy[128];
	        strncpy(topic_copy, topic, sizeof(topic_copy) - 1);
	        topic_copy[sizeof(topic_copy) - 1] = '\0'; // Ensure null-termination

	        // Tokenize the topic
	        char *token = strtok(topic_copy, "/"); // "DCPWR"
	        token = strtok(NULL, "/"); // "slot2" or "2"
	        if (token == NULL) {
	            printf("Incomplete DCPWR topic: %s\n", topic);
	            return;
	        }

	        uint8_t slot_num = 0, channel_num = 0;
	        char control_type[16] = {0};

	        // Check for "slot" prefix
	        if (strncmp(token, "slot", 4) == 0) {
	            slot_num = atoi(token + 4); // Extract number after "slot"
	            token = strtok(NULL, "/"); // "channel0" or "0"
	            if (token == NULL) {
	                printf("Incomplete DCPWR topic after slot: %s\n", topic);
	                return;
	            }
	            // Check for "channel" prefix
	            if (strncmp(token, "channel", 7) == 0) {
	                channel_num = atoi(token + 7); // Extract number after "channel"
	            } else {
	                channel_num = atoi(token); // Assume it's just the number
	            }
	        } else {
	            // Assume slot number without "slot" prefix
	            slot_num = atoi(token);
	            token = strtok(NULL, "/"); // channel number
	            if (token == NULL) {
	                printf("Incomplete DCPWR topic after slot number: %s\n", topic);
	                return;
	            }
	            channel_num = atoi(token);
	        }

	        // Next token should be "control"
	        token = strtok(NULL, "/");
	        if (token == NULL || strcmp(token, "control") != 0) {
	            printf("Expected 'control' in DCPWR topic: %s\n", topic);
	            return;
	        }

	        // Next token is control_type
	        token = strtok(NULL, "/");
	        if (token == NULL) {
	            printf("Missing control type in DCPWR topic: %s\n", topic);
	            return;
	        }
	        strncpy(control_type, token, sizeof(control_type) - 1);
	        control_type[sizeof(control_type) - 1] = '\0'; // Ensure null-termination

	        // Normalize control_type to lowercase for case-insensitive comparison
	        printf("Parsed slot_num=%u, channel_num=%u, control_type=%s\n", slot_num, channel_num, control_type);

	        // **Validation of Slot and Channel Numbers**
	        if (slot_num >= 12 || channel_num > 1) {
	            printf("Invalid slot_num (%u) or channel_num (%u) in DCPWR topic: %s\n", slot_num, channel_num, topic);
	            return;
	        }

	        if (strcmp(control_type, "state") == 0) {
	            // Handle state message ("on" or "off")
	            char state_str[4] = {0}; // Enough for "on", "off"
	            if (len >= sizeof(state_str)) {
	                printf("State message too long: %.*s\n", (int)len, data);
	                return;
	            }
	            memcpy(state_str, data, len);
	            state_str[len] = '\0'; // Null-terminate

	            // Normalize state_str to lowercase

	            uint8_t state;
	            if (strcmp(state_str, "On") == 0) {
	                state = 1;
	            } else if (strcmp(state_str, "Off") == 0) {
	                state = 0;
	            } else {
	                printf("Invalid DCPWR state value: %s\n", state_str);
	                return;
	            }

	            // Update configuration
	            dc_pwr_configs[slot_num][channel_num].state = state;

	            // Retrieve current limit
	            float current_limit = dc_pwr_configs[slot_num][channel_num].current_limit;

	            uint32_t status_burst = 0; // As specified

	            // Call configuration function
	            PMB_Comm_ConfigureDCSwitch(slot_num, channel_num, (uint32_t)(current_limit), state, status_burst);
	            printf("Configured DCPWR slot %u channel %u: State=%s, Current Limit=%.2f\n",
	                   slot_num, channel_num, state ? "On" : "Off", current_limit);
	        }
	        else if (strcmp(control_type, "current") == 0) {
	            // Handle current message (float with up to 2 decimal points)
	            char current_str[16] = {0};
	            if (len >= sizeof(current_str)) {
	                printf("Current message too long: %.*s\n", (int)len, data);
	                return;
	            }
	            memcpy(current_str, data, len);
	            current_str[len] = '\0'; // Null-terminate

	            // Normalize current_str to lowercase (optional, since numbers)
	            // Not necessary for numeric values, but included for consistency

	            char *endptr;
	            float current_f = strtof(current_str, &endptr)*1000;
	            if (endptr == current_str || *endptr != '\0') {
	                printf("Invalid DCPWR current value: %s\n", current_str);
	                return;
	            }

	            // Update configuration
	            dc_pwr_configs[slot_num][channel_num].current_limit = current_f;

	            uint8_t state = dc_pwr_configs[slot_num][channel_num].state;
	            uint32_t status_burst = 0; // As specified

	            // Call configuration function
	            PMB_Comm_ConfigureDCSwitch(slot_num, channel_num, (uint32_t)(current_f), state, status_burst);
	            printf("Configured DCPWR slot %u channel %u: State=%s, Current Limit=%.2f\n",
	                   slot_num, channel_num, state ? "On" : "Off", current_f);
	        }
	        else {
	            printf("Unknown DCPWR control type: %s\n", control_type);
	        }

	        return;
	    }

	// Existing CMB topic handling continues here...
	// [Include the existing topic handling code here]

	// If no known topic matched
//	LWIP_PLATFORM_DIAG(("Unknown topic: %s\n", topic));
}


/**
  * @brief  Subscribes to all topics needed for the MQTT client, including ACPWR and DCPWR topics.
  * @param  client: Pointer to the MQTT client.
  * @param  client_info: Pointer to the client-specific information.
  * @param  slot: Slot number for topic subscription.
  * @retval None
  */
void mqtt_subscribe_to_topics(mqtt_client_t *client, const struct mqtt_connect_client_info_t* client_info, int slot) {
    err_t err;
    char topic[128];

    // List of all the CMB/slot%d/... topics to subscribe to
    const char* cmb_topics[] = {
        "control/iap",
        "control/trigger/state",
        "control/serial/protocol",
        "control/serial/baudrate",
        "control/serial/flowcontrol",
        "control/serial/databits",
        "control/serial/parity",
        "control/serial/stopbits",
        "control/ethernet/speed",
        "status/ethernet/link",
        "status/ethernet/activity",
    };

    // Subscribe to each CMB topic
    for (size_t i = 0; i < sizeof(cmb_topics) / sizeof(cmb_topics[0]); i++) {
        snprintf(topic, sizeof(topic), "CMB/slot%d/%s", slot, cmb_topics[i]);
        err = mqtt_sub_unsub(client, topic, 1, mqtt_request_cb, LWIP_CONST_CAST(void*, client_info), 1);
        if (err != ERR_OK) {
//            LWIP_PLATFORM_DIAG(("Failed to subscribe to %s, error %d\n", topic, err));
        } else {
//            LWIP_PLATFORM_DIAG(("Successfully subscribed to %s\n", topic));
        }
    }

    // Subscribe to ACPWR topics: ACPWR/+/control/current and ACPWR/+/control/state
    const char* acpwr_topics[] = {
        "ACPWR/+/control/current",
        "ACPWR/+/control/state",
    };

    for (size_t i = 0; i < sizeof(acpwr_topics) / sizeof(acpwr_topics[0]); i++) {
        err = mqtt_sub_unsub(client, acpwr_topics[i], 1, mqtt_request_cb, LWIP_CONST_CAST(void*, client_info), 1);
        if (err != ERR_OK) {
//            LWIP_PLATFORM_DIAG(("Failed to subscribe to %s, error %d\n", acpwr_topics[i], err));
        } else {
//            LWIP_PLATFORM_DIAG(("Successfully subscribed to %s\n", acpwr_topics[i]));
        }
    }

    // Subscribe to DCPWR topics: DCPWR/+/+/control/current and DCPWR/+/+/control/state
    const char* dc_pwr_topics[] = {
        "DCPWR/+/+/control/current",
        "DCPWR/+/+/control/state",
    };

    for (size_t i = 0; i < sizeof(dc_pwr_topics) / sizeof(dc_pwr_topics[0]); i++) {
        err = mqtt_sub_unsub(client, dc_pwr_topics[i], 1, mqtt_request_cb, LWIP_CONST_CAST(void*, client_info), 1);
        if (err != ERR_OK) {
//            LWIP_PLATFORM_DIAG(("Failed to subscribe to %s, error %d\n", dc_pwr_topics[i], err));
        } else {
//            LWIP_PLATFORM_DIAG(("Successfully subscribed to %s\n", dc_pwr_topics[i]));
        }
    }
}
