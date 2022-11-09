
#include <SPIHandling.h>
#include "Program.h"
#include "MotorAngleState.h"
#include "MotorControl.h"





extern "C" {

void SPI_CUSTOM_HANDLER(SPI_HandleTypeDef *hspi)
{
	HAL_GPIO_WritePin(Debug_Output1_GPIO_Port, Debug_Output1_Pin, GPIO_PIN_SET);



	uint16_t txdata;
	uint16_t rxData;


	//get the next word to go out from communication handler
	ComHandler.Slave_GetNextOut(&txdata);


	int goodToSend = 1;
	if(hspi->Instance->SR & 0b1)//RXNE, RX Has word To Read.
	{
		rxData = hspi->Instance->DR;
	}
	else
	{
		goodToSend = 0;
		Error_Handler(); //should not happen
	}

	if(hspi->Instance->SR & 0b10)//TXE  Tx is empty and can be set to the next word
	{
		hspi->Instance->DR = txdata;
	}
	else
	{
		goodToSend = 0;
		//Error_Handler(); //should not happen
	}
	if(hspi->Instance->SR & 0b1000000)//OVR
	{
		Error_Handler();
	}



	//feed the incoming word to the communication handler
	if(goodToSend)
		ComHandler.Slave_ProcessNextIn(rxData);



	HAL_GPIO_WritePin(Debug_Output1_GPIO_Port, Debug_Output1_Pin, GPIO_PIN_RESET);
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{



}




}



