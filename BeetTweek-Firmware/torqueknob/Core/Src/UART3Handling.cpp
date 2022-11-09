/*
 * UART3Handling.cpp
 *
 *  Created on: Jul 29, 2020
 *      Author: casht
 */
#include "UART3Handling.h"

#include "MotorController.h"
#include "CircularQueueBuffer.h"

#include "usart.h"
#include "SerialCommands.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ee24.h"
#include "Program.h"

extern MotorController motorController;
extern CircularQueueBuffer<uint8_t, UART3_RX_BuffSize> SerialDevice_RX_Buffer;
extern CircularQueueBuffer<uint8_t, SPI3_RX_BuffSize> SPI3_RX_Buffer;


extern "C" {


	void HandleUART3RXInterupt()
	{

		//read the byte into the buffer
		while(huart3.Instance->SR & (1<<5))//rxne
		{
			SerialDevice_RX_Buffer.AddNewData((uint8_t)(huart3.Instance->DR & 0x000000FF));
		}
	}

}




HAL_StatusTypeDef WaitReadUART3RXByte(uint8_t* data, int size, int timeout)
{
	int tickStart = HAL_GetTick();

	while((HAL_GetTick() < uint32_t(tickStart + timeout)) && SerialDevice_RX_Buffer.HeadLeadAmount() < size)
	{
		//wait
	}

	if(SerialDevice_RX_Buffer.HeadLeadAmount() < size)
		return HAL_TIMEOUT;

	//now there should be size bytes in the rx buffer.  copy into data now and zero out again.
	for( int i = 0; i < size; i++)
	{
		SerialDevice_RX_Buffer.ReadOne(&data[i]);
	}

	return HAL_OK;
}


HAL_StatusTypeDef HandleSerialAPI(UART_HandleTypeDef* huart)
{
	  const int defaultTimeoutMS = 10000;
	  uint8_t byte;

	  while(SerialDevice_RX_Buffer.HeadLeadAmount())
	  {


		  HAL_StatusTypeDef status = WaitReadUART3RXByte(&byte, 1, defaultTimeoutMS);
		  if(status == HAL_TIMEOUT)
			  return HAL_TIMEOUT;




		  uint8_t commandTypeCode = byte;
		  if(commandTypeCode == CommandTypeCode_Action)
		  {
			  //its a command to do something.
			  byte = CommandAction_NONE;
			  //get action code
			  status = WaitReadUART3RXByte(&byte, 1, defaultTimeoutMS);
			  if(status == HAL_TIMEOUT)
				  return HAL_TIMEOUT;

			  uint8_t actionCode = byte;

			  //get parameter length
			  status = WaitReadUART3RXByte(&byte, 1, defaultTimeoutMS);
			  if(status == HAL_TIMEOUT)
			 		return HAL_TIMEOUT;

			  uint8_t parameterLength = byte;

			  if(actionCode == CommandAction_ToggleStatusLed)
			  {
				  //toggle led
				  HAL_GPIO_TogglePin(Board_Status_LED_GPIO_Port, Board_Status_LED_Pin);
			  }
			  else if(actionCode == CommandAction_SetMotorModelParam)
			  {
				  //1 param code byte, and 4 bytes for packed float.
				  uint8_t paramCode;
				  WaitReadUART3RXByte((uint8_t*)&paramCode, 1, defaultTimeoutMS);
				  float packedValue;
				  uint8_t data[4];
				  uint32_t bits;
				  WaitReadUART3RXByte(data, parameterLength-1, defaultTimeoutMS);

				  bits = ((data[0] << 24) & 0xFF000000) | ((data[1] << 16) & 0x00FF0000) | ((data[2] << 8) & 0x0000FF00) | ((data[3] << 0) & 0x000000FF);
				  *((uint32_t*)&packedValue) = bits;

				  if(paramCode == 1)
				  {
					  motorController.antiCoggAmplitude = packedValue;
				  }

			  }
			  //finally send the action code back to indicate the action completed
			  status = HAL_UART_Transmit(huart, (uint8_t*)&actionCode, 1, defaultTimeoutMS);
			  if(status != HAL_OK)
				  Error_Handler();

		  }
		  else if (commandTypeCode == CommandTypeCode_Request)
		  {
			  //its a request
			  byte = CommandRequest_NONE;
			  status = WaitReadUART3RXByte(&byte, 1, defaultTimeoutMS);
			  if(status == HAL_TIMEOUT)
				  return HAL_TIMEOUT;

			  uint8_t requestCode = byte;

			  //read the parameter data length
			  status = WaitReadUART3RXByte(&byte, 1, defaultTimeoutMS);
			  uint8_t parameterDataLength = byte;

			  //read the parameter data if present
			  uint8_t parameterData[256] = {0};
			  status = WaitReadUART3RXByte((uint8_t*)parameterData, parameterDataLength, defaultTimeoutMS);



			  if(requestCode == CommandRequest_BoardModelString)
			  {
				  const char* modelString = "MotorBoard";
				  uint8_t responseSize = strlen(modelString);

				  status = HAL_UART_Transmit(huart, &responseSize, 1, defaultTimeoutMS);
				  if(status == HAL_TIMEOUT)
					  return HAL_TIMEOUT;
				  status = HAL_UART_Transmit(huart, (uint8_t*) modelString, responseSize, defaultTimeoutMS);
				  if(status == HAL_TIMEOUT)
					  return HAL_TIMEOUT;
			  }
			  else if(requestCode == CommandRequest_InitialConnectionCode)
			  {
				  uint8_t responseSize = strlen(INITIAL_CONNECTION_CODE);

				  status = HAL_UART_Transmit(huart, &responseSize, 1, defaultTimeoutMS);
				  if(status == HAL_TIMEOUT)
					  return HAL_TIMEOUT;
				  status = HAL_UART_Transmit(huart, (uint8_t*) INITIAL_CONNECTION_CODE, responseSize, defaultTimeoutMS);
				  if(status == HAL_TIMEOUT)
					  return HAL_TIMEOUT;
			  }

			  else if(requestCode == CommandRequest_ModuleNameString)
			  {
				  const char* moduleNameString = "BeetTweek";
				  uint8_t responseSize = strlen(moduleNameString);

				  status = HAL_UART_Transmit(huart, &responseSize, 1, defaultTimeoutMS);
				  if(status == HAL_TIMEOUT)
					  return HAL_TIMEOUT;
				  status = HAL_UART_Transmit(huart, (uint8_t*) moduleNameString, responseSize, defaultTimeoutMS);
				  if(status == HAL_TIMEOUT)
					  return HAL_TIMEOUT;
			  }
			  else if(requestCode == CommandRequest_ModuleSerialNumber)
			  {
				  uint32_t serialNumber = 99;

				  char serialNum[128] = {'\n'};
				  memset(serialNum, '\n', 128);
				  sprintf(serialNum, "%u", serialNumber);

				  uint8_t responseSize = strlen(serialNum);


				  status = HAL_UART_Transmit(huart, &responseSize, 1, defaultTimeoutMS);
				  if(status == HAL_TIMEOUT)
					  return HAL_TIMEOUT;
				  status = HAL_UART_Transmit(huart, (uint8_t*) serialNum, responseSize, defaultTimeoutMS);
				  if(status == HAL_TIMEOUT)
					  return HAL_TIMEOUT;
			  }
			  else if(requestCode == CommandRequest_FirmwareVersion)
			  {
				  char firmwareNum[128] = {'\n'};
				  memset(firmwareNum, '\n', 128);
				  sprintf(firmwareNum, "%u", 8);

				  uint8_t responseSize = strlen(firmwareNum);

				  status = HAL_UART_Transmit(huart, &responseSize, 1, defaultTimeoutMS);
				  if(status == HAL_TIMEOUT)
					  return HAL_TIMEOUT;
				  status = HAL_UART_Transmit(huart, (uint8_t*) firmwareNum, responseSize, defaultTimeoutMS);
				  if(status == HAL_TIMEOUT)
					  return HAL_TIMEOUT;

			  }
			  else if(requestCode == CommandRequest_MotorModelParam)
			  {
				  if(parameterData[0] == 1)
				  {
					  uint8_t responseSize = sizeof(float);
					  HAL_UART_Transmit(huart, &responseSize, 1, defaultTimeoutMS);
					  HAL_UART_Transmit(huart, (uint8_t*)&motorController.antiCoggAmplitude, sizeof(float), defaultTimeoutMS);
				  }
			  }
			  else if(requestCode == CommandRequest_IsHeadOfModule)
			  {
				  const char* modelString = "NO";
				  uint8_t responseSize = strlen(modelString);

				  status = HAL_UART_Transmit(huart, &responseSize, 1, defaultTimeoutMS);
				  if(status == HAL_TIMEOUT)
					  return HAL_TIMEOUT;
				  status = HAL_UART_Transmit(huart, (uint8_t*) modelString, responseSize, defaultTimeoutMS);
				  if(status == HAL_TIMEOUT)
					  return HAL_TIMEOUT;
			  }
		  }

	  }
	  return HAL_OK;
}
