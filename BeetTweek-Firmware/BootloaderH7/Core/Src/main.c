/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "usb_otg.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "ProgressIndication.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define FIRMWARE_UPDATE_FILENAME "FIRMWARE.bin"
#define APPLICATION_ADDRESS (0x08040000)
#define APPLICATION_SECTOR (2)
#define APPLICATION_NSECTORS (14)
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
char errorCode[256] = "NOT CALLED FROM USERCODE";
volatile int panelReversed = 0;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static void goto_application( void )
{
	FATFS_UnLinkDriver(SDPath);
	HAL_SD_DeInit(&hsd1);


	printf("Jumping To Application..\r\n");



	void (*app_reset_handler) (void) = (void*) (*(volatile uint32_t *) (APPLICATION_ADDRESS + 4));


	//__set_MSP(*(uint32_t *)APPLICATION_ADDRESS);

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);

	app_reset_handler();

}

int DoFirmwareUpdateFromSDCard()
{
	printf("Checking SD Card For Firmware Update..\r\n");
	UpdateProgressPercent(0, 1.0f, 1.0f, 0.0f);

	//Get SD and FatFs running.
	if(f_mount(&SDFatFS, (TCHAR const*)SDPath, 0) != FR_OK)
	{
		Error_Handler();
	}


	FIL FIRMWARE_FILE;
	FRESULT fr;
	FILINFO fno;
	int exists;
	fr = f_stat(FIRMWARE_UPDATE_FILENAME, &fno);
	switch (fr) {
		case FR_OK: exists = 1; break;
		default: exists = 0;
	}

	if(!exists)
		return 0;

	FRESULT res = f_open(&FIRMWARE_FILE, FIRMWARE_UPDATE_FILENAME, FA_READ);
	if(res != FR_OK)
		return 0;




	HAL_FLASH_Unlock();

	int retries = 3;
	int dataVerified = 1;
	do {
		dataVerified = 1;
		printf("	Erasing Application Flash (Bank1)...\r\n");
		FLASH_EraseInitTypeDef EraseInitStruct;
		uint32_t SectorError;

		EraseInitStruct.TypeErase     = FLASH_TYPEERASE_SECTORS;
		EraseInitStruct.Sector        = APPLICATION_SECTOR;
		EraseInitStruct.Banks         = FLASH_BANK_1;
		EraseInitStruct.NbSectors     = APPLICATION_NSECTORS - 8;
		EraseInitStruct.VoltageRange  = FLASH_VOLTAGE_RANGE_3;

		HAL_StatusTypeDef ret = HAL_FLASHEx_Erase( &EraseInitStruct, &SectorError );
		if((ret != HAL_OK) || (SectorError != 0xFFFFFFFF))
			Error_Handler();

		UpdateProgressPercent(10,  1.0f, 1.0f, 0.0f);

		printf("	Erasing Application Flash (Bank2)...\r\n");
		EraseInitStruct.TypeErase     = FLASH_TYPEERASE_SECTORS;
		EraseInitStruct.Sector        = 0;
		EraseInitStruct.Banks         = FLASH_BANK_2;
		EraseInitStruct.NbSectors     = 8;                    //erase all sectors on bank2
		EraseInitStruct.VoltageRange  = FLASH_VOLTAGE_RANGE_3;

		ret = HAL_FLASHEx_Erase( &EraseInitStruct, &SectorError );
		if((ret != HAL_OK) || (SectorError != 0xFFFFFFFF))
			Error_Handler();

		UpdateProgressPercent(20, 1.0f, 1.0f, 0.0f);

		printf("	Reading and Programming...\r\n");

		fr = f_lseek(&FIRMWARE_FILE, 0);
		if(fr != FR_OK)
			Error_Handler();

		int flashAddressByteOffset = 0;
		int progress = 0;
		while(f_eof(&FIRMWARE_FILE) == 0)
		{
			//read 256bits (H7 flashword size)
			uint8_t data[4*8];
			UINT bytesRead;
			fr = f_read(&FIRMWARE_FILE, data, 4*8, &bytesRead);
			if(fr != FR_OK)
				Error_Handler();

			//write 256bits

			FLASH->CR1 |= FLASH_VOLTAGE_RANGE_3;
			FLASH->CR2 |= FLASH_VOLTAGE_RANGE_3;
			ret = HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, APPLICATION_ADDRESS+flashAddressByteOffset, (uint32_t)(&data[0]));
			if(ret != HAL_OK)
				Error_Handler();

			flashAddressByteOffset+=32;
			progress++;
			if(progress >= 60)
				progress = 60;

			UpdateProgressPercent(progress, 1.0f, 1.0f, 0.0f);
		}


		printf("	Verifying...\r\n");
		UpdateProgressPercent(80, 1.0f, 1.0f, 0.0f);


		fr = f_lseek(&FIRMWARE_FILE, 0);
		if(fr != FR_OK)
			Error_Handler();



		uint32_t flashAddress = APPLICATION_ADDRESS;
		while(f_eof(&FIRMWARE_FILE) == 0)
		{
			//read 1byte from file
			uint8_t data;
			UINT bytesRead;
			fr = f_read(&FIRMWARE_FILE, &data, 1, &bytesRead);
			if(fr != FR_OK)
				Error_Handler();

			uint8_t flashVal = *(uint32_t*)flashAddress;

			if(data != flashVal)
			{
				dataVerified = 0;
				break;
			}

			flashAddress++;
		}



		if(dataVerified)
		{
			printf("	Flash Verified.\r\n");
		}
		else
		{
			printf("	Flash Verification Error, retries left: %d\r\n", retries-1);

		}

		retries--;
	}while(dataVerified == 0 && retries > 0);


	HAL_FLASH_Lock();

	if(dataVerified)
	{
		printf("	Flash Programmed.\r\n");

		fr = f_close(&FIRMWARE_FILE);
		if(fr != FR_OK)
			Error_Handler();

		printf("	Removing Firmware File From SD..\r\n");

		fr = f_unlink("FIRMWARE.bin");
		if(fr != FR_OK)
			Error_Handler();


		//unmount
		f_mount(0, (TCHAR const*)SDPath, 0);
	}
	else
	{
		printf("SD Programming Attempts Failed.\r\n");
		Error_Handler();
	}

	return 1;
}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */


  //goto_application( );

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

/* Configure the peripherals common clocks */
  PeriphCommonClock_Config();

  /* USER CODE BEGIN SysInit */

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
  MX_USB_OTG_FS_PCD_Init();
  /* USER CODE BEGIN 2 */

  printf("Starting Bootloader..\r\n");

  InitProgress();


	int sdResult = DoFirmwareUpdateFromSDCard();
	if(!sdResult)
	{
	  printf("No SD Firmware Update.\r\n");
	  for(int i = 0; i < 100; i++)
	  {
		  HAL_Delay(2);
		  UpdateProgressPercent(i, 0.0f, 0.0f, 1.0f);
	  }
	}
	else
	{
		UpdateProgressPercent(100, 1.0f, 0.5f, 0.25f);
	}



	DeInitProgess();

  goto_application();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
  /** Macro to configure the PLL clock source
  */
  __HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSI);
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

int __io_putchar(int ch)
{
	HAL_StatusTypeDef status =  HAL_UART_Transmit(&huart3, (uint8_t*)&ch, 1, HAL_MAX_DELAY);
	//CDC_Transmit_FS((uint8_t*)&ch, 1);
	return ch;
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
  __disable_irq();
  while (1)
  {
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

