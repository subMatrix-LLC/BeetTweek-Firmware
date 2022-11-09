/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
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
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32f4xx_hal_tim.h"
#include "stm32f4xx_hal_flash_ex.h"

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */




extern char errorCode[256];


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
#define DRV8313_A_nReset_Pin GPIO_PIN_0
#define DRV8313_A_nReset_GPIO_Port GPIOC
#define Board_Status_LED_Pin GPIO_PIN_2
#define Board_Status_LED_GPIO_Port GPIOC
#define Debug_Output1_Pin GPIO_PIN_3
#define Debug_Output1_GPIO_Port GPIOC
#define DRV8313_A_CURSENSE_ADC_Pin GPIO_PIN_0
#define DRV8313_A_CURSENSE_ADC_GPIO_Port GPIOA
#define DRV8313_B_CURSENSE_ADC_Pin GPIO_PIN_1
#define DRV8313_B_CURSENSE_ADC_GPIO_Port GPIOA
#define COMS_NSS_Pin GPIO_PIN_4
#define COMS_NSS_GPIO_Port GPIOA
#define CAP_SENSE_CHARGE_Pin GPIO_PIN_5
#define CAP_SENSE_CHARGE_GPIO_Port GPIOA
#define DRV8313_A_IN1_Pin GPIO_PIN_6
#define DRV8313_A_IN1_GPIO_Port GPIOA
#define DRV8313_A_IN2_Pin GPIO_PIN_7
#define DRV8313_A_IN2_GPIO_Port GPIOA
#define Debug_Output2_Pin GPIO_PIN_4
#define Debug_Output2_GPIO_Port GPIOC
#define DRV8313_A_IN3_Pin GPIO_PIN_0
#define DRV8313_A_IN3_GPIO_Port GPIOB
#define Prog_USART_TX_Pin GPIO_PIN_10
#define Prog_USART_TX_GPIO_Port GPIOB
#define Prog_USART_RX_Pin GPIO_PIN_11
#define Prog_USART_RX_GPIO_Port GPIOB
#define AS5048_SCK_Pin GPIO_PIN_13
#define AS5048_SCK_GPIO_Port GPIOB
#define AS5048_MISO_Pin GPIO_PIN_14
#define AS5048_MISO_GPIO_Port GPIOB
#define AS5048_MOSI_Pin GPIO_PIN_15
#define AS5048_MOSI_GPIO_Port GPIOB
#define DRV8313_EN1_Pin GPIO_PIN_6
#define DRV8313_EN1_GPIO_Port GPIOC
#define DRV8313_EN2_Pin GPIO_PIN_7
#define DRV8313_EN2_GPIO_Port GPIOC
#define DRV8313_EN3_Pin GPIO_PIN_8
#define DRV8313_EN3_GPIO_Port GPIOC
#define DRV8313_A_nFault_Pin GPIO_PIN_9
#define DRV8313_A_nFault_GPIO_Port GPIOC
#define DEBUG_USART_TX_Pin GPIO_PIN_9
#define DEBUG_USART_TX_GPIO_Port GPIOA
#define DEBUG_USART_RX_Pin GPIO_PIN_10
#define DEBUG_USART_RX_GPIO_Port GPIOA
#define DRV8313_B_nSleep_Pin GPIO_PIN_15
#define DRV8313_B_nSleep_GPIO_Port GPIOA
#define COMS_SPI_SCK_Pin GPIO_PIN_10
#define COMS_SPI_SCK_GPIO_Port GPIOC
#define COMS_SPI_MISO_Pin GPIO_PIN_11
#define COMS_SPI_MISO_GPIO_Port GPIOC
#define COMS_SPI_MOSI_Pin GPIO_PIN_12
#define COMS_SPI_MOSI_GPIO_Port GPIOC
#define DRV8313_A_nSleep_Pin GPIO_PIN_2
#define DRV8313_A_nSleep_GPIO_Port GPIOD
#define AS5048_CS_Pin GPIO_PIN_4
#define AS5048_CS_GPIO_Port GPIOB
#define CROSS_BOARD_GPIO_Pin GPIO_PIN_6
#define CROSS_BOARD_GPIO_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
