/*
 * Program.cpp
 *
 *  Created on: Jul 29, 2020
 *      Author: casht
 */

#include <CircularQueueBuffer.h>
#include <MathExtras_Color.h>
#include <Modes/Mode_Torque_Friction.h>
#include <Modes/Mode_TurnTable.h>
#include "main.h"
#include "SerialCommands.h"
#include "UART3Handling.h"
#include "SPIHandling.h"
#include "DACX0504.h"
#include "PCM3168.h"
#include "us_delay.h"
#include "LEDPanelManager.h"
#include "DualBoardComHandler.h"
#include "HAL_Extras.h"
#include "profiling.h"
#include "MathExtras_Curves.h"
#include "SDPagedBuffer.h"
#include "MotorAngleState.h"
#include "MotorController.h"

#include "Modes/Mode.h"
#include "Modes/Mode_Spring1.h"
#include "Modes/Mode_Indent1.h"
#include "Modes/Mode_Indent2.h"
#include "Modes/Mode_IndentRouting.h"
#include "Modes/Mode_Sequencer.h"
#include "Modes/Mode_Rachet.h"
#include "Modes/Mode_Utility.h"
#include "Modes/Mode_Knob1.h"
#include "Modes/Mode_SequencedPluck.h"
#include "Modes/Mode_TorqueCurve.h"
#include "Modes/Mode_Orbits.h"
#include "Modes/Mode_Chords.h"
#include "Modes/Mode_Clocks.h"

#include "adcdacbuffers.h"

#include "math.h"
#include <cstring>
#include <limits>
#include "stdio.h"

#include "usart.h"
#include "spi.h"
#include "dac.h"
#include "tim.h"
#include "rng.h"
#include "sai.h"
#include "ee24.h"



CircularQueueBuffer<uint8_t, UART3_RX_BuffSize> SerialDevice_RX_Buffer;
CircularQueueBuffer<uint8_t, UART3_TX_BuffSize> SerialDevice_TX_Buffer;


DACX0504Chip DACChip;
LEDPanelManager LEDManager;
DualBoardComHandler ComHandler;
ModeManager modeManager;
SerialDevice serialCOMDevice;

#if defined(COMBINEDBOARD)
MotorDevice motorDevA;
MotorController motorController;
AS5048_Device motorEncoderDev;

#endif

uint16_t MotorSpiOutData_Latest = 0;
uint16_t MotorAngleData_Latest = 0;

MotorAngleState GlobalMotorAngleState;

//extern DMA_HandleTypeDef hdma_adc1;
extern FIL EEPROMFILE;
//Calibration Points For Interpolated calibration for V/Octave.  each point is a volt, -9v to 9v
float DACCalibrationPoints[4][NUM_DAC_ADC_CAL_POINTS] =
{ 0 };

float DACADCCalibrationPointsVolts[NUM_DAC_ADC_CAL_POINTS] = {
		-9.0f,
		-8.0f,
		-7.0f,
		-6.0f,
		-5.0f,
		-4.0f,
		-3.0f,
		-2.0f,
		-1.0f,
		 0.0f,
		 1.0f,
		 2.0f,
		 3.0f,
		 4.0f,
		 5.0f,
		 6.0f,
		 7.0f,
		 8.0f,
		 9.0f
};


float ADCCalibrationPoints[4][NUM_DAC_ADC_CAL_POINTS] = { 0 };



#define READMETEXT "Firmware Version: %lu \r\n\r\n" \
"Warning: Do not delete or modify the EEPROM file as it contains factory calibrations.\r\n\r\n" \
"To Update Firmware:  Copy the appropriate FIRMWARE.bin file from submatrixaudio.com/downloads to this SD card.  Place the SD card into the module (with the power off). Then power on the module."


void FixCalibrations()
{
	return;
	//Fix any bad adc numbers and apply offset
	for(int channelIdx = 0; channelIdx < 4; channelIdx++)
	{
		for(int i = 0; i < NUM_DAC_ADC_CAL_POINTS; i++)
		{
			if(i > 0)
			{
				if(ADCCalibrationPoints[channelIdx][i] >= ADCCalibrationPoints[channelIdx][i-1])
					ADCCalibrationPoints[channelIdx][i] = ADCCalibrationPoints[channelIdx][i-1] - 5;

				if(DACCalibrationPoints[channelIdx][i] >= DACCalibrationPoints[channelIdx][i-1])
					DACCalibrationPoints[channelIdx][i] = DACCalibrationPoints[channelIdx][i-1] - 5;

			}
		}
	}
}

arm_linear_interp_instance_f32 VoltToADCCalInterpInst[4];
arm_linear_interp_instance_f32 VoltToDACCalInterpInst[4];


//Create Modes
Mode_Spring1 mode_Spring1;
Mode_TorqueFriction mode_TorqueFriction;
Mode_Indent1 mode_Indent1;
Mode_Rachet mode_Rachet;
Mode_TurnTable mode_DJ;
Mode_Knob1 mode_Knob1;
Mode_Knob1 mode_Knob1Hidden;
Mode_Utility mode_Utility;
Mode_SequencedPluck mode_Pluck;
Mode_TorqueCurve mode_TorqueCurve;
Mode_Orbits mode_Orbits;
Mode_Chords mode_Chords;
Mode_Clocks mode_Clocks;


extern "C" {


	void UserCodeInit1()
	{
		//init default calibration points arrays, numbers from standard unit.
		for( int i = 0; i < 4; i++)
		{
			for(int j = 0 ; j < NUM_DAC_ADC_CAL_POINTS ; j++)
			{
				ADCCalibrationPoints[i][j] = 0.0f;
				DACCalibrationPoints[i][j] = 0.0f;
			}
			ADCCalibrationPoints[i][0] =  1.0f;
			DACCalibrationPoints[i][0] = -1.0f;
			ADCCalibrationPoints[i][1] =  0.93346f;
			DACCalibrationPoints[i][1] = -0.98839f;


			ADCCalibrationPoints[i][NUM_DAC_ADC_CAL_POINTS/2] = 0.20525f;
			DACCalibrationPoints[i][NUM_DAC_ADC_CAL_POINTS/2] = -0.00335f;

			ADCCalibrationPoints[i][NUM_DAC_ADC_CAL_POINTS-1] = -1.0f;
			DACCalibrationPoints[i][NUM_DAC_ADC_CAL_POINTS-1] =  1.0f;
			ADCCalibrationPoints[i][NUM_DAC_ADC_CAL_POINTS-2] = -0.52156f;
			DACCalibrationPoints[i][NUM_DAC_ADC_CAL_POINTS-2] =  0.9815625f;
		}

		/*
		for( int i = 0; i < ANTCOGGPOINTS; i++)
		{
			Mode::coggDriveLookupCW[i] = 0.0f;
			Mode::coggDriveLookupCCW[i] = 0.0f;

			Mode::coggDriveLookupCW_SMPLCNT[i] = 0;
			Mode::coggDriveLookupCCW_SMPLCNT[i] = 0;
		}
		*/

#if !defined(COMBINEDBOARD)
		serialCOMDevice.huart = &huart3;
#else
		serialCOMDevice.usb = true;
#endif

	}


	void DoFirstBoardBootActions(bool clrFactoryCals = true, bool clrUserCals = true, bool resetSerial = true)
	{
		//consider the eeprom is un-initialized, time to put some default things into eeprom. (State 1 Factory Reset)
		printf("Executing (DoFirstBoardBootActions)\r\n");

		//load default adc and dac calibrations into eeprom for user and factory
		bool writeSuccess = true;
		for(int channelIdx = 0; channelIdx < 4; channelIdx++)
		{
			for(int i = 0; i < NUM_DAC_ADC_CAL_POINTS; i++)
			{
				if(clrUserCals)
				{
					writeSuccess &= ee24_write_32(EE_DAC0_CAL_32bits + NUM_DAC_ADC_CAL_POINTS*4*channelIdx + i*4, *((uint32_t*)&DACCalibrationPoints[channelIdx][i]), 1000);
					writeSuccess &= ee24_write_32(EE_ADC0_CAL_32bits + NUM_DAC_ADC_CAL_POINTS*4*channelIdx + i*4, *((uint32_t*)&ADCCalibrationPoints[channelIdx][i]), 1000);
				}
				if(clrFactoryCals)
				{
					writeSuccess &= ee24_write_32(EE_DAC0_FACTORY_CAL_32bits + NUM_DAC_ADC_CAL_POINTS*4*channelIdx + i*4, *((uint32_t*)&DACCalibrationPoints[channelIdx][i]), 1000);
					writeSuccess &= ee24_write_32(EE_ADC0_FACTORY_CAL_32bits + NUM_DAC_ADC_CAL_POINTS*4*channelIdx + i*4, *((uint32_t*)&ADCCalibrationPoints[channelIdx][i]), 1000);
				}
				if(writeSuccess == false)
				{
					strcpy(errorCode, "DoFirstBoardBootActions EEPROM Write Error");
					Error_Handler();
				}
			}
		}





		//Clear All EEPROM except persistent (factory and user calibrations)
		ee24_write_ffff(EE_LASTMODE, EE_END - EE_LASTMODE, 1000);

		//reset motor zero phase angle (missing on versions < 17)
		ee24_write_float(EE_MotorZeroPhaseAngle_32bits, 0.0f, 1000);

		//Clear files
		FRESULT res = f_unlink("GESTUREF");
		if(res != FR_OK && res != FR_NO_FILE)
			Error_Handler();

		res = f_unlink("TTable");
		if(res != FR_OK && res != FR_NO_FILE)
			Error_Handler();



		//create readme file
		res = f_unlink("ReadMe.txt");
		if(res != FR_OK && res != FR_NO_FILE)
			Error_Handler();

		FIL readmeFile;
		res = f_open(&readmeFile, "README.txt", FA_OPEN_ALWAYS | FA_WRITE);
		if(res != FR_OK)
			Error_Handler();

		//write firmware version to file
		char header[sizeof(READMETEXT)+1] = {0};
		UINT bw;
		sprintf(header, READMETEXT, firmwareVersion);
		res = f_write(&readmeFile, header, strlen(header), &bw);
		if(res != FR_OK)
			Error_Handler();
		res = f_close(&readmeFile);
		if(res != FR_OK)
			Error_Handler();



		//initialize mode eeprom sections with the starting values of each mode
		for(int i = 0; i < modeManager.modesListEndIdx; i++)
		{
			uint32_t eepromBaseModeAddr = modeManager.ModeEEPromBase(i);
			modeManager.modes[i]->WriteEEPROMState(eepromBaseModeAddr);
		}


		//write serial number 0.  serial number 0 is reserved.
		if(resetSerial)
			ee24_write_32(EE_SERIALNUMBER_32Bit, 0, 1000);

		//write the eeprom check code. and firmware ver
		ee24_write_16(EE_EEPROMCheckCode_16bit, 0xABAB, 1000);
		ee24_write_32(EE_FIRMWARE_VERSION_32bits, firmwareVersion, 1000);

		ee24_flush();
	}






	void InitPCM3168();

	void BoardVerificationInit()
	{
		HAL_StatusTypeDef status;


		MotorDeviceInit(&motorDevA, DRV8313_EN1_GPIO_Port, DRV8313_EN1_Pin,
					  DRV8313_EN2_GPIO_Port,   DRV8313_EN2_Pin,
					  DRV8313_EN3_GPIO_Port,   DRV8313_EN3_Pin,
					  DRV8313_IN1_GPIO_Port, DRV8313_IN1_Pin,
					  DRV8313_IN2_GPIO_Port, DRV8313_IN2_Pin,
					  DRV8313_IN3_GPIO_Port, DRV8313_IN3_Pin,

					  &htim3,
					  TIM_CHANNEL_1,
					  TIM_CHANNEL_2,
					  TIM_CHANNEL_4,

					  DRV8313_A_nRESET_GPIO_Port, DRV8313_A_nRESET_Pin,
					  0, 0,
					  100,
					  DRV8313_A_nFAULT_GPIO_Port, DRV8313_A_nFAULT_Pin,
					  1,
					  22, 0.8f);

		MotorAngleStateInit(&motorDevA.angleState);

		AS5048DeviceInit(&motorEncoderDev, &hspi5, AS5048_CS_GPIO_Port, AS5048_CS_Pin);

		ResetMotorChip(&motorDevA);

		SetMotorEnable(&motorDevA, 1, GPIO_PIN_SET);
		SetMotorEnable(&motorDevA, 2, GPIO_PIN_SET);
		SetMotorEnable(&motorDevA, 3, GPIO_PIN_SET);
	}

	void BoardVerificationLoopUpdate()
	{
		HAL_Delay(1000);
		HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
	}




	void UserCodeInit2()
	{
		HAL_StatusTypeDef status;

		WS2812_INIT();
		LEDManager.SetLEDRingRangeLinear_Float(LEDPanelManager::RINGIDENTIFIER_OUTER, 0.0f, 1.0f, (MathExtras::Color*)&MathExtras::Color::BLUE, (MathExtras::Color*)&MathExtras::Color::BLUE, 1.0f);
		WS2812_SWAP_BUFFERS();

		printf("Initializing Firmware %lu..\r\n", firmwareVersion);

		//Get SD and FatFs running.
		if(f_mount(&SDFatFS, (TCHAR const*)SDPath, 0) != FR_OK)
		{
		  strcpy(errorCode, "f_mount sdcard error");
		  Error_Handler();
		}



		InitUSDelay();

		for(int i = 0; i < 4; i++)
		{
			VoltToADCCalInterpInst[i].nValues = NUM_DAC_ADC_CAL_POINTS;
			VoltToADCCalInterpInst[i].pYData = &ADCCalibrationPoints[i][0];
			VoltToADCCalInterpInst[i].xSpacing = 1.0f;
			VoltToADCCalInterpInst[i].x1 = DACADCCalibrationPointsVolts[0];

			VoltToDACCalInterpInst[i].nValues = NUM_DAC_ADC_CAL_POINTS;
			VoltToDACCalInterpInst[i].pYData = &DACCalibrationPoints[i][0];
			VoltToDACCalInterpInst[i].xSpacing = 1.0f;
			VoltToDACCalInterpInst[i].x1 = DACADCCalibrationPointsVolts[0];
		}





		modeManager.Initialize();

		//Page 1
		Mode* mode1 = modeManager.AddMode(&mode_DJ);
		modeManager.AddMode(&mode_TorqueFriction);
		modeManager.AddMode(&mode_Spring1);
		modeManager.AddMode(&mode_TorqueCurve);
		modeManager.AddMode(&mode_Rachet);
		modeManager.AddMode(&mode_Orbits);
		modeManager.AddMode(&mode_Clocks);
		modeManager.AddMode(&mode_Indent1);



		//Page 2
		modeManager.AddMode(&mode_Chords);
		modeManager.AddMode(&mode_Pluck);

//		strcpy(mode_Knob1Hidden.modeUniqueName,"SuperPot");
//		mode_Knob1Hidden.isHidden = false;
//		modeManager.AddMode(&mode_Knob1Hidden);

		modeManager.AddMode(&mode_Utility);
		//modeManager.AddMode(&mode_Indent1);



		//if the serialNumber is 0xFFFFFFFF, this indicates the board is new.
		uint32_t serialNumber = 0xFFFFFFFF;
		uint16_t eepromcheckcode;
		ee24_read_32(EE_SERIALNUMBER_32Bit, &serialNumber, 1000);
		ee24_read_16(EE_EEPROMCheckCode_16bit, &eepromcheckcode, 1000);
		if(serialNumber == 0xFFFFFFFF || eepromcheckcode != 0xABAB)
		{
			printf("Doing New Board First Boot Actions..\r\n");
			DoFirstBoardBootActions(true,true,true);
		}

		uint32_t eepromFirmwareVersion = 0;
		ee24_read_32(EE_FIRMWARE_VERSION_32bits, &eepromFirmwareVersion, 1000);

		printf("EEPROM Version is: %lu\r\n", eepromFirmwareVersion);
		//Similarly if the firmware just updated or button 8 is held on startup, this will force a reset (not clearing factory/user calibrations or serialnumber)
		//Clears mode memory region.
		if((eepromFirmwareVersion != firmwareVersion) || HAL_GPIO_ReadPin(PUSH_BUTTON_OUT_4_GPIO_Port, PUSH_BUTTON_OUT_4_Pin))
		{
			printf("Doing Firmware Update Boot Actions..\r\n");
			DoFirstBoardBootActions(false,false,false);
		}





#ifndef NUCLEO_TEST
		//load adc and dac calibrations from eeprom
		bool readSuccess=true;
		for(int channelIdx = 0; channelIdx < 4; channelIdx++)
		{
			for(int i = 0; i < NUM_DAC_ADC_CAL_POINTS; i++)
			{
				float adc;
				float dac;
				readSuccess &= ee24_read_32(EE_DAC0_CAL_32bits + NUM_DAC_ADC_CAL_POINTS*4*channelIdx + i*4, (uint32_t*)&dac, 1000);
				readSuccess &= ee24_read_32(EE_ADC0_CAL_32bits + NUM_DAC_ADC_CAL_POINTS*4*channelIdx + i*4, (uint32_t*)&adc, 1000);

				DACCalibrationPoints[channelIdx][i] = dac;
				ADCCalibrationPoints[channelIdx][i] = adc;

			}
		}
		if(!readSuccess)
		{
			 strcpy(errorCode, "calibration read error");
			Error_Handler();
		}
#endif

		//read front plate flip
		ee24_read_32(EE_FLIPPED_FRONT_PLATE_32bits, (uint32_t*)(&LEDManager.panelReversed_), 1000);
		if(LEDManager.panelReversed_ != 0 && LEDManager.panelReversed_ != 1)
			LEDManager.panelReversed_ = 0;

		//load friction factor
		Mode::frictionCalFactor = FRICTIONFACTOR_DEF;
		ee24_read_float(EE_FrictionFactor_32bits, &Mode::frictionCalFactor, 1000, FRICTIONFACTOR_DEF);
		if(Mode::frictionCalFactor == 0.0f)
			Mode::frictionCalFactor = FRICTIONFACTOR_DEF;//friction val of 0.0f is always not actually good.
		Mode::frictionCalFactor = MathExtras::ClampInclusive(Mode::frictionCalFactor, FRICTIONFACTOR_MIN, FRICTIONFACTOR_MAX);


		uint32_t newCalibrationsPerformedFlag = 0xFFFFFFFF;
		ee24_read_32(EE_NewCalibrationsPerformed_32bits,&newCalibrationsPerformedFlag,1000);
		int combinedBoardMotorDirection = 1;
		if(newCalibrationsPerformedFlag != 0XABCDEFEF)
		{
			ee24_write_float(EE_DrivePowerFactor_32bits, DRIVEPOWERFACTOR_DEF, 1000);
			Mode::drivePowerFactor = DRIVEPOWERFACTOR_DEF;

			ee24_write_float(EE_DriveOffset_32bits, DRIVEOFFSET_DEF, 1000);
			Mode::driveOffset = DRIVEOFFSET_DEF;



			ee24_write_32(EE_CombinedBoardMotorDirection_32bits, 1, 1000);


			ee24_write_32(EE_NewCalibrationsPerformed_32bits, 0XABCDEFEF, 1000);
		}
		else
		{
			//load drive power factor
			ee24_read_float(EE_DrivePowerFactor_32bits, &Mode::drivePowerFactor, 1000, DRIVEPOWERFACTOR_DEF);
			Mode::drivePowerFactor = MathExtras::ClampInclusive(Mode::drivePowerFactor, DRIVEPOWERFACTOR_MIN, DRIVEPOWERFACTOR_MAX);

			//load drive offset
			ee24_read_float(EE_DriveOffset_32bits, &Mode::driveOffset, 1000, DRIVEOFFSET_DEF);
			Mode::driveOffset = MathExtras::ClampInclusive(Mode::driveOffset, DRIVEOFFSET_MIN, DRIVEOFFSET_MAX);

			ee24_read_32(EE_CombinedBoardMotorDirection_32bits, (uint32_t*)&combinedBoardMotorDirection, 1000);
			combinedBoardMotorDirection = MathExtras::ClampInclusive(combinedBoardMotorDirection, -1,1);
		}

		//restore taptempo state and colors..
		ee24_read_float(EE_TAP_TEMPO_TNEXT_32bits, &tempo.tNext,  1000, 1.0f);
		if(tempo.tNext == 0.0f) tempo.tNext = 1.0f;
		ee24_read_float(EE_TAP_TEMPO_BPS_32bits, &tempo.bps,  1000, 1.0f);
		if(tempo.bps == 0.0f) tempo.bps = 1.0f;
		ee24_read_float(EE_TAP_TEMPO_BPSFLT_32bits, &tempo.bps_filtered,  1000, 1.0f);
		if(tempo.bps_filtered == 0.0f) tempo.bps_filtered = 1.0f;

		tempo.nextTNext = tempo.tNext;


		uint8_t lastModeIdx;
		ee24_read(EE_LASTMODE, &lastModeIdx, 1, 1000);

		if(lastModeIdx < modeManager.modesListEndIdx)
			modeManager.TransitionToMode(modeManager.modes[lastModeIdx]);
		else
			modeManager.TransitionToMode(mode1);






		//input op-amp offset. (Match VCOMAD of PCM2168 until board changes)
		HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_2, DAC_ALIGN_12B_R, ((2.6f - 0.285f)/(3.3f))*4096);
		HAL_DAC_Start(&hdac1, DAC_CHANNEL_2);

		//switch level dac
		HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 0);
		HAL_DAC_Start(&hdac1, DAC_CHANNEL_1);


#if defined(COMBINEDBOARD)


		MotorDeviceInit(&motorDevA, DRV8313_EN1_GPIO_Port, DRV8313_EN1_Pin,
			  DRV8313_EN2_GPIO_Port,   DRV8313_EN2_Pin,
			  DRV8313_EN3_GPIO_Port,   DRV8313_EN3_Pin,
			  DRV8313_IN1_GPIO_Port, DRV8313_IN1_Pin,
			  DRV8313_IN2_GPIO_Port, DRV8313_IN2_Pin,
			  DRV8313_IN3_GPIO_Port, DRV8313_IN3_Pin,

			  &htim3,
			  TIM_CHANNEL_1,
			  TIM_CHANNEL_2,
			  TIM_CHANNEL_4,

			  DRV8313_A_nRESET_GPIO_Port, DRV8313_A_nRESET_Pin,
			  0, 0,
			  100,
			  DRV8313_A_nFAULT_GPIO_Port, DRV8313_A_nFAULT_Pin,
			  combinedBoardMotorDirection,
			  22, 0.8f);

		MotorAngleStateInit(&motorDevA.angleState);

		AS5048DeviceInit(&motorEncoderDev, &hspi5, AS5048_CS_GPIO_Port, AS5048_CS_Pin);


		ResetMotorChip(&motorDevA);


		//wait a bit beofre enabling motor to ignore any bad values spinning the motor on startup
		HAL_Delay(50);

		SetMotorEnable(&motorDevA, 1, GPIO_PIN_SET);
		SetMotorEnable(&motorDevA, 2, GPIO_PIN_SET);
		SetMotorEnable(&motorDevA, 3, GPIO_PIN_SET);

		ee24_read_float(EE_MotorZeroPhaseAngle_32bits, &motorController.motorAngleZeroPhase, 1000,0.0f);
		//do motor calibration if zero phase is unset.
		if(motorController.motorAngleZeroPhase == 0.0f){
			motorController.ZeroPhaseCalibration();
			motorController.curControlScheme = MotorController::ControlScheme_TorqueTarget;
			ee24_write_float(EE_MotorZeroPhaseAngle_32bits, motorController.motorAngleZeroPhase, 1000);
		}




#endif








		//NOTE: at this time point the motor board should be waiting to receive SPI!  So we can start the Knob Processing Timer Below.
		MotorAngleStateInit(&GlobalMotorAngleState);


		//Start the processing timer.
		HAL_TIM_Base_Start_IT(&htim2);
		//HAL_TIM_Base_Start_IT(&htim3);

		HAL_TIM_Base_Start(&htim5);

		//ignore any UART Bits that have come during this time..
		SerialDevice_RX_Buffer.readIndex = SerialDevice_RX_Buffer.headIndex;


#ifndef NUCLEO_TEST
		InitPCM3168();
#endif

		status = HAL_SAI_Receive_DMA(&hsai_BlockA2, (uint8_t*)&adc0DMA, ADCDMABUFFERSIZE);
		if(status != HAL_OK)
			Error_Handler();

		status = HAL_SAI_Receive_DMA(&hsai_BlockB2, (uint8_t*)&adc1DMA, ADCDMABUFFERSIZE);
		if(status != HAL_OK)
			Error_Handler();



	    //turn off complete and half complete interrupts for adc dma
//	    ((DMA_Stream_TypeDef   *)DMA1_Stream1)->CR &= ~(DMA_SxCR_TCIE_Msk | DMA_SxCR_HTIE_Msk);
//	    	HAL_Delay(100);

		status = HAL_SAI_Transmit_DMA(&hsai_BlockA1, (uint8_t*)&dac0DMA, (DACDMABUFFERSIZE));
		if(status != HAL_OK)
			Error_Handler();

		status = HAL_SAI_Transmit_DMA(&hsai_BlockB1, (uint8_t*)&dac1DMA, (DACDMABUFFERSIZE));//TODO check if this call keeps things synced.
		if(status != HAL_OK)
			Error_Handler();




	    //turn off complete and half complete interrupts for block b dac dma because block b is in sync with block a, and we block-process on a.
	    ((DMA_Stream_TypeDef   *)DMA1_Stream1)->CR &= ~(DMA_SxCR_TCIE_Msk | DMA_SxCR_HTIE_Msk);

	    //turn off complete and half complete interrupts for SAI2 as it should be in sync
	    ((DMA_Stream_TypeDef   *)DMA1_Stream2)->CR &= ~(DMA_SxCR_TCIE_Msk | DMA_SxCR_HTIE_Msk);
	    ((DMA_Stream_TypeDef   *)DMA1_Stream3)->CR &= ~(DMA_SxCR_TCIE_Msk | DMA_SxCR_HTIE_Msk);







	}


	void ProgramMainLoopUpdate()
	{
		  static uint32_t lastTick = HAL_GetTick();
		  static int counter = 0;


		  HAL_StatusTypeDef status =  HandleSerialAPI(&serialCOMDevice);


		  Mode* curMode = modeManager.currentMode();

		  static int plugStateIdx = 0;
		  static bool initialPlugs = true;

		  const float thresh = 0.06f;
		  //Determine Plugin State For Inputs by modulating dac

		  for(int i = 0; i < 4; i++)
		  {
			  float adcAsFloat;
			  if(i<2)
				  adcAsFloat = float(adc0DMA[i%2] << 8)/INT32_MAX;
			  else
				  adcAsFloat = float(adc1DMA[i%2] << 8)/INT32_MAX;

			  if((adcAsFloat < thresh && PlugStateDACSequence[plugStateIdx] == 1)
					  ||
			  (adcAsFloat > thresh && PlugStateDACSequence[plugStateIdx] == 0))
			  {
				  if(adcPlugCounts[i] < 0xFFFF)
					  adcPlugCounts[i]++;
			  }
			  else
			  {
				  	  adcPlugCounts[i]=0;
			  }



			  if(!initialPlugs)
			  {
				  adcPlugStates_1[i] = adcPlugStates[i];
				  if(adcPlugCounts[i] < ADC_PLUG_COUNT_THRESH)
				  {
					  adcPlugStates[i] = 1;//Plugged-IN
				  }
				  else
				  {
					  adcPlugStates[i] = 0;//Un-Plugged
				  }


				  if(adcPlugStates_1[i] != adcPlugStates[i])
				  {
					  curMode->OnADCPlugChange(i);
				  }
			  }
		  }

		  plugStateIdx++;
		  if(plugStateIdx >= 128)
		  {
			  plugStateIdx = 0;
			  initialPlugs = false;
		  }

		  HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R,  PlugStateDACSequence[plugStateIdx]*4095);
		  HAL_DAC_Start(&hdac1, DAC_CHANNEL_1);




		  double sampleTime = (double(htim5.Instance->CNT)/htim5.Instance->ARR)*TIM5_RESET_TIME;
		  htim5.Instance->CNT = 0;



		  if(fullyStartedUp)
		  {
//			  static bool once = true;
//			  if(once){
//				  //Mode::DoAntiCoggCalibration();
//			  	  once = false;
//			  }


			  curMode->UpdateLEDS(sampleTime);
			  curMode->PostLEDUpdate(sampleTime);
			  modeManager.UpdateButtons();


			   //flush eeprom
			  f_sync(&EEPROMFILE);
			  modeManager.currentMode()->MainThreadUpdateFunction(sampleTime);
			  modeManager.currentMode()->DebugPrint();
		  }

		  WS2812_SWAP_BUFFERS();



		  //read dip switches
		  //GPIO_PinState dip2 = HAL_GPIO_ReadPin(DIP_SW_2_GPIO_Port, DIP_SW_2_Pin);
		  //GPIO_PinState dip3 = HAL_GPIO_ReadPin(DIP_SW_3_GPIO_Port, DIP_SW_3_Pin);
		  GPIO_PinState dip4 = HAL_GPIO_ReadPin(DIP_SW_4_GPIO_Port, DIP_SW_4_Pin);

		  panelReversed = int(!(bool)dip4);
		  //LEDManager.panelReversed_ = panelReversed;



		  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, HAL_GPIO_ReadPin(PCM3168_OVF_GPIO_Port ,PCM3168_OVF_Pin));






		  counter++;
		  if(counter > 150)
		  {

			  fullyStartedUp = 1;
		  }
		  else
		  {
				LEDManager.SetLEDRingRangeLinear_Float(LEDPanelManager::RINGIDENTIFIER_INNER, 0.0f, counter/150.0f, (MathExtras::Color*)&MathExtras::Color::MAGENTA, (MathExtras::Color*)&MathExtras::Color::MAGENTA, 1.0f);
				WS2812_SWAP_BUFFERS();
		  }


		  //target 120Hz Update Rate
		  while(HAL_GetTick() < lastTick + 8)
		  {
			  HAL_Delay(1);
		  }
		  lastTick = HAL_GetTick();
	}



	void InitPCM3168()
	{
		//reset the PCM3168
		HAL_GPIO_WritePin(PCM3168_RST_GPIO_Port, PCM3168_RST_Pin, GPIO_PIN_RESET);
		HAL_Delay(100);
		HAL_GPIO_WritePin(PCM3168_RST_GPIO_Port, PCM3168_RST_Pin, GPIO_PIN_SET);

		HAL_Delay(1000);

		//Turn off HPF
		//read ADC CONTROL 2 register
		uint8_t reg = PCM3168Read(0x52);
		//turn off HPF
		reg |= 0b00000111;
		PCM3168Write(0x52, reg);

		//make sure ADC attenuation 0 (not needed)
		//PCM3168Write(0x58,215);
		//PCM3168Write(0x59,215);
		//PCM3168Write(0x5A,215);
		//PCM3168Write(0x5B,215);

		//Make ADC's Single Ended
		PCM3168Write(0x53, 0b111111);




		PCM3168_Toggle48DeEmph();

	}


	void ErrorLoop()
	{
		LEDManager.SetKnobCenterLEDS(LED_BASE_BRIGHTNESS_255, 0, 0);

		WS2812_SWAP_BUFFERS();

	}
}





