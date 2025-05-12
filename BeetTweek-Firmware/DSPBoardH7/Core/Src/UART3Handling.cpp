/*
 * UART3Handling.cpp
 *
 *  Created on: Jul 29, 2020
 *      Author: casht
 */


#include <CircularQueueBuffer.h>
#include "main.h"
#include "usart.h"
#include "SerialCommands.h"
#include "stdio.h"
#include "string.h"
#include "LEDPanelManager.h"
#include "Modes/Mode.h"
#include "BootLoaderJump.h"
#include "ee24.h"
#include "Modes/Mode.h"
#include "Program.h"
#include "UART3Handling.h"
#include "usb_device.h"
#include "MotorControl.h"

#if defined(VIRTUALCOMUSB)
#include "usbd_cdc_if.h"
#endif
extern USBD_HandleTypeDef hUsbDeviceFS;
extern ModeManager modeManager;
extern MotorDevice motorDevA;
extern LEDPanelManager LEDManager;
extern CircularQueueBuffer<uint8_t, UART3_RX_BuffSize> SerialDevice_RX_Buffer;
extern float DACCalibrationPoints[4][NUM_DAC_ADC_CAL_POINTS];
extern float ADCCalibrationPoints[4][NUM_DAC_ADC_CAL_POINTS];
extern "C" {

	void HandleUART3RXInterupt()
	{
	    /* Computation of UART mask to apply to RDR register */
	    UART_MASK_COMPUTATION(&huart3);
		uint16_t uhMask = huart3.Mask;


		//read the byte into the buffer
		uint8_t data = (huart3.Instance->RDR & (uint8_t)uhMask);
		if(fullyStartedUp){
			CircularQueueBuffer<uint8_t, UART3_RX_BuffSize>::CircularBufferCode code = SerialDevice_RX_Buffer.AddNewData(data);
			if(code != CircularQueueBuffer<uint8_t, UART3_RX_BuffSize>::CircularBufferCode_OK)
				Error_Handler();
		}


	}


	void OnRecieveUSBCom(uint8_t* Buf, uint16_t Len)
	{
		if(fullyStartedUp)
		{
			for(uint32_t i = 0; i < Len; i++)
			{
				CircularQueueBuffer<uint8_t, UART3_RX_BuffSize>::CircularBufferCode code = SerialDevice_RX_Buffer.AddNewData(Buf[i]);
				if(code != CircularQueueBuffer<uint8_t, UART3_RX_BuffSize>::CircularBufferCode_OK)
					Error_Handler();
			}
		}
	}
}




HAL_StatusTypeDef WaitReadSerialDeviceRXByte(SerialDevice* device, uint8_t* data, int size, uint32_t timeout)
{
	uint32_t tickStart = HAL_GetTick();
	uint32_t endTime = tickStart + timeout;
	if(endTime < tickStart)
	{
		//wrap around detected

		//wait till wrap event
		while(HAL_GetTick() <= UINT32_MAX && HAL_GetTick() > UINT32_MAX/2){}

		//compute new end time
		int diff1 = UINT32_MAX - tickStart;
		int diff2 = endTime - diff1;

		endTime = diff2;
	}
	while((HAL_GetTick() < endTime) && SerialDevice_RX_Buffer.HeadLeadAmount() < size)
	{
		//wait remaining time.
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
extern PCD_HandleTypeDef hpcd_USB_OTG_FS;

extern "C" {
HAL_StatusTypeDef HandleTxNextBits()
{
	extern CircularQueueBuffer<uint8_t, UART3_TX_BuffSize> SerialDevice_TX_Buffer;
	auto len = SerialDevice_TX_Buffer.HeadLeadAmount();
	if(len > 64)
		 len = 64;
	auto status = CDC_Transmit_FS(&SerialDevice_TX_Buffer.buffer[SerialDevice_TX_Buffer.readIndex], len);
	if(status == USBD_OK)
	{
		uint8_t dummy;
		for(int i = 0; i < len; i++)
			SerialDevice_TX_Buffer.ReadOne(&dummy);
	}
	else
	{
		//wait until HandleTxNextBits() is called from next interupt callback...
	}


	return HAL_OK;
}

}


HAL_StatusTypeDef SerialDeviceTransmit(SerialDevice* device, const uint8_t* pData, uint16_t Size, uint32_t Timeout)
{
	if(Size == 0)
		Error_Handler();

	if(device->huart != nullptr)
		return HAL_UART_Transmit(device->huart, pData, Size, Timeout);
	else if(device->usb == true)
	{

#if defined(VIRTUALCOMUSB)
		uint8_t ret;
		uint8_t* dataPointer = (uint8_t*)pData;
		//PCD_HandleTypeDef *hpcd = (PCD_HandleTypeDef *)hUsbDeviceFS.pData;

		//if timed out once - return immediatly because connection must be not be working.
		if(device->timedOut)
			return HAL_ERROR;

		//wait until class data available.
		int timerCounter = 0;
		while(hUsbDeviceFS.pClassData == nullptr ){
			HAL_Delay(1);
			timerCounter++;
			if(timerCounter > Timeout)
			{
				device->connected = false;
				device->timedOut = true;
				return HAL_ERROR;
			}
		}

		//should be connected at this point.
		if(device->connected == false)
			device->connected = true;


		USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef*)hUsbDeviceFS.pClassData;
		extern CircularQueueBuffer<uint8_t, UART3_TX_BuffSize> SerialDevice_TX_Buffer;

		for(int i = 0; i < Size; i++)
		{
			auto ret = SerialDevice_TX_Buffer.AddNewData(pData[i]);
			if(ret == CircularQueueBuffer<uint8_t, UART3_TX_BuffSize>::CircularBufferCode_HeadCrossRead)
			{
				strcpy(errorCode, "SerialDeviceTransmit: TX RAM buffer overflow.");
				//Error_Handler();
			}

		}
		auto status = HandleTxNextBits();
#endif


		int i = 0;
		//
		return HAL_OK;


	}
	else{
		return HAL_ERROR;
	}

	return HAL_ERROR;
}



void HAL_ErrorCheck(HAL_StatusTypeDef status)
{
	if(status != HAL_OK)
		Error_Handler();
}
#define EXIT_UART(status) if(status != HAL_OK) {return status; }




HAL_StatusTypeDef HandleSerialAPI(SerialDevice* serialDevice)
{
	  const int defaultTimeoutMS = 20000;
	  uint8_t byte;

//	  while(SerialDevice_RX_Buffer.HeadLeadAmount())
//	  {
//		  SerialDevice_RX_Buffer.ReadOne(&byte);
//		  SerialDeviceTransmit(serialDevice, &byte, 1, defaultTimeoutMS);
//	  }
//	  return HAL_OK;




	  while(SerialDevice_RX_Buffer.HeadLeadAmount())
	  {

		  HAL_StatusTypeDef status = WaitReadSerialDeviceRXByte(serialDevice, &byte, 1, defaultTimeoutMS);
		  EXIT_UART(status)


		  uint8_t commandTypeCode = byte;
		  if(commandTypeCode == CommandTypeCode_Action)
		  {
			  //its a command to do something.
			  byte = CommandAction_NONE;
			  status = WaitReadSerialDeviceRXByte(serialDevice, &byte, 1, defaultTimeoutMS);
			  EXIT_UART(status)

			  uint8_t actionCode = byte;

			  status = WaitReadSerialDeviceRXByte(serialDevice, &byte, 1, defaultTimeoutMS);
			  EXIT_UART(status)
			  uint8_t parameterLength = byte;


			  char parameterData[255] = {'\0'};
			  WaitReadSerialDeviceRXByte(serialDevice, (uint8_t*)parameterData, parameterLength, defaultTimeoutMS);


			  if(actionCode == CommandAction_ToggleStatusLed)
			  {
				  //toggle led
				  HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

				  HAL_GPIO_TogglePin(MULTI_BOARD_BOOT_GPIO_Port, MULTI_BOARD_BOOT_Pin);

				  //flash some leds..
				  for(int i = 0; i < LED_NUM_LEDS_PER_RING; i++)
				  {
					  LEDManager.SetLEDOuterRing_Int(i, 32, 32, 32);
					  LEDManager.SetLEDInnerRing_Int(i, 32, 32, 32);
				  }
				  WS2812_SWAP_BUFFERS();


			  }
			  else if(actionCode == CommandAction_SetFrictionFactor)
			  {
				  char* endPtr;
				  float number = strtof(parameterData, &endPtr);
				  number = MathExtras::ClampInclusive(number, FRICTIONFACTOR_MIN, FRICTIONFACTOR_MAX);
				  Mode::frictionCalFactor = number;
				  ee24_write_float(EE_FrictionFactor_32bits, Mode::frictionCalFactor , 1000);
			  }
			  else if(actionCode == CommandAction_SetDrivePowerFactor)
			  {
				  char* endPtr;
				  float number = strtof(parameterData, &endPtr);
				  number = MathExtras::ClampInclusive(number, DRIVEPOWERFACTOR_MIN, DRIVEPOWERFACTOR_MAX);
				  Mode::drivePowerFactor = number;
				  ee24_write_float(EE_DrivePowerFactor_32bits, Mode::drivePowerFactor , 1000);
			  }
			  else if(actionCode == CommandAction_SetDriveOffset)
			  {
				  char* endPtr;
				  float number = strtof(parameterData, &endPtr);
				  number = MathExtras::ClampInclusive(number, DRIVEOFFSET_MIN, DRIVEOFFSET_MAX);
				  Mode::driveOffset = number;
				  ee24_write_float(EE_DriveOffset_32bits, Mode::driveOffset , 1000);
			  }
			  else if(actionCode == CommandAction_SetMotorDirection)
			  {

				  int number = atoi(parameterData);
				  number = MathExtras::ClampInclusive(number, -1, 1);
#ifdef COMBINEDBOARD
				  motorDevA.direction = number;
#endif
				  ee24_write_float(EE_CombinedBoardMotorDirection_32bits, number , 1000);
			  }
			  else if(actionCode == CommandAction_ToggleCalibrationMode)
			  {
				  modeManager.calibrationMode = true;
			  }
			  else if((actionCode == CommandAction_SetDAC0 ) || (actionCode == CommandAction_SetDAC1) || (actionCode == CommandAction_SetDAC2) || (actionCode == CommandAction_SetDAC3))
			  {
				  char* endPtr;
				  float number = strtof(parameterData, &endPtr);
				  //uint16_t dacIndex = actionCode - uint16_t(CommandAction_SetDAC0);

				  if((actionCode == CommandAction_SetDAC0 ))
					  dacOuts[0] = number;
				  else if(actionCode == CommandAction_SetDAC1 )
					  dacOuts[1] = number;
				  else if(actionCode == CommandAction_SetDAC2 )
				  	  dacOuts[2] = number;
				  else if(actionCode == CommandAction_SetDAC3 )
				  	  dacOuts[3] = number;
			  }
			  else if((actionCode == CommandAction_SetDACCAL0) || (actionCode == CommandAction_SetDACCAL1) || (actionCode == CommandAction_SetDACCAL2) || (actionCode == CommandAction_SetDACCAL3))
			  {
				    //parameter data is numbers separated by ","
					char * pch;
					pch = strtok (parameterData,",");
					int calibrationPointIndex = 0;
					bool writeSuccess = true;
					while (pch != NULL)
					{
					  //extract number
					  char* endPtr;
					  float number = strtof(pch, &endPtr);

					  int dacIdx = 0;
					  //store in eeprom and apply to the calibration map in ram
					  if((actionCode == CommandAction_SetDACCAL0))
					  {
						  dacIdx = 0;
						  writeSuccess &= ee24_write_float(EE_DAC0_CAL_32bits + calibrationPointIndex*4, number, 1000);
					  }
					  else if((actionCode == CommandAction_SetDACCAL1))
					  {
						  dacIdx = 1;
						  writeSuccess &= ee24_write_float(EE_DAC1_CAL_32bits + calibrationPointIndex*4, number, 1000);
					  }
					  else if((actionCode == CommandAction_SetDACCAL2))
					  {
						  dacIdx = 2;
						  writeSuccess &= ee24_write_float(EE_DAC2_CAL_32bits + calibrationPointIndex*4, number, 1000);
					  }
					  else if((actionCode == CommandAction_SetDACCAL3))
					  {
						  dacIdx = 3;
						  writeSuccess &= ee24_write_float(EE_DAC3_CAL_32bits + calibrationPointIndex*4, number, 1000);
					  }


					  if(writeSuccess == false)
					  {
						  strcpy(errorCode, "UART Write Error");
						  Error_Handler();
					  }

					  DACCalibrationPoints[dacIdx][calibrationPointIndex] = number;


					  calibrationPointIndex++;
					  pch = strtok (NULL, ",");
					}
					FixCalibrations();
			  }
			  else if((actionCode == CommandAction_SetADCCAL0) || (actionCode == CommandAction_SetADCCAL1)
					  || (actionCode == CommandAction_SetADCCAL2) || (actionCode == CommandAction_SetADCCAL3))
			  {
				    //parameter data is numbers seperated by ","
					char * pch;
					pch = strtok (parameterData,",");
					int calibrationPointIndex = 0;
					bool writeSuccess = true;
					while (pch != NULL)
					{
					  //extract number
					  char* endPtr;
					  float number = strtof(pch, &endPtr);

					  int adcIdx = 0;
					  //store in eeprom and apply to the calibration map in ram
					  if((actionCode == CommandAction_SetADCCAL0))
					  {
						  adcIdx = 0;
						  writeSuccess &= ee24_write_float(EE_ADC0_CAL_32bits + calibrationPointIndex*4, number, 1000);
					  }
					  else if((actionCode == CommandAction_SetADCCAL1))
					  {
						  adcIdx = 1;
						  writeSuccess &= ee24_write_float(EE_ADC1_CAL_32bits + calibrationPointIndex*4, number, 1000);
					  }
					  else if((actionCode == CommandAction_SetADCCAL2))
					  {
						  adcIdx = 2;
						  writeSuccess &= ee24_write_float(EE_ADC2_CAL_32bits + calibrationPointIndex*4, number, 1000);
					  }
					  else if((actionCode == CommandAction_SetADCCAL3))
					  {
						  adcIdx = 3;
						  writeSuccess &= ee24_write_float(EE_ADC3_CAL_32bits + calibrationPointIndex*4, number, 1000);
					  }


					  ADCCalibrationPoints[adcIdx][calibrationPointIndex] = number;


					  calibrationPointIndex++;
					  pch = strtok (NULL, ",");
					}

					FixCalibrations();
			  }
			  else if(actionCode == CommandAction_SaveUserCalibrationsToFactory)
			  {
				  FixCalibrations();
				  bool writeSuccess = true;
				  for(int chan = 0; chan < 4; chan++)
				  {
					  for(int i = 0; i < NUM_DAC_ADC_CAL_POINTS; i++)
					  {
						  writeSuccess &= ee24_write_float(EE_ADC0_FACTORY_CAL_32bits + chan*NUM_DAC_ADC_CAL_POINTS*4 + i*4, ADCCalibrationPoints[chan][i], 1000);
						  writeSuccess &= ee24_write_float(EE_DAC0_FACTORY_CAL_32bits + chan*NUM_DAC_ADC_CAL_POINTS*4 + i*4, DACCalibrationPoints[chan][i], 1000);
					  }
				  }
				  if(!writeSuccess)
				  {
					  strcpy(errorCode, "UART Write Error");
					  Error_Handler();
				  }
			  }
			  else if(actionCode == CommandAction_RestoreFactoryCalibrationsToUser)
			  {
				  bool writeSuccess = true;
				  for(int chan = 0; chan < 4; chan++)
				  {
					  for(int i = 0; i < NUM_DAC_ADC_CAL_POINTS; i++)
					  {
						  float adc,dac;
						  writeSuccess &= ee24_read_float(EE_ADC0_FACTORY_CAL_32bits + chan*NUM_DAC_ADC_CAL_POINTS*4 + i*4, &adc, 1000, 0.0f);
						  writeSuccess &= ee24_read_float(EE_DAC0_FACTORY_CAL_32bits + chan*NUM_DAC_ADC_CAL_POINTS*4 + i*4, &dac, 1000, 0.0f);

						  ADCCalibrationPoints[chan][i] = adc;
						  DACCalibrationPoints[chan][i] = dac;
					  }
				  }
				  if(!writeSuccess)
				  {
					  strcpy(errorCode, "UART Write Error");
					  Error_Handler();
				  }
			  }
			  else if(actionCode == CommandAction_RestoreUserCalibrations)
			  {
				  bool writeSuccess = true;
				  for(int chan = 0; chan < 4; chan++)
				  {
					  for(int i = 0; i < NUM_DAC_ADC_CAL_POINTS; i++)
					  {
						  float adc,dac;
						  writeSuccess &= ee24_read_float(EE_ADC0_CAL_32bits + chan*NUM_DAC_ADC_CAL_POINTS*4 + i*4, &adc, 1000, 0.0f);
						  writeSuccess &= ee24_read_float(EE_DAC0_CAL_32bits + chan*NUM_DAC_ADC_CAL_POINTS*4 + i*4, &dac, 1000, 0.0f);

						  ADCCalibrationPoints[chan][i] = adc;
						  DACCalibrationPoints[chan][i] = dac;
					  }
				  }
				  if(!writeSuccess)
				  {
					  strcpy(errorCode, "UART Write Error");
					  Error_Handler();
				  }

			  }
			  else if(actionCode == CommandAction_SetModuleSerialNumber)
			  {
				  char* endPtr;
				  uint32_t number = strtoul(parameterData, &endPtr, 10);

				  bool writeSuccess = ee24_write_32(EE_SERIALNUMBER_32Bit, number , 1000);
				  if(!writeSuccess)
				  {
					  strcpy(errorCode, "UART Write Error");
					  Error_Handler();
				  }
				//  uint16_t writeStatus = EE_WriteVariable(VirtAddVarTab[EE_SERIALNUMBER_Bits_0], number & 0x0000FFFF);
				  //writeStatus = EE_WriteVariable(VirtAddVarTab[EE_SERIALNUMBER_Bits_1], number >> 16);
			  }
			  else if(actionCode == CommandAction_GoToBootMode)
			  {
				  //set an led on the outer ring.  index 0 is top, going clockwise to index n-1 left of top

				  //if the board actually goes into boot mode - this is the last thing drawn.
				  for(int i = 0; i < LED_NUM_LEDS_PER_RING; i++)
				  {
					  LEDManager.SetLEDOuterRing_Int(i, 32, 32, 0);
					  LEDManager.SetLEDInnerRing_Int(i, 32, 32, 0);
				  }
				  for(int i = 0; i < 8; i++){
					  LEDManager.SetLEDButton(i, 32, 32, 0);
				  }
				  WS2812_SWAP_BUFFERS();

				  //delay to let leds write out..
				  HAL_Delay(100);



				  //configure motor reset pin for output

				  //set output level
				  HAL_GPIO_WritePin(MOTOR_T_NRST_GPIO_Port, MOTOR_T_NRST_Pin, GPIO_PIN_SET);

				  //and init
				  GPIO_InitTypeDef GPIO_InitStruct = {0};
				  GPIO_InitStruct.Pin = MOTOR_T_NRST_Pin;
				  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
				  GPIO_InitStruct.Pull = GPIO_NOPULL;
				  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
				  HAL_GPIO_Init(MOTOR_T_NRST_GPIO_Port, &GPIO_InitStruct);


//				  //configure dsp (self) reset pin for output

				  //set output level
				  HAL_GPIO_WritePin(SELF_T_NRST_GPIO_Port, SELF_T_NRST_Pin, GPIO_PIN_SET);

				  //and init
				  GPIO_InitTypeDef GPIO_SelfInitStruct = {0};
				  GPIO_SelfInitStruct.Pin = SELF_T_NRST_Pin;
				  GPIO_SelfInitStruct.Mode = GPIO_MODE_OUTPUT_PP;
				  GPIO_SelfInitStruct.Pull = GPIO_PULLUP;
				  GPIO_SelfInitStruct.Speed = GPIO_SPEED_FREQ_LOW;
				  HAL_GPIO_Init(SELF_T_NRST_GPIO_Port, &GPIO_SelfInitStruct);


				      HAL_GPIO_WritePin(MULTI_BOARD_BOOT_GPIO_Port, MULTI_BOARD_BOOT_Pin, GPIO_PIN_SET);
				      HAL_Delay(1);

				      HAL_GPIO_WritePin(MOTOR_T_NRST_GPIO_Port, MOTOR_T_NRST_Pin, GPIO_PIN_RESET);
					  HAL_GPIO_WritePin(SELF_T_NRST_GPIO_Port, SELF_T_NRST_Pin, GPIO_PIN_RESET);


				  //should never get here, indicates an unsuccessful boot mode.
				  int c = 0;
				  while(1){
					  for(int i = 0; i < LED_NUM_LEDS_PER_RING; i++)
					  {
						  LEDManager.SetLEDOuterRing_Int(i, c, 0, 0);
						  LEDManager.SetLEDInnerRing_Int(i, c, 0, 0);
					  }
					  WS2812_SWAP_BUFFERS();
					  c++;
					  if(c > 32)
						  c = 0;
				  }



			  }
			  else if(actionCode == CommandAction_ResetSlaveController)
			  {
				  //reset the motor board TODO
				  ResetBoards();
			  }



			  //finally send the action code back to indicate the action completed
			  status = SerialDeviceTransmit(serialDevice, (uint8_t*)&actionCode, 1, defaultTimeoutMS);
			  if(status != HAL_OK)
			  {
				  strcpy(errorCode, "UART Write Error");
				  Error_Handler();
			  }


		  }
		  else if (commandTypeCode == CommandTypeCode_Request)
		  {
			  //its a request

			  //read the request code
			  byte = CommandRequest_NONE;
			  status = WaitReadSerialDeviceRXByte(serialDevice, &byte, 1, defaultTimeoutMS);
			  EXIT_UART(status)

			  uint8_t requestCode = byte;

			  //read the parameter data length
			  status = WaitReadSerialDeviceRXByte(serialDevice, &byte, 1, defaultTimeoutMS);
			  uint8_t parameterDataLength = byte;

			  //read the parameter data if present
			  uint8_t parameterData[256] = {0};
			  status = WaitReadSerialDeviceRXByte(serialDevice, (uint8_t*)parameterData, parameterDataLength, defaultTimeoutMS);


			  if(requestCode == CommandRequest_BoardModelString)
			  {
				  const char* modelString = "DSPBoard";
				  uint8_t responseSize = strlen(modelString);

				  status = SerialDeviceTransmit(serialDevice, &responseSize, 1, defaultTimeoutMS);
				  EXIT_UART(status)

				  status = SerialDeviceTransmit(serialDevice, (uint8_t*) modelString, responseSize, defaultTimeoutMS);
				  EXIT_UART(status)
			  }
			  else if(requestCode == CommandRequest_ModuleNameString)
			  {
				  const char* modelString = "BeetTweek";
				  uint8_t responseSize = strlen(modelString);

				  status = SerialDeviceTransmit(serialDevice, &responseSize, 1, defaultTimeoutMS);
				  EXIT_UART(status)
				  status = SerialDeviceTransmit(serialDevice, (uint8_t*) modelString, responseSize, defaultTimeoutMS);
				  EXIT_UART(status)
			  }


			  else if(requestCode == CommandRequest_InitialConnectionCode)
			  {
				  uint8_t responseSize = strlen(INITIAL_CONNECTION_CODE);

				  status = SerialDeviceTransmit(serialDevice, &responseSize, 1, defaultTimeoutMS);
				  EXIT_UART(status)

				  status = SerialDeviceTransmit(serialDevice, (uint8_t*) INITIAL_CONNECTION_CODE, responseSize, defaultTimeoutMS);
				  EXIT_UART(status)
			  }
			  else if(requestCode == CommandRequest_GetFrictionFactor)
			  {

				  char valueStr[128] = {'\n'};
				  memset(valueStr, '\n', 128);
				  sprintf(valueStr, "%f", Mode::frictionCalFactor);

				  uint8_t responseSize = strlen(valueStr);

				  status = SerialDeviceTransmit(serialDevice, &responseSize, 1, defaultTimeoutMS);
				  EXIT_UART(status)

				  status = SerialDeviceTransmit(serialDevice, (uint8_t*) valueStr, responseSize, defaultTimeoutMS);
				  EXIT_UART(status)
			  }
			  else if(requestCode == CommandRequest_GetDrivePowerFactor)
			  {
				  char valueStr[128] = {'\n'};
				  memset(valueStr, '\n', 128);
				  sprintf(valueStr, "%f", Mode::drivePowerFactor);

				  uint8_t responseSize = strlen(valueStr);

				  status = SerialDeviceTransmit(serialDevice, &responseSize, 1, defaultTimeoutMS);
				  EXIT_UART(status)

				  status = SerialDeviceTransmit(serialDevice, (uint8_t*) valueStr, responseSize, defaultTimeoutMS);
				  EXIT_UART(status)
			  }
			  else if(requestCode == CommandRequest_GetDriveOffset)
			  {
				  char valueStr[128] = {'\n'};
				  memset(valueStr, '\n', 128);
				  sprintf(valueStr, "%f", Mode::driveOffset);

				  uint8_t responseSize = strlen(valueStr);

				  status = SerialDeviceTransmit(serialDevice, &responseSize, 1, defaultTimeoutMS);
				  EXIT_UART(status)

				  status = SerialDeviceTransmit(serialDevice, (uint8_t*) valueStr, responseSize, defaultTimeoutMS);
				  EXIT_UART(status)
			  }
			  else if(requestCode == CommandRequest_GetMotorDirection)
			  {
				  char valueStr[128] = {'\n'};
				  memset(valueStr, '\n', 128);

				  int direction = 1;
#ifdef COMBINEDBOARD
				  direction = motorDevA.direction;
#endif
				  sprintf(valueStr, "%d", direction);

				  uint8_t responseSize = strlen(valueStr);

				  status = SerialDeviceTransmit(serialDevice, &responseSize, 1, defaultTimeoutMS);
				  EXIT_UART(status)

				  status = SerialDeviceTransmit(serialDevice, (uint8_t*) valueStr, responseSize, defaultTimeoutMS);
				  EXIT_UART(status)
			  }
			  else if(requestCode == CommandRequest_GetADC0 || requestCode == CommandRequest_GetADC1
					  || requestCode == CommandRequest_GetADC2 || requestCode == CommandRequest_GetADC3)
			  {
				  int adcIdx = 0;
				  if(requestCode      == CommandRequest_GetADC0)
					  adcIdx = 0;
				  else if(requestCode == CommandRequest_GetADC1)
				  	  adcIdx = 1;
				  else if(requestCode == CommandRequest_GetADC2)
				  	  adcIdx = 2;
				  else if(requestCode == CommandRequest_GetADC3)
				  	  adcIdx = 3;


				  char valueStr[128] = {'\n'};
				  memset(valueStr, '\n', 128);
				  sprintf(valueStr, "%f", adcIns[adcIdx]);

				  uint8_t responseSize = strlen(valueStr);

				  status = SerialDeviceTransmit(serialDevice, &responseSize, 1, defaultTimeoutMS);
				  EXIT_UART(status)

				  status = SerialDeviceTransmit(serialDevice, (uint8_t*) valueStr, responseSize, defaultTimeoutMS);
				  EXIT_UART(status)
			  }
			  else if(requestCode == CommandRequest_GetADCVOLTS0 || requestCode == CommandRequest_GetADCVOLTS1 || requestCode == CommandRequest_GetADCVOLTS2 || requestCode == CommandRequest_GetADCVOLTS3)
			  {
				  int adcIdx = 0;
				  if(requestCode      == CommandRequest_GetADCVOLTS0)
					  adcIdx = 0;
				  else if(requestCode == CommandRequest_GetADCVOLTS1)
					  adcIdx = 1;
				  else if(requestCode == CommandRequest_GetADCVOLTS2)
					  adcIdx = 2;
				  else if(requestCode == CommandRequest_GetADCVOLTS3)
					  adcIdx = 3;


				  char valueStr[128] = {'\n'};
				  memset(valueStr, '\n', 128);
				  sprintf(valueStr, "%f", adcVolts[adcIdx]);

				  uint8_t responseSize = strlen(valueStr);

				  status = SerialDeviceTransmit(serialDevice, &responseSize, 1, defaultTimeoutMS);
				  EXIT_UART(status)

				  status = SerialDeviceTransmit(serialDevice, (uint8_t*) valueStr, responseSize, defaultTimeoutMS);
				  EXIT_UART(status)
			  }
			  else if(requestCode == CommandRequest_GetDAC0 || requestCode == CommandRequest_GetDAC1 || requestCode == CommandRequest_GetDAC2 || requestCode == CommandRequest_GetDAC3)
			  {
				  int dacIdx = 0;
				  if(requestCode == CommandRequest_GetDAC0)
					  dacIdx = 0;
				  else if(requestCode == CommandRequest_GetDAC1)
					  dacIdx = 1;
				  else if(requestCode == CommandRequest_GetDAC2)
					  dacIdx = 2;
				  else if(requestCode == CommandRequest_GetDAC3)
					  dacIdx = 3;


				  char valueStr[128];
				  memset(valueStr, '\n', 128);
				  sprintf(valueStr, "%f", dacOuts[dacIdx]);

				  uint8_t responseSize = strlen(valueStr);

				  status = SerialDeviceTransmit(serialDevice, &responseSize, 1, defaultTimeoutMS);
				  EXIT_UART(status)

				  status = SerialDeviceTransmit(serialDevice, (uint8_t*) valueStr, responseSize, defaultTimeoutMS);
				  EXIT_UART(status)
			  }
			  else if(requestCode == CommandRequest_GetDACCAL0 || requestCode == CommandRequest_GetDACCAL1 || requestCode == CommandRequest_GetDACCAL2 || requestCode == CommandRequest_GetDACCAL3)
			  {
				  int dacIdx = 0;
				  if(requestCode == CommandRequest_GetDACCAL0)
					  dacIdx = 0;
				  else if(requestCode == CommandRequest_GetDACCAL1)
					  dacIdx = 1;
				  else if(requestCode == CommandRequest_GetDACCAL2)
					  dacIdx = 2;
				  else if(requestCode == CommandRequest_GetDACCAL3)
					  dacIdx = 3;


				  char valueStr[255];
				  memset(valueStr, '\n', 255);
				  char* strPtr = valueStr;
				  for(int i = 0; i < NUM_DAC_ADC_CAL_POINTS; i++)
				  {
					  strPtr += sprintf(strPtr, "%f", (DACCalibrationPoints[dacIdx][i]));

					  if(i != NUM_DAC_ADC_CAL_POINTS -1)
						  strPtr += sprintf(strPtr, ",");
				  }

				  uint8_t responseSize = strlen(valueStr);

				  status = SerialDeviceTransmit(serialDevice, &responseSize, 1, defaultTimeoutMS);
				  EXIT_UART(status)

				  status = SerialDeviceTransmit(serialDevice, (uint8_t*) valueStr, responseSize, defaultTimeoutMS);
				  EXIT_UART(status)

			  }
			  else if(requestCode == CommandRequest_GetADCCAL0 || requestCode == CommandRequest_GetADCCAL1 || requestCode == CommandRequest_GetADCCAL2 || requestCode == CommandRequest_GetADCCAL3)
			  {
				  int adcIdx = 0;
				  if(requestCode == CommandRequest_GetADCCAL0)
					  adcIdx = 0;
				  else if(requestCode == CommandRequest_GetADCCAL1)
					  adcIdx = 1;
				  else if(requestCode == CommandRequest_GetADCCAL2)
					  adcIdx = 2;
				  else if(requestCode == CommandRequest_GetADCCAL3)
					  adcIdx = 3;


				  char valueStr[255];
				  memset(valueStr, '\n', 255);
				  char* strPtr = valueStr;
				  for(int i = 0; i < NUM_DAC_ADC_CAL_POINTS; i++)
				  {
					  strPtr += sprintf(strPtr, "%f", (ADCCalibrationPoints[adcIdx][i]));

					  if(i != NUM_DAC_ADC_CAL_POINTS -1)
						  strPtr += sprintf(strPtr, ",");

				  }

				  uint8_t responseSize = strlen(valueStr);

				  status = SerialDeviceTransmit(serialDevice, &responseSize, 1, defaultTimeoutMS);
				  EXIT_UART(status)

				  status = SerialDeviceTransmit(serialDevice, (uint8_t*) valueStr, responseSize, defaultTimeoutMS);
				  EXIT_UART(status)

			  }
			  else if(requestCode == CommandRequest_ModuleSerialNumber)
			  {

				  uint32_t serialNumber = 123456;
				  ee24_read_32(EE_SERIALNUMBER_32Bit,  &serialNumber, 1000);

				  char serialNum[128];
				  memset(serialNum, '\n', 128);

				  sprintf(serialNum, "%lu", serialNumber);

				  uint8_t responseSize = strlen(serialNum);

				  status = SerialDeviceTransmit(serialDevice, &responseSize, 1, defaultTimeoutMS);
				  EXIT_UART(status)

				  status = SerialDeviceTransmit(serialDevice, (uint8_t*) serialNum, responseSize, defaultTimeoutMS);
				  EXIT_UART(status)
			  }
			  else if(requestCode == CommandRequest_FirmwareVersion)
			  {
				  char firmwareNum[128] = {'\n'};
				  memset(firmwareNum, '\n', 128);
				  sprintf(firmwareNum, "%u", (unsigned int)firmwareVersion);

				  uint8_t responseSize = strlen(firmwareNum);

				  status = SerialDeviceTransmit(serialDevice, &responseSize, 1, defaultTimeoutMS);
				  EXIT_UART(status)

				  status = SerialDeviceTransmit(serialDevice, (uint8_t*) firmwareNum, responseSize, defaultTimeoutMS);
				  EXIT_UART(status)

			  }
			  else if(requestCode == CommandRequest_IsHeadOfModule)
			  {
				  const char* responseString = "YES";
				  uint8_t responseSize = strlen(responseString);

				  status = SerialDeviceTransmit(serialDevice, &responseSize, 1, defaultTimeoutMS);
				  EXIT_UART(status)

				  status = SerialDeviceTransmit(serialDevice, (uint8_t*) responseString, responseSize, defaultTimeoutMS);
				  EXIT_UART(status)
			  }
		  }

	  }

	  return HAL_OK;
}
