/*
 * Mode_Utility.cpp
 *
 *  Created on: Mar 12, 2021
 *      Author: casht
 */




#include "Modes/Mode_Utility.h"
#include "BlockProcessing.h"
#include "PCM3168.h"


Mode_Utility::Mode_Utility() : Mode()
{
	SetBothNames("Mode_Utility");

}
Mode_Utility::~Mode_Utility()
{

}

void Mode_Utility::Initialize() {

	Mode::Initialize();

	inputOutputDescriptors[0].funcCombo = true;
	inputOutputDescriptors[0].quickAction = false;

	inputOutputDescriptors[3].funcCombo = true;

	inputOutputDescriptors[4].funcCombo = true;

	if(subMode == 1 || subMode == 4)
	{
		channelInterpEnables[0] = 0;
		channelInterpEnables[1] = 0;
		channelInterpEnables[2] = 0;
		channelInterpEnables[3] = 0;
	}
	else
	{
		channelInterpEnables[0] = 1;
		channelInterpEnables[1] = 1;
		channelInterpEnables[2] = 1;
		channelInterpEnables[3] = 1;
	}

	bypassGuestureInput = true;
}
//
//inline void Mode_Utility::UnInitialize() {
//}

inline void Mode_Utility::UpdateLEDS(float sampleTime) {

//if(firstLEDFrame)
//	WS2812_SETALLLED(1, 1, 1);

if(subMode == 0)
{
	WS2812_SETALLLED(10, 10, 10);//white
}
else if(subMode == 1)
{
	WS2812_SETALLLED(0, 10, 10);//cyan
}
else if(subMode == 2)
{
	WS2812_SETALLLED(10, 0, 10);//purple
}
else if(subMode == 3)
{
	WS2812_SETALLLED(10, 10, 0);//yellow
}
else if(subMode == 4)
{
	WS2812_SETALLLED(5, 2, 10);//deep purple
}
}
inline void Mode_Utility::AudioDSPFunction(float sampleTime, int bufferSwap)
{




	if(subMode == 0)
	{
		for(int i = 0; i < 4; i++)
		{
			dacOuts[i] = adc0DMA[i];
		}
	}
	else if(subMode == 1)//
	{

		BLOCK_PROCESS_ALL()
			dac0DMA[sampleIdxDMA] = -adc0DMA[sampleIdxDMA] ;
			dac1DMA[sampleIdxDMA] = -adc1DMA[sampleIdxDMA] ;
		}


	}
	else if(subMode == 2)//VOLT to VOLT Pass through.
	{
		for(int i = 0; i < 4; i++)
		{
			SetDacVolts(i, adcVolts[i]);
		}
	}
	else if(subMode == 3)//Drive DAC at 0
	{



		for(int i = 0; i < 4; i++)
		{
			dacOuts[i] = VoltsToDac(i, 0.0f);
		}
	}
	else if(subMode == 4)
	{
		//static float s = 0.0f;
		const float atten = 0.2f;

		BLOCK_PROCESS_ALL()

			//simple spring oscillator
		 	double adc = ADC_TO_FLOAT(adc0DMA[adc0Idx]);
			adc = ADCToVolts(0, adc)/ER_CV_VOLTAGE_MAX;
			double w = 2000.0*M_TWO_PI*0.5*adc;

			osc.Process(w,SAMPLETIME);

			dac0DMA[sampleIdxDMA] = int32_t(osc.x*atten*INT32_MAX) >> 8;
			dac1DMA[sampleIdxDMA] = int32_t(osc.x*atten*INT32_MAX) >> 8;
		}

	}
}

inline void Mode_Utility::KnobDSPFunction(float sampleTime) {


	if(motorCalibrationMode == 1)
	{
		if(calModetransition)
			calModetransition = false;

		driveTorque = 1.0f;

		if(LocalMotorAngleState.currentAngularVelFiltered > 1.0f)
		{
			calibrationMotorDirection = 1;
			motorCalibrationMode++;
			calModetransition = true;
		}
		else if(LocalMotorAngleState.currentAngularVelFiltered < -1.0f)
		{
			calibrationMotorDirection = -1;
			motorCalibrationMode++;
			calModetransition = true;
		}
	}

	else if(motorCalibrationMode == 2)
	{
		if(calModetransition)
		{
			calModetransition = false;
			timer = 1.0f;
			angleVar1 = MotorAngleStateGetCurrentAccumulatedAnglef(&LocalMotorAngleState);
		}

		//frictionCalFactor
		//drivePowerFactor
		//driveOffset

		driveTorque = 1.0;
		//driveOffset = 0.0f;
		frictionCalFactor = 0.0f;


		timer -= sampleTime;
		if(timer <= 0.0f)
		{

			forwardTestVel1 = LocalMotorAngleState.currentAngularVelFiltered;
			angleVar2 =  MotorAngleStateGetCurrentAccumulatedAnglef(&LocalMotorAngleState) - angleVar1;
			//printf("Forward Phase Finished: v: %f, delta Angle: %f\n", LocalMotorAngleState.currentAngularVelFiltered, angleVar2);
			motorCalibrationMode++;
			calModetransition = true;

		}

		//calibrate to vel = 1.55f

	}
	else if(motorCalibrationMode == 3)
	{
		if(calModetransition)
		{
			calModetransition = false;
			timer = 1.0f;
			angleVar1 = MotorAngleStateGetCurrentAccumulatedAnglef(&LocalMotorAngleState);
		}

		driveTorque = -1.0;
		//driveOffset = 0.0f;
		frictionCalFactor = 0.0f;

		timer -= sampleTime;
		if(timer <= 0.0f)
		{

			angleVar3 =  MotorAngleStateGetCurrentAccumulatedAnglef(&LocalMotorAngleState) - angleVar1;

			//printf("Backward Phase Finished: v: %f, delta Angle: %f\n", LocalMotorAngleState.currentAngularVelFiltered, angleVar2);
			float bias = angleVar2 + angleVar3;
			float range = MathExtras::Abs(angleVar2) + MathExtras::Abs(angleVar3);
			printf("bias: %f, range: %f, driveoffset: %f\n", bias, range, driveOffset);

			driveOffset += bias*0.001f;
			if(MathExtras::Abs(bias)/range < 0.001f)
			{
				motorCalibrationMode++;
			}
			else
			{
				motorCalibrationMode=2;
			}

			calModetransition = true;

		}
	}
	else if(motorCalibrationMode == 4)
	{
		if(calModetransition)
		{
			calModetransition = false;
			timer = 10.0f;
		}


		//driveTorque += posError*10.0f;
		driveTorque = 1.0f;
		frictionCalFactor = 100.0f;

		float velError = 1.5f - MathExtras::Abs(LocalMotorAngleState.currentAngularVelFiltered);
		drivePowerFactor += (velError)*0.001f;


		drivePowerFactor = MathExtras::ClampInclusive(drivePowerFactor, 0.0f, 1.0f);


		if(LocalMotorAngleState.currentAngularVelFiltered > 1.0f)
		{
			calibrationMotorDirection = 1;
		}
		else if(LocalMotorAngleState.currentAngularVelFiltered < -1.0f)
		{
			calibrationMotorDirection = -1;
		}


		if(timer <= 0.0f)
		{
			motorCalibrationMode++;
			calModetransition = true;
		}

		timer -= sampleTime;

	}
	else if(motorCalibrationMode == 5)
	{

		if(calModetransition)
		{
			calModetransition = false;
			timer = 30.0f;
			angleVar1 = 0.0f;
			frictionCalFactor = 0.0f;
		}


		float posError = angleVar1 - MotorAngleStateGetCurrentAccumulatedAnglef(&LocalMotorAngleState);
		if(posError >= 0.0f && angleVar5 <= 0.0f)
		{
			angleVar2 = 0.0f;
			oscilationRangeFiltered += (angleVar4 - oscilationRangeFiltered)*0.1f;
		}
		else if(posError <= 0.0f && angleVar5 >= 0.0f)
		{
			angleVar3 = 0.0f;
			oscilationRangeFiltered += (angleVar4 - oscilationRangeFiltered)*0.1f;
		}

		if(posError > angleVar2)
		{
			angleVar2 = posError;
		}
		if(posError < angleVar3)
		{
			angleVar3 = posError;
		}
		angleVar4 = angleVar2 - angleVar3;

		angleVar5 = posError;
		driveTorque += posError*10.0f;

		if(MathExtras::Abs(MotorAngleStateGetCurrentAccumulatedAnglef(&LocalMotorAngleState)) < 2.0f)
		{
			timer -= sampleTime;
			frictionCalFactor += (0.34f - oscilationRangeFiltered)*0.001f;
			frictionCalFactor = MathExtras::ClampInclusive(frictionCalFactor, 0.0f, 2.0f);
		}
		else
		{
			frictionCalFactor = 0.0f;
		}
		if(timer <= 0.0f)
		{
			motorCalibrationMode++;
			calModetransition = true;
		}
	}


	else
	{
		driveTorque = 0;
	}


	MasterProcess_Torque(sampleTime);
}
//
//inline void Mode_Utility::ButtonDown(int button) {
//}
//
void Mode_Utility::ButtonUp(int button) {

}

void Mode_Utility::OnFuncCombo(int button)
{
	if(button == 0)
	{
		//Cycle
		subMode++;

		if(subMode > 4)
			subMode = 0;


		if(subMode == 1 || subMode == 4)
		{
			channelInterpEnables[0] = 0;
			channelInterpEnables[1] = 0;
			channelInterpEnables[2] = 0;
			channelInterpEnables[3] = 0;
		}
		else
		{
			channelInterpEnables[0] = 1;
			channelInterpEnables[1] = 1;
			channelInterpEnables[2] = 1;
			channelInterpEnables[3] = 1;
		}
	}
	else if(button == 3)
	{


		PCM3168_Toggle48DeEmph();

	}

	if(button == 4)
	{
		motorCalibrationMode=2;
		calModetransition = true;


		frictionCalFactor = 0.0f;
		drivePowerFactor = 0.25f;
		driveOffset = 0.0f;


		//if(motorCalibrationMode > 1)
		//	motorCalibrationMode = 0;
		printf("Entering Calibration Mode.\n");
	}
}
//
inline void Mode_Utility::DebugPrint() {

	//if(motorCalibrationMode != 0)
	//	printf("Motor Cal Mode: %d\n", motorCalibrationMode);
	//printf("v: %f, timer: %f\n", LocalMotorAngleState.currentAngularVelFiltered, timer);
	//printf("dpf: %f, v: %f\n", drivePowerFactor, LocalMotorAngleState.currentAngularVelFiltered);
	printf("a: %f, md: %d, driveOffset: %f, range: %f, fcf: %f, dpf: %f, timer: %f\n", MotorAngleStateGetCurrentAccumulatedAnglef(&LocalMotorAngleState),
			calibrationMotorDirection, driveOffset, oscilationRangeFiltered, frictionCalFactor, drivePowerFactor, timer);
}

