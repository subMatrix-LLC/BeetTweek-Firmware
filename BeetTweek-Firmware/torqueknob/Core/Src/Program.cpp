/*
 * Program.cpp
 *
 *  Created on: Jul 29, 2020
 *      Author: casht
 */
#include "main.h"
#include "stm32f4xx_hal_rcc.h"


#include "CircularQueueBuffer.h"
#include "MathExtras.h"

#include "Program.h"
#include "math.h"
#include "MotorControl.h"
#include "MotorAngleState.h"
#include "MotorController.h"
#include "AS5048.h"
#include "RollingFloatBuffer.h"
#include "ProbeTools.h"
#include "PIDControl.h"
#include "us_delay.h"
#include "SerialCommands.h"
#include "UART3Handling.h"
#include "DualBoardComHandler.h"
#include "CapSense.h"
#include "ee24.h"
#include <iostream>




CircularQueueBuffer<uint8_t, UART3_RX_BuffSize> SerialDevice_RX_Buffer;
CircularQueueBuffer<uint8_t, SPI3_RX_BuffSize> SPI3_RX_Buffer;

AS5048_Device motorEncoderDev;
MotorDevice motorDevA;

CapSenseDevice knobCapSense;

MotorController motorController;
float MOTORUPDATE_SAMPLINGTIME;

DualBoardComHandler ComHandler;

uint16_t dummy;


#include "tim.h"
#include "usart.h"
#include "spi.h"
#include "MotorAngleState.h"

extern "C" {


void UserCodeInit1()
{


}

void UserCodeInit2()
{
	printf("UserCodeInit2..r\n");

	InitUSDelay();

	//Start SPI Recieving right off the bat.
	HAL_GPIO_WritePin(Debug_Output1_GPIO_Port, Debug_Output1_Pin, GPIO_PIN_SET);
	HAL_SPI_Receive_IT(&hspi3, (uint8_t*)&dummy, 1);
	DelayUS(10);
	HAL_GPIO_WritePin(Debug_Output1_GPIO_Port, Debug_Output1_Pin, GPIO_PIN_RESET);



	MotorDeviceInit(&motorDevA, DRV8313_EN1_GPIO_Port, DRV8313_EN1_Pin,
		  DRV8313_EN2_GPIO_Port,   DRV8313_EN2_Pin,
		  DRV8313_EN3_GPIO_Port,   DRV8313_EN3_Pin,
		  DRV8313_A_IN1_GPIO_Port, DRV8313_A_IN1_Pin,
		  DRV8313_A_IN2_GPIO_Port, DRV8313_A_IN2_Pin,
		  DRV8313_A_IN3_GPIO_Port, DRV8313_A_IN3_Pin,
		  &htim3,
		  DRV8313_A_nReset_GPIO_Port, DRV8313_A_nReset_Pin,
		  DRV8313_A_nSleep_GPIO_Port, DRV8313_A_nSleep_Pin,
		  100,
		  DRV8313_A_nFault_GPIO_Port, DRV8313_A_nFault_Pin,
		  1,
		  22, 0.2f);



	MotorAngleStateInit(&motorDevA.angleState);


	AS5048DeviceInit(&motorEncoderDev, &hspi2, AS5048_CS_GPIO_Port, AS5048_CS_Pin);

	InitializeCapSense(&knobCapSense);

	ResetMotorChip(&motorDevA);




	//start motor updating
    if (HAL_TIM_Base_Start_IT(&htim2) != HAL_OK)
    {
    	Error_Handler();
    }




	//wait a bit beofre enabling motor to ignore any bad values spinning the motor on startup
	HAL_Delay(50);

	SetMotorEnable(&motorDevA, 1, GPIO_PIN_SET);
	SetMotorEnable(&motorDevA, 2, GPIO_PIN_SET);
	SetMotorEnable(&motorDevA, 3, GPIO_PIN_SET);



	motorController.ZeroPhaseCalibration();
//	    motorController.AntiCoggingCalibration();
//
//	    bool s = true;
//
//	    s &= ee24_write_float(EE_ANGLECALIBRATION_32bit, motorController.motorAngleZeroPhase, 10000);
//
//	    s &= ee24_write_32(EE_SERIALNUMBER_32Bit, 0, 10000);
//	    s &= ee24_write_16(EE_EEPROMCheckCode_16bit, eepromCheckCode, 10000);
//
//	    s &= ee24_write(EE_CW_ANTICOGG_DATA, (uint8_t*)motorController.coggDriveLookupCW, ANTCOGGPOINTS*sizeof(float), 10000);
//	    if(!s)
//	    	Error_Handler();
//	    s &= ee24_write(EE_CCW_ANTICOGG_DATA, (uint8_t*)motorController.coggDriveLookupCCW, ANTCOGGPOINTS*sizeof(float), 10000);
//	    if(!s)
//	    	Error_Handler();
//	}
//	else
//	{
//		ee24_read_float(EE_ANGLECALIBRATION_32bit,  &motorController.motorAngleZeroPhase, 10000);
//
//		ee24_read(EE_CW_ANTICOGG_DATA, (uint8_t*)motorController.coggDriveLookupCW, ANTCOGGPOINTS*sizeof(float), 10000);
//		ee24_read(EE_CCW_ANTICOGG_DATA, (uint8_t*)motorController.coggDriveLookupCCW, ANTCOGGPOINTS*sizeof(float), 10000);
//	}

	//calculate anticog means
//	motorController.coggDriveCWMean = 0.0f;
//	for(int i = 0; i < ANTCOGGPOINTS; i++)
//	{
//		motorController.coggDriveCWMean += motorController.coggDriveLookupCW[i];
//		motorController.coggDriveCCWMean += motorController.coggDriveLookupCCW[i];
//	}
//	motorController.coggDriveCWMean /= ANTCOGGPOINTS;
//	motorController.coggDriveCCWMean /= ANTCOGGPOINTS;
//
//	motorController.meanFrictionDrive = MathExtras::Abs(motorController.coggDriveCWMean) + MathExtras::Abs(motorController.coggDriveCCWMean);
//	motorController.meanFrictionDrive *= 0.5f;

}

void ProgramMainLoopUpdate()
{

	static int counter = 0;



	  MotorErrorState chipStateA = ReadMotorErrorState(&motorDevA);
	  if(chipStateA == MotorErrorState_Fault )
	  {
		  //printf("chip in fault state!");
//		  SetMotorEnable(&motorDevA, 1, 0);
//		  SetMotorEnable(&motorDevA, 2, 0);
//		  SetMotorEnable(&motorDevA, 3, 0);

		  Error_Handler();

		  HAL_Delay(100);

		  ResetMotorChip(&motorDevA);
	  }


	  HandleSerialAPI(&huart3);




	  //HAL_GPIO_WritePin(Board_Status_LED_GPIO_Port, Board_Status_LED_Pin, (GPIO_PinState)DetermineCapSense(&knobCapSense));






//
//	  printf("%f,%f\r\n",motorController.motorDevice->angleState.currentAngularVelFiltered,
//			  motorController.motorDevice->angleState.currentAngle);

	 // printf("%d\r\n", knobCapSense.CAP_SENSE_LATEST_RISE_CNT);


	  if(motorDevA.angleState.currentAngle > 0.5f)
	  {
		  HAL_GPIO_WritePin(Board_Status_LED_GPIO_Port, Board_Status_LED_Pin,GPIO_PIN_SET);
	  }
	  else
	  {
		  HAL_GPIO_WritePin(Board_Status_LED_GPIO_Port, Board_Status_LED_Pin,GPIO_PIN_RESET);
	  }

	  counter++;
}

}
