/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dac.h"
#include "dma.h"
#include "fatfs.h"
#include "i2c.h"
#include "mdma.h"
#include "rng.h"
#include "sai.h"
#include "sdmmc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Program.h"
#include "stdio.h"
#include "ee24.h"
#include "adcdacbuffers.h"
#include "core_cm7.h"
#include "BootLoaderJump.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define BOARD_VALIDATION_MODE
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

uint8_t workBuffer[_MAX_SS];

FLASH_OBProgramInitTypeDef OBInit;
char errorCode[256] = "NOT CALLED FROM USERCODE";
const char __attribute__((section (".versionIdentifier"))) BoardVersionIdentifier[32] = "BEETTWEEKDSPH7";


volatile int fullyStartedUp = 0;
volatile int panelReversed = 0;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
/* USER CODE BEGIN PFP */
static void MPU_Config(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//static void FS_FileOperationsTEST(void)
//{
//  FRESULT res;                                          /* FatFs function common result code */
//  uint32_t byteswritten, bytesread;                     /* File write/read counts */
//  uint8_t wtext[] = "This is STM32 working with FatFs + uSD"; /* File write buffer */
//  uint8_t rtext[100];                                   /* File read buffer */
//
//  /* Register the file system object to the FatFs module */
//  if(f_mount(&SDFatFS, (TCHAR const*)SDPath, 0) == FR_OK)
//  {
//    /* Create and Open a new text file object with write access */
//	res = f_open(&SDFile, "STM32.TXT", FA_CREATE_ALWAYS | FA_WRITE);
//    if(res == FR_OK)
//    {
//      /* Write data to the text file */
//      res = f_write(&SDFile, wtext, sizeof(wtext), (void *)&byteswritten);
//
//      if((byteswritten > 0) && (res == FR_OK))
//      {
//        /* Close the open text file */
//        f_close(&SDFile);
//
//        /* Open the text file object with read access */
//        if(f_open(&SDFile, "STM32.TXT", FA_READ) == FR_OK)
//        {
//          /* Read data from the text file */
//          res = f_read(&SDFile, rtext, sizeof(rtext), (void *)&bytesread);
//
//          if((bytesread > 0) && (res == FR_OK))
//          {
//            /* Close the open text file */
//            f_close(&SDFile);
//
//            /* Compare read data with the expected data */
//            if((bytesread == byteswritten))
//            {
//              /* Success of the demo: no error occurrence */
//              return;
//            }
//          }
//        }
//      }
//    }
//  }
//  /* Error */
//  Error_Handler();
//}

void ResetBoards()
	{
		  //configure motor reset pin for output
		  GPIO_InitTypeDef GPIO_InitStruct = {0};
		  GPIO_InitStruct.Pin = MOTOR_T_NRST_Pin;
		  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		  GPIO_InitStruct.Pull = GPIO_NOPULL;
		  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		  HAL_GPIO_Init(MOTOR_T_NRST_GPIO_Port, &GPIO_InitStruct);

		  //configure dsp (self) reset pin for output
		  GPIO_InitTypeDef GPIO_SelfInitStruct = {0};
		  GPIO_SelfInitStruct.Pin = SELF_T_NRST_Pin;
		  GPIO_SelfInitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		  GPIO_SelfInitStruct.Pull = GPIO_NOPULL;
		  GPIO_SelfInitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		  HAL_GPIO_Init(SELF_T_NRST_GPIO_Port, &GPIO_SelfInitStruct);



			  HAL_GPIO_WritePin(MOTOR_T_NRST_GPIO_Port, MOTOR_T_NRST_Pin, GPIO_PIN_RESET);
			  HAL_GPIO_WritePin(SELF_T_NRST_GPIO_Port, SELF_T_NRST_Pin, GPIO_PIN_RESET);
			  HAL_Delay(100);
			  HAL_GPIO_WritePin(MOTOR_T_NRST_GPIO_Port, MOTOR_T_NRST_Pin, GPIO_PIN_SET);
			  HAL_GPIO_WritePin(SELF_T_NRST_GPIO_Port, SELF_T_NRST_Pin, GPIO_PIN_SET);


		  //configure  motor reset  pin back to input
		  GPIO_InitStruct.Pin = MOTOR_T_NRST_Pin;
		  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		  GPIO_InitStruct.Pull = GPIO_NOPULL;
		  HAL_GPIO_Init(MOTOR_T_NRST_GPIO_Port, &GPIO_InitStruct);


		  //configure  dsp (self) reset  pin back to input
		  GPIO_SelfInitStruct.Pin = SELF_T_NRST_Pin;
		  GPIO_SelfInitStruct.Mode = GPIO_MODE_INPUT;
		  GPIO_SelfInitStruct.Pull = GPIO_NOPULL;
		  HAL_GPIO_Init(SELF_T_NRST_GPIO_Port, &GPIO_SelfInitStruct);
	}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  //uint32_t div0tripEnabled = SCB->CCR & SCB_CCR_DIV_0_TRP_Msk;
  //uint32_t unaligntripEnabled = SCB->CCR & SCB_CCR_UNALIGN_TRP_Msk;

  MPU_Config();
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  UserCodeInit1();
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

/* Configure the peripherals common clocks */
  PeriphCommonClock_Config();

  /* USER CODE BEGIN SysInit */

  initSAI1 = 0;
  initSAI2 = 0;


  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_MDMA_Init();
  MX_DAC1_Init();
  MX_TIM4_Init();
  MX_USART3_UART_Init();
  MX_TIM2_Init();
  MX_RNG_Init();
  MX_I2C1_Init();
  MX_TIM5_Init();
  MX_SAI1_Init();
  MX_SDMMC1_SD_Init();
  MX_SPI6_Init();
  MX_I2C2_Init();
  MX_USART2_UART_Init();
  MX_SPI2_Init();
  MX_SAI2_Init();
  MX_FATFS_Init();
  MX_TIM3_Init();
  MX_SPI5_Init();
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */

  //enable SAI2 first, then SAI1 as it is recommended to enable the slaves first.
  initSAI1 = 1;
  initSAI2 = 1;

  MX_SAI2_Init();
  MX_SAI1_Init();


  //HAL_StatusTypeDef status = HAL_I2C_Init(&hi2c1);


#if defined(BOARD_VALIDATION_MODE) && defined(COMBINEDBOARD)

  BoardVerificationInit();
  while(true)
  {
	  BoardVerificationLoopUpdate();
  }
#endif


//if button 7 is held, jump to stm32 bootloader
  if(HAL_GPIO_ReadPin(PUSH_BUTTON_OUT_3_GPIO_Port, PUSH_BUTTON_OUT_3_Pin))
  {
//	  HAL_GPIO_WritePin(MULTI_BOARD_BOOT_GPIO_Port, MULTI_BOARD_BOOT_Pin, GPIO_PIN_SET);
//	  HAL_Delay(1000);
//
//	  ResetBoards();
//
	  JumpToBootloader();
  }


  UserCodeInit2();


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	 ProgramMainLoopUpdate();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	// uint8_t data = 0x83;
	 //HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(&hi2c2, 0, &data, 1, 100);


	 //if(status != HAL_OK)
	//	 Error_Handler();
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 60;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SDMMC|RCC_PERIPHCLK_SAI1
                              |RCC_PERIPHCLK_SAI2|RCC_PERIPHCLK_SPI2;
  PeriphClkInitStruct.PLL2.PLL2M = 4;
  PeriphClkInitStruct.PLL2.PLL2N = 10;
  PeriphClkInitStruct.PLL2.PLL2P = 1;
  PeriphClkInitStruct.PLL2.PLL2Q = 2;
  PeriphClkInitStruct.PLL2.PLL2R = 4;
  PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_3;
  PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOMEDIUM;
  PeriphClkInitStruct.PLL2.PLL2FRACN = 0;
  PeriphClkInitStruct.PLL3.PLL3M = 32;
  PeriphClkInitStruct.PLL3.PLL3N = 150;
  PeriphClkInitStruct.PLL3.PLL3P = 2;
  PeriphClkInitStruct.PLL3.PLL3Q = 2;
  PeriphClkInitStruct.PLL3.PLL3R = 2;
  PeriphClkInitStruct.PLL3.PLL3RGE = RCC_PLL3VCIRANGE_1;
  PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOWIDE;
  PeriphClkInitStruct.PLL3.PLL3FRACN = 0;
  PeriphClkInitStruct.SdmmcClockSelection = RCC_SDMMCCLKSOURCE_PLL2;
  PeriphClkInitStruct.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLL3;
  PeriphClkInitStruct.Sai23ClockSelection = RCC_SAI23CLKSOURCE_PLL3;
  PeriphClkInitStruct.Spi123ClockSelection = RCC_SPI123CLKSOURCE_PLL2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
/**
  * @brief  Configure the MPU attributes as Normal Non Cacheable for Flash Bank B sectors 6 and 7 (used for EEPROM emulation).
  * @note   The Base Address is 0x081C0000 .
  *         The Configured Region Size is 256KB (2 x 128 KB for 2 flash sectors).
  * @param  None
  * @retval None
  */
static void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct;

  /* Disable the MPU */
  HAL_MPU_Disable();

  /* Configure the MPU attributes as Normal Non Cacheable for Flash Bank B sectros 6 and 7 */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = ADDR_FLASH_SECTOR_6_BANK2;
  MPU_InitStruct.Size = MPU_REGION_SIZE_256KB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Enable the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}










/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

	//generate an assert that will stop the debugger if connected...
	//look at the stack...
	//volatile int inf = 1/0;

	int i = 0;
	while(true)
	{
		printf("HAL Error Hander: ");
		printf(errorCode);
		printf("\r\n");
		HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

		//if(HAL_GPIO_ReadPin(PUSH_BUTTON_OUT_3_GPIO_Port, PUSH_BUTTON_OUT_3_Pin) && HAL_GPIO_ReadPin(PUSH_BUTTON_OUT_4_GPIO_Port, PUSH_BUTTON_OUT_4_Pin))
		//	ResetBoards();

		ErrorLoop();
		i++;

		if(i >= 1)
		{
			HAL_SD_DeInit(&hsd1);//attempt to "SHUTDOWN" the sdcard, in hopes of better state next reboot.
			ResetBoards();
		}


	}
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	 printf("Wrong parameters value: file %s on line %u\r\n", file, (unsigned int)line);

  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
