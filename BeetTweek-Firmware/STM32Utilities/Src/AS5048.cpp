/*
 * AS5048.c
 *
 *  Created on: Jan 3, 2020
 *      Author: casht
 */

#include "AS5048.h"
#include "us_delay.h"


void AS5048DeviceInit(AS5048_Device* device,
		SPI_HandleTypeDef* hspi,
		GPIO_TypeDef* chipSelGPIOPort,
		uint16_t chipSelGPIOPin)
{
	device->hspi = hspi;
	device->chipSelGPIOPort = chipSelGPIOPort;
	device->chipSelGPIOPin = chipSelGPIOPin;
}

float AS5048_SPI_ReadAnglef(AS5048_Device* device)
{
	static int angleRead = 0;
	static int firstRead = 1;

	static uint8_t txBuff[2] = {0xFF,0xFF};
	static uint8_t rxBuff[2] = {0x00,0x00};

	int numReadings = 1;

	if(firstRead)
	{
		numReadings = 2;
		firstRead = 0;
	}

	for(int i = 0; i < numReadings; i++)
	{
		HAL_GPIO_WritePin(device->chipSelGPIOPort, device->chipSelGPIOPin, GPIO_PIN_RESET);

		HAL_StatusTypeDef status = HAL_SPI_TransmitReceive(device->hspi, txBuff, rxBuff, 1, 1000);
		angleRead = (rxBuff[1] << 8) | rxBuff[0];


		HAL_GPIO_WritePin(device->chipSelGPIOPort, device->chipSelGPIOPin, GPIO_PIN_SET);

		//only add delay if we are doing multiple readings
		if(i < (numReadings - 1)){
			DelayUS(1);
		}

	}

	angleRead = angleRead & 0x3FFF;

	return angleRead/16383.0;
}
