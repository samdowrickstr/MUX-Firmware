#ifndef PMB_COMM_H
#define PMB_COMM_H

#include "stm32h7xx_hal.h"
#include "cmsis_os.h"
#include "cJSON.h"
#include "main.h"

// Define buffer sizes
#define UART4_RX_BUFFER_SIZE 4096
#define UART4_TX_BUFFER_SIZE 256

// UART settings
#define PMB_UART_BAUDRATE     115200
#define PMB_UART_WORDLENGTH   UART_WORDLENGTH_8B
#define PMB_UART_STOPBITS     UART_STOPBITS_1
#define PMB_UART_PARITY       UART_PARITY_NONE
#define PMB_UART_MODE         UART_MODE_TX_RX
#define PMB_UART_HWCONTROL    UART_HWCONTROL_NONE

// Extern variables
extern UART_HandleTypeDef huart4;

// Maximum number of DCSwitch and ACSwitch channels
#define MAX_DCSWITCH_PORTS    12
#define MAX_ACSWITCH_CHANNELS 2
#define MAX_DCSWITCH_CHANNELS 2  // Each DCSwitch has 2 channels

// Structure to hold ACSwitch channel status
typedef struct {
    int ch;
    int channel_state;
    int channel_fault_mask;
    double output_voltage;
    double output_current;
} PMB_ACSwitchChannelStatus_t;

// Structure to hold ACSwitch status
typedef struct {
    int system_state;
    int fault_mask;
    int channel_count;
    PMB_ACSwitchChannelStatus_t channels[MAX_ACSWITCH_CHANNELS];
} PMB_ACSwitchStatus_t;

// Structure to hold DCSwitch channel status
typedef struct {
    int ch;
    int channel_state;
    int channel_fault_mask;
    double output_voltage;
    double output_current;
} PMB_DCSwitchChannelStatus_t;

// Structure to hold DCSwitch status for each port
typedef struct {
    int port;
    int system_state;
    int fault_mask;
    int pcb_temperature;
    double current_reference;
    int channel_count;
    PMB_DCSwitchChannelStatus_t channels[MAX_DCSWITCH_CHANNELS];
} PMB_DCSwitchStatus_t;

// Structure to hold the entire system status
typedef struct {
    double elapsed_time;
    int system_state;
    int fault_mask;
    PMB_ACSwitchStatus_t ac_switch_status;
    int dc_switch_count;
    PMB_DCSwitchStatus_t dc_switch_statuses[MAX_DCSWITCH_PORTS];
} PMB_SystemStatus_t;

// Extern declarations
extern PMB_SystemStatus_t pmb_system_status;
extern osMutexId_t pmbStatusMutexHandle;

// Function prototypes
void PMB_Comm_Init(void);
void PMB_Comm_Task(void *argument);
void PMB_Comm_SendJSON(cJSON *json);
void PMB_Comm_RequestSystemIdentification(void);
void PMB_Comm_RequestSystemStatus(uint32_t interval);
void PMB_Comm_ConfigureDCSwitch(uint8_t port, uint8_t channel, uint32_t current_limit, uint8_t on, uint32_t status_burst);
void PMB_Comm_ConfigureACSwitch(uint8_t channel, uint8_t on, uint32_t current_limit);
void PMB_Comm_ActivateCLI(void);
PMB_SystemStatus_t PMB_GetLatestSystemStatus(void);

// Callback prototypes
void PMB_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void PMB_UART_ErrorCallback(UART_HandleTypeDef *huart);

#endif // PMB_COMM_H
