/*
 * MotorController.cpp
 *
 *  Created on: Nov 18, 2020
 *      Author: casht
 */

#include "MotorController.h"

MotorController::MotorController() {


	PIDStateInit(&angleFollowPID);
	angleFollowPID.PGain = 5.0f;
	angleFollowPID.IGain = 0.000f;


	angleFollowPID.IClampHigh = 1;
	angleFollowPID.IClampLow = 1;
	angleFollowPID.IClampHighLevel = 2.0f;
	angleFollowPID.IClampLowLevel = -2.0f;



	PIDStateInit(&speedTargetPID);
	speedTargetPID.PGain = 0.2f;
	speedTargetPID.IGain = 0.001f;


	//2nd order butterworth cutoff of 60hz
	driveFilterCoeff[0] = 0.0007926127641769609;
	driveFilterCoeff[1] = 0.0015852255283539218;
	driveFilterCoeff[2] = 0.0007926127641769609;
	driveFilterCoeff[3] = 1.9188007733572694;
	driveFilterCoeff[4] = -0.9219712244139773;


	arm_biquad_cascade_df1_init_f32(&driveFilter, 1, &driveFilterCoeff[0], &driveFilterState[0]);

	for(int i = 0; i < ANTCOGGPOINTS; i++)
	{
		coggDriveLookupCW[i] = 0.0f;
		coggDriveLookupCCW[i] = 0.0f;

		coggDriveLookupCW_SMPLCNT[i] = 0;
		coggDriveLookupCCW_SMPLCNT[i] = 0;
	}


}

MotorController::~MotorController() {
}

void MotorController::SetZeroPhaseAngle()
{
	motorAngleZeroPhase = motorAngle;
}
void MotorController::ZeroPhaseCalibration()
{
	calibrationMode = CalibrationMode_ZeroPhase;
	curControlScheme = MotorController::ControlScheme_Lock;


	//Hold Motor Still While Reseting Motor Mechanical Angle.
	for(int i = 0; i < 100; i++)
	{
		UpdateMotorControl(0.01f);
		HAL_Delay(10);
	}



	SetZeroPhaseAngle();
	curControlScheme = MotorController::ControlScheme_Free;
	HAL_Delay(200);

	calibrationMode = CalibrationMode_None;
}



void MotorController::AntiCoggingCalibration()
{
	calibrationMode = CalibrationMode_AntiCogg;
	curControlScheme = MotorController::ControlScheme_SpeedTarget;
	speedTarget = -0.5f;


	do
	{
		bool cwSampleComplete=true;
		for(int i = 0; i < ANTCOGGPOINTS; i++)
		{
			//printf("%f\r\n",coggDriveLookupCW[i]);

		  if(coggDriveLookupCW_SMPLCNT[i] < ANTICOGGMINSAMPLES)
		  {
			  cwSampleComplete = false;
		  }

		}
		coggDriveLookupCW_SMPLCMPL = cwSampleComplete;
	} while(!coggDriveLookupCW_SMPLCMPL);


	speedTarget = 0.5f;

	do
	{
		bool ccwSampleComplete=true;
		for(int i = 0; i < ANTCOGGPOINTS; i++)
		{
		 // printf("%f\r\n",coggDriveLookupCCW[i]);
		  if(coggDriveLookupCCW_SMPLCNT[i] < ANTICOGGMINSAMPLES)
		  {
			  ccwSampleComplete = false;
		  }
		}
		coggDriveLookupCCW_SMPLCMPL = ccwSampleComplete;
	} while(!coggDriveLookupCCW_SMPLCMPL);


	calibrationMode = CalibrationMode_None;


}
void MotorController::UpdateMotorControl(float DeltaTime)
{


	//use for oscope debugging
	//HAL_GPIO_WritePin(Debug_Output2_GPIO_Port, Debug_Output2_Pin, GPIO_PIN_SET);


			//capture motor angle to rolling buffer
			motorAngle = AS5048_SPI_ReadAnglef(encoder);
			MotorAngleStateUpdateLocalf( &motorDevice->angleState, motorAngle, DeltaTime);



			//Determine Rotor Flux Angle
			rotorFluxAngle = (motorAngle - motorAngleZeroPhase)*(motorDevice->numPoles/2.0f) * M_TWO_PI;
			rotorFluxAngle = MathExtras::WrapMax<float>(rotorFluxAngle, M_TWO_PI);


			float curAccumAngle = MotorAngleStateGetCurrentAccumulatedAnglef(&motorDevice->angleState);
			angleError = angleTarget - curAccumAngle;



			if(curControlScheme == ControlScheme_PhysicallyModelled_Direct)
			{
//				float inertia = 10000.0f;
//				float springConstant = 2.0f;
//				float flywheelEngagefrictionFactor = 0.001f;

				driveStrength = -(motorDevA.angleState.currentAngularVelFiltered);
			}
			else if(curControlScheme == ControlScheme_ConstantTorque)
			{
				driveStrength = 1.0f;
			}
			else if(curControlScheme == ControlScheme_TorqueTarget)
			{
				driveStrength = torqueCommand;
			}
			else if(curControlScheme == ControlScheme_TorqueModeling)
			{
				//Integrate commanded torque to get change momentum. which is proportional to speed
				torqueIntegral += torqueCommand*DeltaTime;
				torqueIntegral = MathExtras::ClampInclusive(torqueIntegral, -1.0f, 1.0f);

				const float c1 = 7.81f;
				float velTarget = torqueIntegral*c1;


				//Adjust final drivestrenghth based on speed to reflect momentum.
				float Error = (velTarget - motorDevA.angleState.currentAngularVelFiltered)*0.005f;
				driveStrength += Error;

				driveStrength = MathExtras::ClampInclusive(driveStrength, -1.0f, 1.0f);
			}
			else if(curControlScheme == ControlScheme_AngleTarget)
			{
				driveStrength =  PIControl(angleTarget, curAccumAngle, &angleFollowPID);
			}
			else if(curControlScheme == ControlScheme_SpeedTarget)
			{
				driveStrength = PIControl(speedTarget, motorDevice->angleState.currentAngularVelFiltered, &speedTargetPID);
			}
			else if(curControlScheme == ControlScheme_Free)
			{
				driveStrength = 0.0f;
			}

			//add in proportional to the velocity always.  This is tuned to give a low friction for 0 drive strength. (if 0 PWM equated to off friction this would not be needed).
			//(PWM Deadtime compensation)
			//driveStrength += motorDevice->angleState.currentAngularVelFiltered*0.17f;

			//filter the final drive, this is to reduce mechanical noise for high frequency switching.
			arm_biquad_cascade_df1_f32(&driveFilter, &driveStrength, &driveStrengthFiltered, 1);



			//handle direction and wire flip
			driveAngle = MathExtras::WrapMax(float(rotorFluxAngle - MathExtras::Sign(driveStrengthFiltered)*float(motorDevice->direction)*M_PI_2 + phaseOffsetExtra), float(M_TWO_PI));
//
//			if(driveStrengthFiltered > 0.0f)
//				driveAngle = MathExtras::WrapMax(float(rotorFluxAngle + float(motorDevice->direction)*M_PI_2 + phaseOffsetExtra), float(M_TWO_PI));
//			else
//				driveAngle = MathExtras::WrapMax(float(rotorFluxAngle - float(motorDevice->direction)*M_PI_2 + phaseOffsetExtra), float(M_TWO_PI));

			if(curControlScheme == ControlScheme_Lock)
			{
				driveAngle = 0.0f;
				driveStrengthFiltered = 1.0f;
			}




			//SetMotorPWMDrives(&motorDevA, MotorDriveWaveForm_Sinusoidal,  fabs(driveStrengthFiltered), driveAngle);
			SetMotorPWMDrivesSpaceVector_ALTREV(&motorDevA, fabs(driveStrengthFiltered), driveAngle);



			timerPeriodCount++;



	//HAL_GPIO_WritePin(Debug_Output2_GPIO_Port, Debug_Output2_Pin, GPIO_PIN_RESET);
}
