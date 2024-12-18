/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define TWS_DATA_Pin GPIO_PIN_5
#define TWS_DATA_GPIO_Port GPIOB
#define MIIM_MDIO_Pin GPIO_PIN_7
#define MIIM_MDIO_GPIO_Port GPIOB
#define MIIM_MDC_Pin GPIO_PIN_8
#define MIIM_MDC_GPIO_Port GPIOB
#define IP_SEL_0_Pin GPIO_PIN_4
#define IP_SEL_0_GPIO_Port GPIOF
#define IP_SEL_1_Pin GPIO_PIN_5
#define IP_SEL_1_GPIO_Port GPIOF
#define IP_SEL_2_Pin GPIO_PIN_6
#define IP_SEL_2_GPIO_Port GPIOF
#define IP_SEL_3_Pin GPIO_PIN_7
#define IP_SEL_3_GPIO_Port GPIOF
#define IP_SEL_4_Pin GPIO_PIN_8
#define IP_SEL_4_GPIO_Port GPIOF
#define TWS_CLK_Pin GPIO_PIN_5
#define TWS_CLK_GPIO_Port GPIOA

#if defined(CMB_HW) || defined(CMBTS_HW)
#define LED_Pin GPIO_PIN_10
#define LED_GPIO_Port GPIOD
#endif

#if defined(CMM_HW)
#define LED_Pin GPIO_PIN_14
#define LED_GPIO_Port GPIOD
#endif

/* USER CODE BEGIN Private defines */

extern TIM_HandleTypeDef htim1;

int32_t ETH_PHY_IO_WriteReg(uint32_t DevAddr, uint32_t RegAddr, uint32_t RegVal);

#define USE_IAP_TFTP

#define USER_FLASH_FIRST_PAGE_ADDRESS 0x08100000 /* Starting address of user application in Flash */
#define USER_FLASH_LAST_PAGE_ADDRESS  0x081E0000
#define USER_FLASH_END_ADDRESS        0x081FFFFF
#define APPLICATION_ADDRESS USER_FLASH_FIRST_PAGE_ADDRESS /* Define the application start address */
#define IAP_FLAG_ADDRESS  0x38000000  // Address in SRAM4 for IAP flag
#define IAP_FLAG_VALUE    0xDEADBEEF  // Unique value to indicate IAP mode request
__attribute__((section(".noinit"))) extern volatile uint32_t iap_flag;
/* UserID and Password definition */
#define USERID       "user"
#define PASSWORD     "stm32"
#define LOGIN_SIZE   (17+ sizeof(USERID) + sizeof(PASSWORD))

extern int slot;  // Adding the slot variable here

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
