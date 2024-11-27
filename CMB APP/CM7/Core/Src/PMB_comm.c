#include "PMB_comm.h"
#include "uart_callbacks.h"
#include "MQTTTopic.h"

// UART receive buffer
static uint8_t uart4_rx_buffer[UART4_RX_BUFFER_SIZE];
static uint8_t uart4_tx_buffer[UART4_TX_BUFFER_SIZE];

// FreeRTOS task handle
osThreadId_t pmbCommTaskHandle;
const osThreadAttr_t pmbCommTask_attributes = {
    .name = "pmbCommTask",
    .stack_size = 2048 * 4,
    .priority = (osPriority_t)osPriorityNormal,
};

// UART receive semaphore (not used in this implementation)
osSemaphoreId_t uart4RxSemaphoreHandle;

// Message queue for UART reception
#define UART4_RX_QUEUE_LENGTH 10
#define UART4_RX_ITEM_SIZE    UART4_RX_BUFFER_SIZE
osMessageQueueId_t uart4RxQueueHandle;

// Variables for receiving data
static uint8_t receive_buffer[UART4_RX_BUFFER_SIZE];
static uint16_t receive_index = 0;

// Global variable to hold the latest system status
PMB_SystemStatus_t pmb_system_status;

// Mutex for synchronizing access to the system status
osMutexId_t pmbStatusMutexHandle;

// Forward declarations
static void PMB_Comm_HandleSystemIdentification(cJSON *json);
static void PMB_Comm_HandleSystemStatus(cJSON *json);
static void PMB_Comm_HandleGeneralMessage(cJSON *json);

// Initialize PMB Communication
void PMB_Comm_Init(void) {
    // Create the semaphore for UART RX (not used here but initialized for completeness)
    uart4RxSemaphoreHandle = osSemaphoreNew(1, 0, NULL);

    // Create the message queue
    uart4RxQueueHandle = osMessageQueueNew(UART4_RX_QUEUE_LENGTH, UART4_RX_ITEM_SIZE, NULL);

    // Start the UART receive interrupt
    HAL_UART_Receive_IT(&huart4, uart4_rx_buffer, 1); // Receive one byte at a time

    // Create the UART communication task
    pmbCommTaskHandle = osThreadNew(PMB_Comm_Task, NULL, &pmbCommTask_attributes);

    // Create the mutex for system status
    pmbStatusMutexHandle = osMutexNew(NULL);
}

// Callback function called when data is received over UART4
void PMB_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == UART4) {
        uint8_t received_byte = uart4_rx_buffer[0];

        // Store it in the receive buffer
        if (receive_index < UART4_RX_BUFFER_SIZE - 1) {
            receive_buffer[receive_index++] = received_byte;

            // Check for end of message (assuming CRLF or LF)
            if (received_byte == '\n') {
                // Null-terminate the buffer
                receive_buffer[receive_index] = '\0';

                // Send the buffer to the queue
                osMessageQueuePut(uart4RxQueueHandle, receive_buffer, 0, 0);

                // Reset the receive index
                receive_index = 0;
            }
        } else {
            // Buffer overflow, reset index
            receive_index = 0;
        }

        // Restart UART reception for next byte
        HAL_UART_Receive_IT(&huart4, uart4_rx_buffer, 1);
    }
}

void PMB_UART_ErrorCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == UART4) {
        // Handle UART4 errors
        // Restart reception if necessary
        HAL_UART_Receive_IT(&huart4, uart4_rx_buffer, 1);
    }
}

// Function to send cJSON object over UART4
void PMB_Comm_SendJSON(cJSON *json) {
    if (json == NULL) return;

    char *json_string = cJSON_PrintUnformatted(json);
    if (json_string != NULL) {
        uint16_t length = strlen(json_string);

        // Ensure the length does not exceed buffer size
        if (length < UART4_TX_BUFFER_SIZE - 2) { // Reserve space for CRLF
            // Copy the JSON string to the UART transmit buffer
            memcpy(uart4_tx_buffer, json_string, length);

            // Add CRLF at the end if required by protocol
            uart4_tx_buffer[length++] = '\r';
            uart4_tx_buffer[length++] = '\n';

            // Transmit the data
            HAL_UART_Transmit(&huart4, uart4_tx_buffer, length, HAL_MAX_DELAY);
        } else {
            // Handle error: message too long
//            printf("Error: JSON message too long to send over UART\n");
        }

        cJSON_free(json_string);
    }
}

// Function to request system identification
void PMB_Comm_RequestSystemIdentification(void) {
    cJSON *json = cJSON_CreateObject();
    if (json) {
        cJSON_AddNumberToObject(json, "qid", 1);
        PMB_Comm_SendJSON(json);
        cJSON_Delete(json);
    }
}

// Function to request system status
void PMB_Comm_RequestSystemStatus(uint32_t interval) {
    cJSON *json = cJSON_CreateObject();
    if (json) {
        cJSON_AddNumberToObject(json, "qst", interval);
        PMB_Comm_SendJSON(json);
        cJSON_Delete(json);
    }
}

// Function to configure DCSwitch
void PMB_Comm_ConfigureDCSwitch(uint8_t port, uint8_t channel, uint32_t current_limit, uint8_t on, uint32_t status_burst) {
    cJSON *json = cJSON_CreateObject();
    if (json) {
        cJSON *dcc = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "dcc", dcc);
        cJSON_AddNumberToObject(dcc, "p", port);
        cJSON_AddNumberToObject(dcc, "ch", channel);
        cJSON_AddNumberToObject(dcc, "cl", current_limit);
        cJSON_AddNumberToObject(dcc, "on", on);
        if (status_burst > 0) {
            cJSON_AddNumberToObject(dcc, "sb", status_burst);
        }
        PMB_Comm_SendJSON(json);
        cJSON_Delete(json);
    }
}

// Function to configure ACSwitch
void PMB_Comm_ConfigureACSwitch(uint8_t channel, uint8_t on, uint32_t current_limit) {
    cJSON *json = cJSON_CreateObject();
    if (json) {
        cJSON *acc = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "acc", acc);
        cJSON_AddNumberToObject(acc, "ch", channel);
        cJSON_AddNumberToObject(acc, "on", on);
        cJSON_AddNumberToObject(acc, "cl", current_limit);
        PMB_Comm_SendJSON(json);
        cJSON_Delete(json);
    }
}

// Function to activate CLI on PMB
void PMB_Comm_ActivateCLI(void) {
    const char *cli_sequence = "$PSTRWAKEUP*18\r\n";
    HAL_UART_Transmit(&huart4, (uint8_t *)cli_sequence, strlen(cli_sequence), HAL_MAX_DELAY);
}

// Task function for PMB communication
void PMB_Comm_Task(void *argument) {
    uint8_t message[UART4_RX_BUFFER_SIZE];
    uint32_t status_interval = 1000; // Request status every 5 seconds

    // Initial request for system identification
    PMB_Comm_RequestSystemIdentification();

    // Start requesting system status
    PMB_Comm_RequestSystemStatus(status_interval);

    for (;;) {
        // Wait for a message from the UART receive queue
        if (osMessageQueueGet(uart4RxQueueHandle, message, NULL, osWaitForever) == osOK) {
            // Process the received data
            PMB_Comm_ProcessReceivedData(message, strlen((char *)message));
        }
    }
}

// Process the received data
void PMB_Comm_ProcessReceivedData(uint8_t *data, uint16_t length) {
    // Parse the JSON data
    cJSON *json = cJSON_ParseWithLength((const char *)data, length);
    if (json != NULL) {
        // Convert cJSON object back to string for printing
        char *json_string = cJSON_PrintUnformatted(json); // Use cJSON_Print for formatted output
        if (json_string != NULL) {
//            printf("Received JSON message: %s\n", json_string);
            cJSON_free(json_string); // Free the allocated string
        } else {
//        	printf("Failed to convert JSON object to string\n");
        }

        // Dispatch to appropriate handlers with debug confirmation
        if (cJSON_HasObjectItem(json, "mid")) {
//        	printf("Dispatching to PMB_Comm_HandleSystemIdentification\n");
            PMB_Comm_HandleSystemIdentification(json);
        } else if (cJSON_HasObjectItem(json, "mbs")) {
//        	printf("Dispatching to PMB_Comm_HandleSystemStatus\n");
            PMB_Comm_HandleSystemStatus(json);
        } else if (cJSON_HasObjectItem(json, "msg")) {
//        	printf("Dispatching to PMB_Comm_HandleGeneralMessage\n");
            PMB_Comm_HandleGeneralMessage(json);
        } else {
            // printf other possible messages
//        	printf("Received unknown JSON message\n");
        }
        cJSON_Delete(json);
    } else {
        // Handle parsing error with detailed information
//    	printf("JSON parsing error: %s\n", cJSON_GetErrorPtr());
    }
}


// Handle system identification message
static void PMB_Comm_HandleSystemIdentification(cJSON *json) {
    cJSON *mid = cJSON_GetObjectItem(json, "mid");
    if (mid != NULL) {
        int v1 = cJSON_GetObjectItem(mid, "v1")->valueint;
        int v2 = cJSON_GetObjectItem(mid, "v2")->valueint;
        int v3 = cJSON_GetObjectItem(mid, "v3")->valueint;
        // ... get other fields ...

//        printf("PMB Version: %d.%d.%d\n", v1, v2, v3);
        // Store or process the identification data
    }

    cJSON *sid = cJSON_GetObjectItem(json, "sid");
    if (sid != NULL && cJSON_IsArray(sid)) {
        int slave_count = cJSON_GetArraySize(sid);
        for (int i = 0; i < slave_count; i++) {
            cJSON *slave = cJSON_GetArrayItem(sid, i);
            int p = cJSON_GetObjectItem(slave, "p")->valueint;
            int ad = cJSON_GetObjectItem(slave, "ad")->valueint;
            // ... get other fields ...
//            printf("Slave %d: Address %d\n", p, ad);
            // Store or process slave information
        }
    }
}

static void PMB_Comm_HandleSystemStatus(cJSON *json) {
    cJSON *mbs = cJSON_GetObjectItem(json, "mbs");
    if (mbs != NULL) {
        // Acquire mutex before updating shared data
        osMutexAcquire(pmbStatusMutexHandle, osWaitForever);

        // Safely retrieve and assign values
        cJSON *et = cJSON_GetObjectItem(mbs, "et");
        if (et != NULL && cJSON_IsNumber(et)) {
            pmb_system_status.elapsed_time = et->valuedouble;
        } else {
//            printf("Warning: 'et' field missing or invalid\n");
            pmb_system_status.elapsed_time = 0.0; // Default or previous value
        }

        cJSON *ss = cJSON_GetObjectItem(mbs, "ss");
        if (ss != NULL && cJSON_IsNumber(ss)) {
            pmb_system_status.system_state = ss->valueint;
        } else {
//            printf("Warning: 'ss' field missing or invalid\n");
            pmb_system_status.system_state = 0; // Default or previous value
        }

        cJSON *fm = cJSON_GetObjectItem(mbs, "fm");
        if (fm != NULL && cJSON_IsNumber(fm)) {
            pmb_system_status.fault_mask = fm->valueint;
        } else {
//            printf("Warning: 'fm' field missing or invalid\n");
            pmb_system_status.fault_mask = 0; // Default or previous value
        }

        // Release mutex after updating shared data
        osMutexRelease(pmbStatusMutexHandle);

        // Optionally print the status
//        printf("PMB Elapsed Time: %f s\n", pmb_system_status.elapsed_time);
//        printf("PMB System State: %d\n", pmb_system_status.system_state);
//        printf("PMB Fault Mask: %d\n", pmb_system_status.fault_mask);
    }

    // Handle ACSwitchStatus
    cJSON *acs = cJSON_GetObjectItem(json, "acs");
    if (acs != NULL) {
        cJSON *acs_ss = cJSON_GetObjectItem(acs, "ss");
        cJSON *acs_fm = cJSON_GetObjectItem(acs, "fm");

        if (acs_ss != NULL && cJSON_IsNumber(acs_ss)) {
            // Process system state if needed
//            printf("ACSwitch System State: %d\n", acs_ss->valueint);
        } else {
//            printf("Warning: 'ss' field in 'acs' missing or invalid\n");
        }

        if (acs_fm != NULL && cJSON_IsNumber(acs_fm)) {
            // Process fault mask if needed
//            printf("ACSwitch Fault Mask: %d\n", acs_fm->valueint);
        } else {
//            printf("Warning: 'fm' field in 'acs' missing or invalid\n");
        }

        // Handle ACSwitch channels
        cJSON *cs = cJSON_GetObjectItem(acs, "cs");
        if (cs != NULL && cJSON_IsArray(cs)) {
            int channel_count = cJSON_GetArraySize(cs);
            for (int i = 0; i < channel_count; i++) {
                cJSON *channel = cJSON_GetArrayItem(cs, i);
                if (channel == NULL) {
//                    printf("Warning: 'channel' is NULL\n");
                    continue;
                }

                cJSON *ch = cJSON_GetObjectItem(channel, "ch");
                cJSON *channel_state = cJSON_GetObjectItem(channel, "cs");
                cJSON *channel_fault_mask = cJSON_GetObjectItem(channel, "fm");
                cJSON *ov = cJSON_GetObjectItem(channel, "ov");
                cJSON *oi = cJSON_GetObjectItem(channel, "oi");

                if (ch != NULL && cJSON_IsNumber(ch) &&
                    channel_state != NULL && cJSON_IsNumber(channel_state) &&
                    channel_fault_mask != NULL && cJSON_IsNumber(channel_fault_mask)) {

//                    printf("ACSwitch Channel %d State: %d Fault Mask: %d",
//                                 ch->valueint, channel_state->valueint, channel_fault_mask->valueint);

                    if (ov != NULL && cJSON_IsNumber(ov)) {
//                        printf(" Voltage: %f V", ov->valuedouble);
                    } else {
//                        printf(" Voltage: N/A");
                    }

                    if (oi != NULL && cJSON_IsNumber(oi)) {
//                        printf(" Current: %f mA\n", oi->valuedouble);
                    } else {
//                        printf(" Current: N/A\n");
                    }

                } else {
//                    printf("Warning: Missing or invalid fields in ACSwitch channel\n");
                }
            }
        } else {
//            printf("Warning: 'cs' array in 'acs' missing or invalid\n");
        }
    }

    // Handle DCSwitchStatus
    cJSON *dcs = cJSON_GetObjectItem(json, "dcs");
    if (dcs != NULL && cJSON_IsArray(dcs)) {
        int dcs_count = cJSON_GetArraySize(dcs);
        for (int i = 0; i < dcs_count; i++) {
            cJSON *dcs_item = cJSON_GetArrayItem(dcs, i);
            if (dcs_item == NULL) {
//                printf("Warning: 'dcs_item' is NULL\n");
                continue;
            }

            cJSON *p = cJSON_GetObjectItem(dcs_item, "p");
            cJSON *ss = cJSON_GetObjectItem(dcs_item, "ss");
            cJSON *fm = cJSON_GetObjectItem(dcs_item, "fm");
            cJSON *pt = cJSON_GetObjectItem(dcs_item, "pt");
            cJSON *ir = cJSON_GetObjectItem(dcs_item, "ir");

            if (p != NULL && cJSON_IsNumber(p) &&
                ss != NULL && cJSON_IsNumber(ss) &&
                fm != NULL && cJSON_IsNumber(fm) &&
                pt != NULL && cJSON_IsNumber(pt) &&
                ir != NULL && cJSON_IsNumber(ir)) {

//                printf("DCSwitch Port %d State: %d PCB Temp: %d Current Ref: %f mV\n",
//                             p->valueint, ss->valueint, pt->valueint, ir->valuedouble);

                // Handle DCSwitch channels
                cJSON *cs = cJSON_GetObjectItem(dcs_item, "cs");
                if (cs != NULL && cJSON_IsArray(cs)) {
                    int channel_count = cJSON_GetArraySize(cs);
                    for (int j = 0; j < channel_count; j++) {
                        cJSON *channel = cJSON_GetArrayItem(cs, j);
                        if (channel == NULL) {
//                            printf("Warning: 'channel' in DCSwitch is NULL\n");
                            continue;
                        }

                        cJSON *ch = cJSON_GetObjectItem(channel, "ch");
                        cJSON *channel_state = cJSON_GetObjectItem(channel, "cs");
                        cJSON *channel_fault_mask = cJSON_GetObjectItem(channel, "fm");
                        cJSON *ov = cJSON_GetObjectItem(channel, "ov");
                        cJSON *oi = cJSON_GetObjectItem(channel, "oi");

                        if (ch != NULL && cJSON_IsNumber(ch) &&
                            channel_state != NULL && cJSON_IsNumber(channel_state) &&
                            channel_fault_mask != NULL && cJSON_IsNumber(channel_fault_mask)) {

//                            printf("DCSwitch Port %d Channel %d State: %d Fault Mask: %d",
//                                         p->valueint, ch->valueint, channel_state->valueint, channel_fault_mask->valueint);
                            if (p->valueint == 9){
//                            	printf("channel 9");
                            }

                            if (ov != NULL && cJSON_IsNumber(ov)) {
//                                printf(" Voltage: %f mV", ov->valuedouble);
                            } else {
//                                printf(" Voltage: N/A");
                            }
                            int slot_num = p->valueint;  // Assuming 'p' represents the slot number
                            int channel_num = ch->valueint;  // Channel number
                            float current_a = 0;
                            if (oi != NULL && cJSON_IsNumber(oi)) {
//                                printf(" Current: %f mA\n", oi->valuedouble);
                                current_a = oi->valuedouble /1000;  // Convert mA to A
//                                printf(" Current: %f A\n", current_a);
                                publish_dc_switch_current(slot_num, channel_num, current_a);
                                osDelay(1);
                            } else {
//                                printf(" Current: N/A\n");
                            }

                        } else {
//                            printf("Warning: Missing or invalid fields in DCSwitch channel\n");
                        }
                    }
                } else {
//                    printf("Warning: 'cs' array in DCSwitch missing or invalid\n");
                }
            } else {
//                printf("Warning: Missing or invalid fields in DCSwitch\n");
            }
        }
    }
}


// Handle general message
static void PMB_Comm_HandleGeneralMessage(cJSON *json) {
    cJSON *msg = cJSON_GetObjectItem(json, "msg");
    if (msg != NULL) {
        const char *type = cJSON_GetObjectItem(msg, "type")->valuestring;
        const char *text = cJSON_GetObjectItem(msg, "txt")->valuestring;

//        printf("PMB Message Type: %s Text: %s\n", type, text);
    }
}

// Function to get the latest system status
PMB_SystemStatus_t PMB_GetLatestSystemStatus(void) {
    PMB_SystemStatus_t status_copy;

    // Acquire mutex before reading shared data
    osMutexAcquire(pmbStatusMutexHandle, osWaitForever);

    // Copy the global system status to a local variable
    status_copy = pmb_system_status;

    // Release mutex after reading shared data
    osMutexRelease(pmbStatusMutexHandle);

    return status_copy;
}
