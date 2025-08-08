/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
extern char errorCode[256];
extern volatile int panelReversed;
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
