/**
  ******************************************************************************
  * @file    MQTTClient.h
  * @author  Samuel Dowrick
  * @brief   This file contains the configuration and function declarations for
  *          the MQTT client. It allows setting the MQTT client configurations
  *          and provides function prototypes used by other modules.
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

#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

// MQTT Client Configuration
extern char mqtt_client_id[20];  // Buffer to hold the unique client ID
#define MQTT_USER            NULL    /**< @brief MQTT User */
#define MQTT_PASS            NULL    /**< @brief MQTT Password */
#define MQTT_KEEP_ALIVE      5      /**< @brief MQTT Keep Alive Interval */

// Will Configuration
extern char mqtt_will_topic[50];  /**< @brief Dynamic MQTT Will Topic */
#define MQTT_WILL_MSG        "Offline"          /**< @brief MQTT Will Message */
#define MQTT_WILL_QOS        1                  /**< @brief MQTT Will QoS */
#define MQTT_WILL_RETAIN     1                  /**< @brief MQTT Will Retain Flag */

#include "lwip/apps/mqtt.h"

// Declare the callback function for MQTT requests
void mqtt_request_cb(void *arg, err_t err);

// Declare other functions that might be shared across files
void mqtt_example_init(void);

// External declaration of the MQTT client instance
extern mqtt_client_t* mqtt_client;
#endif // MQTT_CLIENT_H
