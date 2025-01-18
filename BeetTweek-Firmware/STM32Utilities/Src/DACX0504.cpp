/*
 * DACX0504.cpp
 *
 *  Created on: Nov 1, 2020
 *      Author: casht
 */

#include "DACX0504.h"
#include "spi.h"
#include "stdio.h"
#include "string.h"

void DACX0504ChipInit(DACX0504Chip* chip, SPI_HandleTypeDef* hspi, GPIO_TypeDef* CSGPIOPort, uint16_t CSGPIOPin)
{
	chip->hspi = hspi;
	chip->CSGPIOPort = CSGPIOPort;
	chip->CSGPIOPin = CSGPIOPin;

	for(int i = 0; i < DACX0504_DMA_SIZE; i++)
	{
		chip->DMABuffer[i] = 0;
	}
	chip->DMAIdx = 0;
}



void DACX0504SendCommand(DACX0504Chip* chip, uint8_t read, DACX0504Address address, uint16_t data, uint32_t* rxPrevData )
{

	//wait for any ongoing DMA to complete
	while(HAL_SPI_GetState(chip->hspi) != HAL_SPI_StateTypeDef::HAL_SPI_STATE_READY)
	{}



	  uint8_t dataFrame[3];
	  uint8_t addressBits = address;
	  uint16_t dataBits = data;
	  uint8_t readwriteBits = read;

	  dataFrame[0] = ((readwriteBits << 7) & 0x80)  | (addressBits & 0x0F);
	  dataFrame[1] = (dataBits >> 8);  //MSB
	  dataFrame[2] = (dataBits & 0xFF);//LSB

	  HAL_StatusTypeDef status;


	  if(rxPrevData == nullptr)
	  {
#ifdef SPI_DATASIZE_24BIT
		 if(chip->hspi->Init.DataSize == SPI_DATASIZE_24BIT)
		 {
			 //transmit data in 1 32 bit chunk.
			 uint32_t data32Bit = dataFrame[2] | (dataFrame[1] << 8) | (dataFrame[0] << 16);

			 status = HAL_SPI_Transmit(chip->hspi,  (uint8_t*)&data32Bit, 1, 1000);
			 if(status != HAL_OK)
			 {
				 strcpy(errorCode, "DACX0504SendCommand");
				 Error_Handler();
			 }
		 }
#endif
	  }
	  else
	  {
#ifdef SPI_DATASIZE_24BIT
		 if(chip->hspi->Init.DataSize == SPI_DATASIZE_24BIT)
		 {
			 //transmit data in 1 32 bit chunk.
			 uint32_t data32Bit = dataFrame[2] | (dataFrame[1] << 8) | (dataFrame[0] << 16);

			 status = HAL_SPI_TransmitReceive(chip->hspi, (uint8_t*)&data32Bit, (uint8_t*)rxPrevData, 1, 1000);
			 if(status != HAL_OK)
			 {
				 strcpy(errorCode, "DACX0504SendCommand");
				 Error_Handler();
			 }
		 }
#endif
	  }



}

void DACX0504QueueCommand_DMA(DACX0504Chip* chip,  DACX0504Address address, uint16_t data)
{
	  if(chip->DMAIdx >= DACX0504_DMA_SIZE)
	  {
		  strcpy(errorCode, "DACX0504QueueCommand_DMA");
		  Error_Handler();
	  }


	  uint8_t addressBits = address;
	  uint16_t dataBits = data;
	  uint8_t readwriteBits = 0;

#ifdef DACX0504_DMA_32BITWORDSIZE
	  chip->DMABuffer[chip->DMAIdx] = ((((readwriteBits << 7) & 0x80)  | (addressBits & 0x0F)) << 16) | ((dataBits >> 8) << 8) |  (dataBits & 0xFF);
      chip->DMAIdx += 1;
#else
	  chip->DMABuffer[chip->DMAIdx] = ((readwriteBits << 7) & 0x80)  | (addressBits & 0x0F);
	  chip->DMABuffer[chip->DMAIdx+1] = (dataBits >> 8);  //MSB
	  chip->DMABuffer[chip->DMAIdx+2] = (dataBits & 0xFF);//LSB

	  chip->DMAIdx += 3;
#endif

}


void DACX0504StartSendQueue_DMA(DACX0504Chip* chip)
{
	if(chip->DMAIdx == 0)
		return;//nothing to send.

	//HAL_StatusTypeDef status = HAL_SPI_Transmit_DMA(chip->hspi, (uint8_t*)chip->DMABuffer, chip->DMAIdx);
	//HAL_StatusTypeDef status = HAL_SPI_Transmit_IT(chip->hspi, (uint8_t*)chip->DMABuffer, chip->DMAIdx);
	//HAL_StatusTypeDef status = HAL_SPI_Transmit(chip->hspi, (uint8_t*)chip->DMABuffer, chip->DMAIdx, 1000);

	HAL_StatusTypeDef status = HAL_SPI_Transmit(chip->hspi, (uint8_t*)chip->DMABuffer, chip->DMAIdx, 1000);
	if(status != HAL_OK)
	{
		strcpy(errorCode,"DACX0504StartSendQueue_DMA");
		//Error_Handler();
	}



	//SPI2->TXDR = chip->DMABuffer[0];

	//SPI2->CR1 |= SPI_CR1_CSTART;
	//while((SPI2->SR & SPI_SR_TXC) == 0)
	//{
	      //while transmit not complete


	//      ;
	//




	chip->DMAIdx = 0;
}

void DACX0504WaitForDMAComplete(DACX0504Chip* chip)
{
	while(chip->hspi->State != HAL_SPI_STATE_READY)
	{
	}
}

void DACX0504ParseDeviceId(uint8_t rxData[3], uint8_t* resolutionCode, uint8_t* channels, uint8_t* reset, uint8_t* versionIDOut)
{
	*channels = rxData[1] & 0b00001111;
	*resolutionCode = (rxData[1] & 0b01110000) >> 4;
	*reset = (rxData[2] & 0b10000000) >> 7;
	*versionIDOut = rxData[2] & 0x3;
}
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
		uint8_t* DAC3_PWDWN)
{
	*ALM_SEL = (rxData[1] & 0b00100000) >> 5;
	*ALM_EN = (rxData[1] & 0b00010000) >> 4;
	*CRC_EN = (rxData[1] & 0b00001000) >> 3;
	*FSDO = (rxData[1] & 0b00000100) >> 2;
	*DSDO = (rxData[1] & 0b00000010) >> 1;
	*REF_PWDWN = rxData[1] & 0b00000001;


	*DAC3_PWDWN = (rxData[2] & 0b00001000) >> 3;
	*DAC2_PWDWN = (rxData[2] & 0b00000100) >> 2;
	*DAC1_PWDWN = (rxData[2] & 0b00000010) >> 1;
	*DAC0_PWDWN = (rxData[2] & 0b00000001);

}




void DACX0504ResetAllRegisters(DACX0504Chip* chip)
{
	int delay = 10;
	DACX0504SendCommand(chip, 0, DACX0504Address_NOP, 0x0000);
	HAL_Delay(delay);
	DACX0504SendCommand(chip, 0, DACX0504Address_SYNC, 0xFF00);
	HAL_Delay(delay);
	DACX0504SendCommand(chip, 0, DACX0504Address_CONFIG, 0x0000);
	HAL_Delay(delay);
	DACX0504SendCommand(chip, 0, DACX0504Address_GAIN, 0x0000);
	HAL_Delay(delay);
	DACX0504SendCommand(chip, 0, DACX0504Address_TRIGGER, 0x0000);
	HAL_Delay(delay);
	DACX0504SendCommand(chip, 0, DACX0504Address_BRDCAST, 0x0000);
	HAL_Delay(delay);
	DACX0504SendCommand(chip, 0, DACX0504Address_STATUS, 0x0000);
	HAL_Delay(delay);
	DACX0504SendCommand(chip, 0, DACX0504Address_DAC0, 0x0000);
	HAL_Delay(delay);
	DACX0504SendCommand(chip, 0, DACX0504Address_DAC1, 0x0000);
	HAL_Delay(delay);
	DACX0504SendCommand(chip, 0, DACX0504Address_DAC2, 0x0000);
	HAL_Delay(delay);
	DACX0504SendCommand(chip, 0, DACX0504Address_DAC3, 0x0000);
	HAL_Delay(delay);
}
