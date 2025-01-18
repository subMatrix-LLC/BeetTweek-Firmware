/*
 * DACX0504.h
 *
 *  Created on: Oct 7, 2020
 *      Author: casht
 */

#ifndef INC_DAC80504_H_
#define INC_DAC80504_H_

#include "main.h"
#include "us_delay.h"


#define DACX0504_DMA_SIZE 64
#define DACX0504_DMA_32BITWORDSIZE

typedef struct DACX0504Chip
{
	SPI_HandleTypeDef* hspi;
	GPIO_TypeDef* CSGPIOPort;
	uint16_t CSGPIOPin;

#ifdef DACX0504_DMA_32BITWORDSIZE
	uint32_t DMABuffer[DACX0504_DMA_SIZE];
#else
	uint8_t DMABuffer[DACX0504_DMA_SIZE];
#endif
	uint8_t DMAIdx;
} DACX0504Chip;



enum DACX0504Address {
	DACX0504Address_NOP = 0,
	DACX0504Address_DEVICEID,
	DACX0504Address_SYNC,
	DACX0504Address_CONFIG,
	DACX0504Address_GAIN,
	DACX0504Address_TRIGGER,
	DACX0504Address_BRDCAST,
	DACX0504Address_STATUS,
	DACX0504Address_DAC0,
	DACX0504Address_DAC1,
	DACX0504Address_DAC2,
	DACX0504Address_DAC3
};

void DACX0504ChipInit(DACX0504Chip* chip, SPI_HandleTypeDef* hspi, GPIO_TypeDef* CSGPIOPort, uint16_t CSGPIOPin);

void DACX0504SendCommand(DACX0504Chip* chip, uint8_t read, DACX0504Address address, uint16_t data, uint32_t* rxPrevData = nullptr);


void DACX0504QueueCommand_DMA(DACX0504Chip* chip,  DACX0504Address address, uint16_t data);

void DACX0504StartSendQueue_DMA(DACX0504Chip* chip);

void DACX0504WaitForDMAComplete(DACX0504Chip* chip);

void DACX0504ParseDeviceId(uint8_t rxData[3],
		uint8_t* resolutionCode,
		uint8_t* channels,
		uint8_t* reset,
		uint8_t* versionIDOut);

void DACX0504ParseConfigRegister(uint8_t rxData[3],
		uint8_t* ALM_SEL,
		uint8_t* ALM_EN,
		uint8_t* CRC_EN,
		uint8_t* FSDO,
		uint8_t* DSDO,
		uint8_t* REF_PWDWN,
		uint8_t* DAC0_PWDWN,
		uint8_t* DAC1_PWDWN,
		uint8_t* DAC2_PWDWN,
		uint8_t* DAC3_PWDWN);

void DACX0504ResetAllRegisters(DACX0504Chip* chip);

#endif /* INC_DAC80504_H_ */
