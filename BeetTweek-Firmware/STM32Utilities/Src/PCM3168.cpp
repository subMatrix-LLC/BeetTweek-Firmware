/*
 * PCM3168.cpp
 *
 *  Created on: Jul 19, 2021
 *      Author: casht
 */


#include "PCM3168.h"
#include "stdio.h"
#include "string.h"

uint8_t PCM3168Read(uint8_t addr)
{
	uint16_t sendBuff = 0;
	uint16_t rxBuff   = 0;
	uint8_t data      = 0;

	sendBuff = 0;
	sendBuff = ((0b10000000)//read
				| (addr & 0b01111111)) << 8;

	HAL_StatusTypeDef status = HAL_SPI_TransmitReceive(&hspi2, (uint8_t*)&sendBuff, (uint8_t*)&rxBuff, 1, 1000);
	if(status != HAL_OK)
	{
		strcpy(errorCode, "PCM3168Read Error");
		Error_Handler();
	}

	return uint8_t(rxBuff & 0b0000000011111111);
}

void PCM3168Write(uint8_t addr, uint8_t data)
{
	uint16_t sendBuff = 0;
	uint16_t rxBuff   = 0;

	sendBuff = 0;
	sendBuff = ((0b00000000)//write
				| (addr & 0b01111111)) << 8;
	sendBuff |= data;

	HAL_StatusTypeDef status = HAL_SPI_TransmitReceive(&hspi2, (uint8_t*)&sendBuff, (uint8_t*)&rxBuff, 1, 1000);
	if(status != HAL_OK)
	{
		strcpy(errorCode, "PCM3168Write Error");
		Error_Handler();
	}

	//verify
	uint8_t v = PCM3168Read(addr);
	if(data != v)
	{
		strcpy(errorCode, "PCM3168Write Error");
		Error_Handler();
	}

}


void PCM3168_Toggle48DeEmph()
{
	//turn on 48khz digital de-emphasis
	uint8_t reg = PCM3168Read(0x46);

	if(((reg & 0b00110000) >> 4 ) == 0)
	{
		reg &= 0b11011111;
		reg |= 0b00010000;
		PCM3168Write(0x46, reg);
	}
	else
	{
		reg = 0;
		PCM3168Write(0x46, reg);
	}
}

