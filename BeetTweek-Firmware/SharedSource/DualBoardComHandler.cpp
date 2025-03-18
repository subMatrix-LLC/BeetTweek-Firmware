/*
 * DualBoardComHanlder.cpp
 *
 *  Created on: Nov 25, 2020
 *      Author: casht
 */

#include <DualBoardComHandler.h>
#include "MathExtras.h"
#include "spi.h"

DualBoardComHandler::DualBoardComHandler() {
	// TODO Auto-generated constructor stub

}

DualBoardComHandler::~DualBoardComHandler() {
	// TODO Auto-generated destructor stub
}


#ifndef KNOB_STM
void DualBoardComHandler::Master_Process(uint16_t sig1Out, uint16_t sig2Out, uint16_t sig3Out, uint16_t sig4Out,
		uint16_t* sig1In, uint16_t* sig2In, uint16_t* sig3In, uint16_t* sig4In)
{
	//while(hspi3.State != HAL_SPI_STATE_READY) {}

	//puts sigs in arrays to send
	outs[0] = sig1Out;
	outs[1] = sig2Out;
	outs[2] = sig3Out;
	outs[3] = sig4Out;
	ins[0] = sig1In;
	ins[1] = sig2In;
	ins[2] = sig3In;
	ins[3] = sig4In;



	int s = 0;
	//check if its time to send the next scheme to the slave.  using the least sig figs of sig1Out as the carrier.
	//slave has its own counter as well.  In sync with the master.
	if(streamPackCounter == DUALBOARDCOMHANDLER_SCHEME_PACK_INTERVAL)
	{
		uint16_t outBits = (sig1Out & (0xFFFF << 2)) | ((~(0xFFFF << 2) & ((uint16_t)nextPackScheme.id)));
		outs[0] = outBits;

		streamPackCounter = 0;
	}

	wordCounter = 0;

	//send.
	for(;s < MathExtras::Max(currentPackScheme.numMasterOuts, currentPackScheme.numMasterIns) ; s++)
	{

		//HAL_GPIO_WritePin(MOTOR_SPI_SYNC_GPIO_Port, MOTOR_SPI_SYNC_Pin, GPIO_PIN_RESET);

	//	HAL_SPI_TransmitReceive(&hspi3, (uint8_t*)&outs[s], (uint8_t*)ins[s], 1,1000);

		/*HAL_StatusTypeDef status = */HAL_SPI_TransmitReceive_IT(&hspi6, (uint8_t*)&outs[s], (uint8_t*)ins[s], 1);


		//HAL_GPIO_WritePin(MOTOR_SPI_SYNC_GPIO_Port, MOTOR_SPI_SYNC_Pin, GPIO_PIN_SET);
		wordCounter++;
	}
	streamPackCounter++; //one pack sent




}
#else


void DualBoardComHandler::Slave_GetNextOut(uint16_t* sigOut)
{
	*sigOut = outs[wordCounter];
}

void DualBoardComHandler::Slave_ProcessNextIn(uint16_t in)
{

	ins[wordCounter] = in;


	//check if its time to receive the next scheme from the master.  extract the least sig figs of sig1In
	//this could change the current scheme which will immediately take effect.
	if(streamPackCounter == DUALBOARDCOMHANDLER_SCHEME_PACK_INTERVAL)
	{
		//uint16_t extractedBits = (ins[0] & ~(0xFFFF << 2));

		//if(currentPackScheme.id != (PackScheme::PackScheme_ID)extractedBits)
		//{			currentPackScheme.id = (PackScheme::PackScheme_ID)extractedBits;
			Slave_TransitionScheme();
		//}

		streamPackCounter = 0;
	}

	wordCounter++;

	if(wordCounter >= MathExtras::Max(currentPackScheme.numMasterOuts, currentPackScheme.numMasterIns))
	{
		Slave_ProcessSchemeFrame();
		wordCounter = 0;
	}
}
void DualBoardComHandler::Slave_TransitionScheme()
{
	//interface with controller..
//	if(currentPackScheme.id == DualBoardComHandler::PackScheme::PackScheme_MotorTorqueTarget)
//	{
		if(motorController.calibrationMode == MotorController::CalibrationMode_None)
		{
			motorController.curControlScheme = MotorController::ControlScheme_TorqueTarget;
		}
//	}
//	else if(currentPackScheme.id == DualBoardComHandler::PackScheme::PackScheme_MotorAngleTarget)
//	{
//		motorController.curControlScheme = MotorController::ControlScheme_AngleTarget;
//
//	}
}
void DualBoardComHandler::Slave_ProcessSchemeFrame()
{
	//interface with controller..
//	if(currentPackScheme.id == DualBoardComHandler::PackScheme::PackScheme_MotorTorqueTarget)
//	{
		//torque commanding with current angle return.
		outs[0] = motorDevA.angleState.currentAngle*65535;
		motorController.torqueCommand = (ins[0]/65535.0f)*1.5f - 1.5f*0.5f;

//	}
//	else if(currentPackScheme.id == DualBoardComHandler::PackScheme::PackScheme_MotorAngleTarget)
//	{
//		//angle target commanding with current angle return.
//
//
		//outs[0] = motorDevA.angleState.currentAngle*65535;
		//motorController.angleTarget = ins[0]/65535.0f;
//	}




	streamPackCounter++;//one pack received.
}

#endif
