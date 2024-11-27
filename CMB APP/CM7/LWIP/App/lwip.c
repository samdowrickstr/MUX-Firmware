/* USER CODE BEGIN Header */
/**
 ******************************************************************************
  * File Name          : LWIP.c
  * Description        : This file provides initialization code for LWIP
  *                      middleWare.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "lwip.h"
#include "lwip/init.h"
#include "lwip/netif.h"
#if defined ( __CC_ARM )  /* MDK ARM Compiler */
#include "lwip/sio.h"
#endif /* MDK ARM Compiler */
#include "ethernetif.h"
#include <string.h>

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
/* Private function prototypes -----------------------------------------------*/
static void ethernet_link_status_updated(struct netif *netif);
/* ETH Variables initialization ----------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/* Variables Initialization */
struct netif gnetif;
ip4_addr_t ipaddr;
ip4_addr_t netmask;
ip4_addr_t gw;
uint8_t IP_ADDRESS[4];
uint8_t NETMASK_ADDRESS[4];
uint8_t GATEWAY_ADDRESS[4];
/* USER CODE BEGIN OS_THREAD_ATTR_CMSIS_RTOS_V2 */
#define INTERFACE_THREAD_STACK_SIZE ( 2048 )
osThreadAttr_t attributes;
/* USER CODE END OS_THREAD_ATTR_CMSIS_RTOS_V2 */

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */

/**
  * LwIP initialization function
  */
void MX_LWIP_Init(void)
{
  /* IP addresses initialization */
  IP_ADDRESS[0] = 192;
  IP_ADDRESS[1] = 168;
  IP_ADDRESS[2] = 35;
  IP_ADDRESS[3] = 84;
  NETMASK_ADDRESS[0] = 255;
  NETMASK_ADDRESS[1] = 255;
  NETMASK_ADDRESS[2] = 255;
  NETMASK_ADDRESS[3] = 0;
  GATEWAY_ADDRESS[0] = 0;
  GATEWAY_ADDRESS[1] = 0;
  GATEWAY_ADDRESS[2] = 0;
  GATEWAY_ADDRESS[3] = 0;

/* USER CODE BEGIN IP_ADDRESSES */
#if defined(CMBTS_HW)
	// Configuration for CMBTS_HW
	IP_ADDRESS[0] = 192;
	IP_ADDRESS[1] = 168;
	IP_ADDRESS[2] = 35;
	IP_ADDRESS[3] = 20;
	slot = 32;
	LWIP_PLATFORM_DIAG(("Configured for CMBTS_HW: IP 192.168.35.20, Slot 32\n"));

#elif defined(CMB_HW)
	// Configuration for CMB_HW
	IP_ADDRESS[0] = 192;
	IP_ADDRESS[1] = 168;
	IP_ADDRESS[2] = 35;
	IP_ADDRESS[3] = 30;
	slot = 31;
	LWIP_PLATFORM_DIAG(("Configured for CMB_HW: IP 192.168.35.30, Slot 31\n"));

#elif defined(CMM_HW)
	// Configuration for CMM_HW with additional slot adjustment based on IP table selection
	uint8_t state = 15;

	// Uncomment and modify these lines if you need to set 'state' based on GPIO pins
	// state |= (HAL_GPIO_ReadPin(GPIOF, IP_SEL_1_Pin) << 0);
	// state |= (HAL_GPIO_ReadPin(GPIOF, IP_SEL_2_Pin) << 1);
	// state |= (HAL_GPIO_ReadPin(GPIOF, IP_SEL_3_Pin) << 2);
	// state |= (HAL_GPIO_ReadPin(GPIOF, IP_SEL_4_Pin) << 3);

	slot = state;  // Set the global slot variable

	// Define the two IP tables (for local and remote IPs)
	const uint8_t ip_table_0[][4] = {
		{192, 168, 35, 50},  // Slot 0
		{192, 168, 35, 51},  // Slot 1
		{192, 168, 35, 52},  // Slot 2
		{192, 168, 35, 53},  // Slot 3
		{192, 168, 35, 54},  // Slot 4
		{192, 168, 35, 55},  // Slot 5
		{192, 168, 35, 56},  // Slot 6
		{192, 168, 35, 57},  // Slot 7
		{192, 168, 35, 58},  // Slot 8
		{192, 168, 35, 59},  // Slot 9
		{192, 168, 35, 60},  // Slot 10
		{192, 168, 35, 61},  // Slot 11
		{192, 168, 35, 62},  // Slot 12
		{192, 168, 35, 63},  // Slot 13
		{192, 168, 35, 64},  // Slot 14
		{192, 168, 35, 65},  // Slot 15
	};

	const uint8_t ip_table_1[][4] = {
		{192, 168, 35, 100},  // Slot 0
		{192, 168, 35, 101},  // Slot 1
		{192, 168, 35, 102},  // Slot 2
		{192, 168, 35, 103},  // Slot 3
		{192, 168, 35, 104},  // Slot 4
		{192, 168, 35, 105},  // Slot 5
		{192, 168, 35, 106},  // Slot 6
		{192, 168, 35, 107},  // Slot 7
		{192, 168, 35, 108},  // Slot 8
		{192, 168, 35, 109},  // Slot 9
		{192, 168, 35, 110},  // Slot 10
		{192, 168, 35, 111},  // Slot 11
		{192, 168, 35, 112},  // Slot 12
		{192, 168, 35, 113},  // Slot 13
		{192, 168, 35, 114},  // Slot 14
		{192, 168, 35, 115},  // Slot 15
	};

	// Read the IP_SEL_0 pin and select the correct IP table
	GPIO_PinState ip_sel_0_state = HAL_GPIO_ReadPin(GPIOF, IP_SEL_0_Pin);
	LWIP_PLATFORM_DIAG(("IP_SEL_0 state: %d\n", ip_sel_0_state));

	if (slot < (sizeof(ip_table_0) / sizeof(ip_table_0[0]))) {
		if (ip_sel_0_state == GPIO_PIN_SET) {
			// IP_SEL_0 is high, use ip_table_1 for local IP
			IP_ADDRESS[0] = ip_table_1[slot][0];
			IP_ADDRESS[1] = ip_table_1[slot][1];
			IP_ADDRESS[2] = ip_table_1[slot][2];
			IP_ADDRESS[3] = ip_table_1[slot][3];
			LWIP_PLATFORM_DIAG(("Using ip_table_1 for local IP: %d.%d.%d.%d\n",
								IP_ADDRESS[0], IP_ADDRESS[1], IP_ADDRESS[2], IP_ADDRESS[3]));

			// Add 15 to slot number as per requirement
			slot += 15;
			LWIP_PLATFORM_DIAG(("Slot adjusted to: %d\n", slot));

			// Optionally, set remote server IP from ip_table_0
			// Example:
			// udp_set_server_ip(ip_table_0[slot - 15]);
		} else {
			// IP_SEL_0 is low, use ip_table_0 for local IP
			IP_ADDRESS[0] = ip_table_0[slot][0];
			IP_ADDRESS[1] = ip_table_0[slot][1];
			IP_ADDRESS[2] = ip_table_0[slot][2];
			IP_ADDRESS[3] = ip_table_0[slot][3];
			LWIP_PLATFORM_DIAG(("Using ip_table_0 for local IP: %d.%d.%d.%d\n",
								IP_ADDRESS[0], IP_ADDRESS[1], IP_ADDRESS[2], IP_ADDRESS[3]));

			// Optionally, set remote server IP from ip_table_1
			// Example:
			// udp_set_server_ip(ip_table_1[slot]);
		}
	} else {
		// Handle invalid slot number
		printf("Invalid slot number: %d\n", slot);
		// Set a default IP address or handle error as needed
		IP_ADDRESS[0] = 192;
		IP_ADDRESS[1] = 168;
		IP_ADDRESS[2] = 35;
		IP_ADDRESS[3] = 50;  // Default IP address
		slot = 0;  // Reset slot to default
	}

#else
	// If none of the hardware defines are set, retain default IP and slot
	LWIP_PLATFORM_DIAG(("No hardware define set. Using default IP 192.168.35.84 and slot 0.\n"));
	slot = 0;
#endif

/* USER CODE END IP_ADDRESSES */

  /* Initialize the LwIP stack with RTOS */
  tcpip_init( NULL, NULL );

  /* IP addresses initialization without DHCP (IPv4) */
  IP4_ADDR(&ipaddr, IP_ADDRESS[0], IP_ADDRESS[1], IP_ADDRESS[2], IP_ADDRESS[3]);
  IP4_ADDR(&netmask, NETMASK_ADDRESS[0], NETMASK_ADDRESS[1] , NETMASK_ADDRESS[2], NETMASK_ADDRESS[3]);
  IP4_ADDR(&gw, GATEWAY_ADDRESS[0], GATEWAY_ADDRESS[1], GATEWAY_ADDRESS[2], GATEWAY_ADDRESS[3]);

  /* add the network interface (IPv4/IPv6) with RTOS */
  netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);

  /* Registers the default network interface */
  netif_set_default(&gnetif);

  /* We must always bring the network interface up connection or not... */
  netif_set_up(&gnetif);

  /* Set the link callback function, this function is called on change of link status*/
  netif_set_link_callback(&gnetif, ethernet_link_status_updated);

  /* Create the Ethernet link handler thread */
/* USER CODE BEGIN H7_OS_THREAD_NEW_CMSIS_RTOS_V2 */
  memset(&attributes, 0x0, sizeof(osThreadAttr_t));
  attributes.name = "EthLink";
  attributes.stack_size = INTERFACE_THREAD_STACK_SIZE;
  attributes.priority = osPriorityBelowNormal;
  osThreadNew(ethernet_link_thread, &gnetif, &attributes);
/* USER CODE END H7_OS_THREAD_NEW_CMSIS_RTOS_V2 */

/* USER CODE BEGIN 3 */

/* USER CODE END 3 */
}

#ifdef USE_OBSOLETE_USER_CODE_SECTION_4
/* Kept to help code migration. (See new 4_1, 4_2... sections) */
/* Avoid to use this user section which will become obsolete. */
/* USER CODE BEGIN 4 */
/* USER CODE END 4 */
#endif

/**
  * @brief  Notify the User about the network interface config status
  * @param  netif: the network interface
  * @retval None
  */
static void ethernet_link_status_updated(struct netif *netif)
{
  if (netif_is_up(netif))
  {
/* USER CODE BEGIN 5 */
/* USER CODE END 5 */
  }
  else /* netif is down */
  {
/* USER CODE BEGIN 6 */
/* USER CODE END 6 */
  }
}

#if defined ( __CC_ARM )  /* MDK ARM Compiler */
/**
 * Opens a serial device for communication.
 *
 * @param devnum device number
 * @return handle to serial device if successful, NULL otherwise
 */
sio_fd_t sio_open(u8_t devnum)
{
  sio_fd_t sd;

/* USER CODE BEGIN 7 */
  sd = 0; // dummy code
/* USER CODE END 7 */

  return sd;
}

/**
 * Sends a single character to the serial device.
 *
 * @param c character to send
 * @param fd serial device handle
 *
 * @note This function will block until the character can be sent.
 */
void sio_send(u8_t c, sio_fd_t fd)
{
/* USER CODE BEGIN 8 */
/* USER CODE END 8 */
}

/**
 * Reads from the serial device.
 *
 * @param fd serial device handle
 * @param data pointer to data buffer for receiving
 * @param len maximum length (in bytes) of data to receive
 * @return number of bytes actually received - may be 0 if aborted by sio_read_abort
 *
 * @note This function will block until data can be received. The blocking
 * can be cancelled by calling sio_read_abort().
 */
u32_t sio_read(sio_fd_t fd, u8_t *data, u32_t len)
{
  u32_t recved_bytes;

/* USER CODE BEGIN 9 */
  recved_bytes = 0; // dummy code
/* USER CODE END 9 */
  return recved_bytes;
}

/**
 * Tries to read from the serial device. Same as sio_read but returns
 * immediately if no data is available and never blocks.
 *
 * @param fd serial device handle
 * @param data pointer to data buffer for receiving
 * @param len maximum length (in bytes) of data to receive
 * @return number of bytes actually received
 */
u32_t sio_tryread(sio_fd_t fd, u8_t *data, u32_t len)
{
  u32_t recved_bytes;

/* USER CODE BEGIN 10 */
  recved_bytes = 0; // dummy code
/* USER CODE END 10 */
  return recved_bytes;
}
#endif /* MDK ARM Compiler */

