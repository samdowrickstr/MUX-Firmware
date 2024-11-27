/**
  ******************************************************************************
  * @file    MQTTClient.c
  * @author  Samuel Dowrick
  * @brief   This file provides the core implementation for the MQTT client,
  *          including connection management, data handling, and callback
  *          functions for MQTT operations.
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

#include <string.h>
#include "lwip/apps/mqtt.h"
#include "main.h"
#include "MQTTClient.h"  // Include user-defined configurations
#include "MQTTTopic.h"   // Include the header with topic-specific functions

#if LWIP_TCP

#ifndef LWIP_MQTT_EXAMPLE_IPADDR_INIT
#if LWIP_IPV4
#define LWIP_MQTT_EXAMPLE_IPADDR_INIT = IPADDR4_INIT(PP_HTONL(LWIP_MAKEU32(192, 168, 35, 1)))
#else
#define LWIP_MQTT_EXAMPLE_IPADDR_INIT
#endif
#endif

// Static variables
static ip_addr_t mqtt_ip LWIP_MQTT_EXAMPLE_IPADDR_INIT;  // IP address of the MQTT broker
mqtt_client_t* mqtt_client = NULL;                       // Pointer to the MQTT client instance

// Forward declaration of the callback function
void mqtt_request_cb(void *arg, err_t err);

// Static variable to hold the current topic being processed
static char current_topic[256] = {0};
char mqtt_will_topic[50];
char mqtt_client_id[20];

struct mqtt_connect_client_info_t mqtt_client_info = {
    .client_id = mqtt_client_id,
    .client_user = MQTT_USER,
    .client_pass = MQTT_PASS,
    .keep_alive = MQTT_KEEP_ALIVE,
    .will_msg = MQTT_WILL_MSG,
    .will_qos = MQTT_WILL_QOS,
    .will_retain = MQTT_WILL_RETAIN
};

/**
  * @brief  Callback function for incoming data fragments.
  * @param  arg: User-defined argument, in this case, the client information structure.
  * @param  data: Pointer to the data received.
  * @param  len: Length of the data received.
  * @param  flags: Flags indicating additional information about the data.
  * @retval None
  */
static void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags) {
    const struct mqtt_connect_client_info_t* client_info = (const struct mqtt_connect_client_info_t*)arg;
    LWIP_UNUSED_ARG(flags);

//    LWIP_PLATFORM_DIAG(("MQTT client \"%s\" data cb: len %d\n", client_info->client_id, (int)len));

    // Call the topic handler function with the current topic and data
    handle_incoming_topic_data(current_topic, (const char *)data, len);
}

/**
  * @brief  Callback function for incoming publish notifications.
  * @param  arg: User-defined argument, in this case, the client information structure.
  * @param  topic: The topic on which the message was published.
  * @param  tot_len: The total length of the message.
  * @retval None
  */
static void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len) {
    const struct mqtt_connect_client_info_t* client_info = (const struct mqtt_connect_client_info_t*)arg;

//    LWIP_PLATFORM_DIAG(("MQTT client \"%s\" publish cb: topic %s, len %d\n", client_info->client_id, topic, (int)tot_len));

    // Store the topic for use in data callback
    strncpy(current_topic, topic, sizeof(current_topic) - 1);
    current_topic[sizeof(current_topic) - 1] = '\0'; // Ensure null-termination
}

/**
  * @brief  Callback function for publish/subscribe/unsubscribe requests.
  * @param  arg: User-defined argument, in this case, the client information structure.
  * @param  err: The error status of the request.
  * @retval None
  */
void mqtt_request_cb(void *arg, err_t err) {
    const struct mqtt_connect_client_info_t* client_info = (const struct mqtt_connect_client_info_t*)arg;

//    LWIP_PLATFORM_DIAG(("MQTT client \"%s\" request cb: err %d\n", client_info->client_id, (int)err));

    if (err != ERR_OK) {
        // Implement error handling logic here
//        LWIP_PLATFORM_DIAG(("Request failed for client \"%s\" with error %d\n", client_info->client_id, err));
    }
}

/**
  * @brief  Callback function for MQTT connection status changes.
  * @param  client: Pointer to the MQTT client instance.
  * @param  arg: User-defined argument, in this case, the client information structure.
  * @param  status: Connection status of the MQTT client.
  * @retval None
  */
static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status) {
    const struct mqtt_connect_client_info_t* client_info = (const struct mqtt_connect_client_info_t*)arg;
    LWIP_UNUSED_ARG(client);

//    LWIP_PLATFORM_DIAG(("MQTT client \"%s\" connection cb: status %d\n", client_info->client_id, (int)status));

    if (status == MQTT_CONNECT_ACCEPTED) {
        // Subscribe to topics upon successful connection
        mqtt_subscribe_to_topics(client, client_info, slot);

        // Publish a message upon connection
        const char *connect_message = "Online";
        char topic[128];
        snprintf(topic, sizeof(topic), "CMM/slot%d/status", slot); // Use the slot number in the status topic
        err_t err = mqtt_publish(client,
                           topic,                   // Topic
                           connect_message,         // Message
                           strlen(connect_message), // Message length
                           1,                       // QoS level
                           1,                       // Retain flag
                           mqtt_request_cb,         // Callback function
                           LWIP_CONST_CAST(void*, client_info)); // Callback argument

        if (err != ERR_OK) {
//            LWIP_PLATFORM_DIAG(("Failed to publish connection message, error %d\n", err));
        } else {
//            LWIP_PLATFORM_DIAG(("Published connection message to topic %s\n", topic));
        }

    } else {
        // Handle connection failure or disconnection
//        LWIP_PLATFORM_DIAG(("MQTT client \"%s\" failed to connect or disconnected with status %d. Reconnecting...\n", client_info->client_id, status));
        // Implement reconnection logic here
        mqtt_example_init();  // Call the init function to reconnect
    }
}

#endif /* LWIP_TCP */

/**
  * @brief  Initialization function for the MQTT client.
  *         Sets up the MQTT client instance and connects to the broker.
  * @retval None
  */
void mqtt_example_init(void) {
    #if LWIP_TCP
	osDelay(20);
    mqtt_client = mqtt_client_new();
	osDelay(20);

    if (mqtt_client == NULL) {
//        LWIP_PLATFORM_DIAG(("Failed to create MQTT client\n"));
        return;
    }

    // Generate the unique client ID and Will topic based on slot number
    snprintf(mqtt_client_id, sizeof(mqtt_client_id), "CMM%d", slot);
    snprintf(mqtt_will_topic, sizeof(mqtt_will_topic), "CMM/slot%d/status", slot);
	osDelay(200);
    mqtt_client_info.client_id = mqtt_client_id;  // Assign unique Client ID
    mqtt_client_info.will_topic = mqtt_will_topic;  // Assign unique Will topic
	osDelay(200);
    mqtt_client_connect(mqtt_client,
                        &mqtt_ip, MQTT_PORT,
                        mqtt_connection_cb, LWIP_CONST_CAST(void*, &mqtt_client_info),
                        &mqtt_client_info);

    mqtt_set_inpub_callback(mqtt_client,
                            mqtt_incoming_publish_cb,
                            mqtt_incoming_data_cb,
                            LWIP_CONST_CAST(void*, &mqtt_client_info));
	osDelay(100);
    #endif /* LWIP_TCP */
}
