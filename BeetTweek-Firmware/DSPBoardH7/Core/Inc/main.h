/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#include "stm32h7xx_hal_flash_ex.h"
#include "SharedDefines.h"

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
//#define ENABLE_SCRATCH_BUFFER
#define _EEPROM_ASFILE
#define UART3_RX_BuffSize 2048
//#define NUCLEO_TEST



static const uint32_t firmwareVersion = 17;
extern volatile int fullyStartedUp;
extern volatile int panelReversed;

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
//#define COMBINEDBOARD

#if defined(COMBINEDBOARD)
#define VIRTUALCOMUSB
#endif
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void ResetBoards();
void WaitSDCardReady();



extern char errorCode[256];
#define FLASH_BASE_ADDR      (uint32_t)(FLASH_BASE)
#define FLASH_END_ADDR       (uint32_t)(0x081FFFFF)

/* Base address of the Flash sectors Bank 1 */
#define ADDR_FLASH_SECTOR_0_BANK1     ((uint32_t)0x08000000) /* Base @ of Sector 0, 128 Kbytes */
#define ADDR_FLASH_SECTOR_1_BANK1     ((uint32_t)0x08020000) /* Base @ of Sector 1, 128 Kbytes */
#define ADDR_FLASH_SECTOR_2_BANK1     ((uint32_t)0x08040000) /* Base @ of Sector 2, 128 Kbytes */
#define ADDR_FLASH_SECTOR_3_BANK1     ((uint32_t)0x08060000) /* Base @ of Sector 3, 128 Kbytes */
#define ADDR_FLASH_SECTOR_4_BANK1     ((uint32_t)0x08080000) /* Base @ of Sector 4, 128 Kbytes */
#define ADDR_FLASH_SECTOR_5_BANK1     ((uint32_t)0x080A0000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6_BANK1     ((uint32_t)0x080C0000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7_BANK1     ((uint32_t)0x080E0000) /* Base @ of Sector 7, 128 Kbytes */

/* Base address of the Flash sectors Bank 2 */
#define ADDR_FLASH_SECTOR_0_BANK2     ((uint32_t)0x08100000) /* Base @ of Sector 0, 128 Kbytes */
#define ADDR_FLASH_SECTOR_1_BANK2     ((uint32_t)0x08120000) /* Base @ of Sector 1, 128 Kbytes */
#define ADDR_FLASH_SECTOR_2_BANK2     ((uint32_t)0x08140000) /* Base @ of Sector 2, 128 Kbytes */
#define ADDR_FLASH_SECTOR_3_BANK2     ((uint32_t)0x08160000) /* Base @ of Sector 3, 128 Kbytes */
#define ADDR_FLASH_SECTOR_4_BANK2     ((uint32_t)0x08180000) /* Base @ of Sector 4, 128 Kbytes */
#define ADDR_FLASH_SECTOR_5_BANK2     ((uint32_t)0x081A0000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6_BANK2     ((uint32_t)0x081C0000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7_BANK2     ((uint32_t)0x081E0000) /* Base @ of Sector 7, 128 Kbytes */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define AS5048_CS_Pin GPIO_PIN_6
#define AS5048_CS_GPIO_Port GPIOF
#define AS5048_SCK_Pin GPIO_PIN_7
#define AS5048_SCK_GPIO_Port GPIOF
#define AS5048_MISO_Pin GPIO_PIN_8
#define AS5048_MISO_GPIO_Port GPIOF
#define AS5048_MOSI_Pin GPIO_PIN_9
#define AS5048_MOSI_GPIO_Port GPIOF
#define SWITCH_LEVEL_DAC_Pin GPIO_PIN_4
#define SWITCH_LEVEL_DAC_GPIO_Port GPIOA
#define DRV8313_IN1_Pin GPIO_PIN_6
#define DRV8313_IN1_GPIO_Port GPIOA
#define DRV8313_IN2_Pin GPIO_PIN_7
#define DRV8313_IN2_GPIO_Port GPIOA
#define DRV8313_EN1_Pin GPIO_PIN_4
#define DRV8313_EN1_GPIO_Port GPIOC
#define DRV8313_EN2_Pin GPIO_PIN_5
#define DRV8313_EN2_GPIO_Port GPIOC
#define LED_Pin GPIO_PIN_0
#define LED_GPIO_Port GPIOB
#define DRV8313_IN3_Pin GPIO_PIN_1
#define DRV8313_IN3_GPIO_Port GPIOB
#define DRV8313_EN3_Pin GPIO_PIN_2
#define DRV8313_EN3_GPIO_Port GPIOB
#define DRV8313_A_nRESET_Pin GPIO_PIN_11
#define DRV8313_A_nRESET_GPIO_Port GPIOF
#define DRV8313_A_nFAULT_Pin GPIO_PIN_12
#define DRV8313_A_nFAULT_GPIO_Port GPIOF
#define PUSH_BUTTON_FUNC_Pin GPIO_PIN_14
#define PUSH_BUTTON_FUNC_GPIO_Port GPIOF
#define PUSH_BUTTON_FUNC2_Pin GPIO_PIN_15
#define PUSH_BUTTON_FUNC2_GPIO_Port GPIOF
#define PUSH_BUTTON_IN_1_Pin GPIO_PIN_7
#define PUSH_BUTTON_IN_1_GPIO_Port GPIOE
#define PUSH_BUTTON_IN_2_Pin GPIO_PIN_8
#define PUSH_BUTTON_IN_2_GPIO_Port GPIOE
#define PUSH_BUTTON_IN_3_Pin GPIO_PIN_9
#define PUSH_BUTTON_IN_3_GPIO_Port GPIOE
#define PUSH_BUTTON_IN_4_Pin GPIO_PIN_10
#define PUSH_BUTTON_IN_4_GPIO_Port GPIOE
#define PUSH_BUTTON_OUT_1_Pin GPIO_PIN_12
#define PUSH_BUTTON_OUT_1_GPIO_Port GPIOE
#define PUSH_BUTTON_OUT_2_Pin GPIO_PIN_13
#define PUSH_BUTTON_OUT_2_GPIO_Port GPIOE
#define PUSH_BUTTON_OUT_3_Pin GPIO_PIN_14
#define PUSH_BUTTON_OUT_3_GPIO_Port GPIOE
#define PUSH_BUTTON_OUT_4_Pin GPIO_PIN_15
#define PUSH_BUTTON_OUT_4_GPIO_Port GPIOE
#define PCM3168_RST_Pin GPIO_PIN_8
#define PCM3168_RST_GPIO_Port GPIOD
#define PCM3168_OVF_Pin GPIO_PIN_9
#define PCM3168_OVF_GPIO_Port GPIOD
#define LED_DATA_OUT_Pin GPIO_PIN_12
#define LED_DATA_OUT_GPIO_Port GPIOD
#define MOTOR_BOARD_NSS_Pin GPIO_PIN_8
#define MOTOR_BOARD_NSS_GPIO_Port GPIOG
#define DIP_SW_2_Pin GPIO_PIN_0
#define DIP_SW_2_GPIO_Port GPIOD
#define DIP_SW_4_Pin GPIO_PIN_1
#define DIP_SW_4_GPIO_Port GPIOD
#define DIP_SW_3_Pin GPIO_PIN_3
#define DIP_SW_3_GPIO_Port GPIOD
#define SD_SENSE_Pin GPIO_PIN_4
#define SD_SENSE_GPIO_Port GPIOD
#define MOTOR_T_NRST_Pin GPIO_PIN_9
#define MOTOR_T_NRST_GPIO_Port GPIOG
#define MULTI_BOARD_BOOT_Pin GPIO_PIN_10
#define MULTI_BOARD_BOOT_GPIO_Port GPIOG
#define SELF_T_NRST_Pin GPIO_PIN_11
#define SELF_T_NRST_GPIO_Port GPIOG
#define MOTOR_BOARD_SPI_MISO_Pin GPIO_PIN_12
#define MOTOR_BOARD_SPI_MISO_GPIO_Port GPIOG
#define MOTOR_BOARD_SPI_CLK_Pin GPIO_PIN_13
#define MOTOR_BOARD_SPI_CLK_GPIO_Port GPIOG
#define MOTOR_BOARD_SPI_MOSI_Pin GPIO_PIN_14
#define MOTOR_BOARD_SPI_MOSI_GPIO_Port GPIOG
#define CROSS_BOARD_GPIO_Pin GPIO_PIN_6
#define CROSS_BOARD_GPIO_GPIO_Port GPIOB
#define LC24C_SDA_Pin GPIO_PIN_7
#define LC24C_SDA_GPIO_Port GPIOB
#define LC24C_SCL_Pin GPIO_PIN_8
#define LC24C_SCL_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
