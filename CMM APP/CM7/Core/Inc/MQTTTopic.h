/**
  ******************************************************************************
  * @file    MQTTTopic.h
  * @author  Samuel Dowrick
  * @brief   This header file provides declarations for functions that handle
  *          topic-specific logic and subscriptions for the MQTT client.
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

#ifndef MQTT_TOPIC_H
#define MQTT_TOPIC_H

#include "lwip/apps/mqtt.h"

// Function declarations
void handle_incoming_topic_data(const char *topic, const char *data, size_t len);
void mqtt_subscribe_to_topics(mqtt_client_t *client, const struct mqtt_connect_client_info_t* client_info, int slot);

#endif // MQTT_TOPIC_H
