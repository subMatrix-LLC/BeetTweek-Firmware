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
	
	// Initialize test variables
	testIteration = 0;
	maxTestIterations = 10;
	targetSpeed = 1.5f; // Target speed in rad/s
	targetPositionError = 0.1f; // Target position error in radians
	targetOscillationRange = 0.1f; // Target oscillation range in radians
	
	// Initialize parameter adjustment factors
	drivePowerFactorAdjust = 0.01f;
	frictionCalFactorAdjust = 0.01f;
	driveOffsetAdjust = 0.001f;
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
	if(motorCalibrationMode == 1) {
		if(calModetransition) {
			calModetransition = false;
			timer = CALIBRATION_TEST_DURATION;
			angleVar1 = MotorAngleStateGetCurrentAccumulatedAnglef(&LocalMotorAngleState);
			forwardTestVel1 = 0.0f;
			driveTorque = 1.0f;
			decelerationStartTime = 0.0f;
			decelerationStartSpeed = 0.0f;
			hasStartedDeceleration = false;
			printf("Starting Forward Test (Iteration %d):\n", testIteration);
			printf("Initial Angle: %f\n", angleVar1);
			printf("Initial DrivePowerFactor: %f\n", drivePowerFactor);
		}

		// Track maximum speed
		if(LocalMotorAngleState.currentAngularVelFiltered > forwardTestVel1) {
			forwardTestVel1 = LocalMotorAngleState.currentAngularVelFiltered;
		}

		// Start measuring deceleration when we stop driving
		if(timer <= CALIBRATION_COAST_PHASE && !hasStartedDeceleration) {
			driveTorque = 0.0f;
			decelerationStartTime = timer;
			decelerationStartSpeed = LocalMotorAngleState.currentAngularVelFiltered;
			hasStartedDeceleration = true;
			printf("Starting deceleration phase:\n");
			printf("Deceleration Start Speed: %f rev/s\n", decelerationStartSpeed);
		}

		timer -= sampleTime;
		if(timer <= 0.0f) {
			angleVar2 = MotorAngleStateGetCurrentAccumulatedAnglef(&LocalMotorAngleState) - angleVar1;
			float finalSpeed = LocalMotorAngleState.currentAngularVelFiltered;
			float decelerationTime = CALIBRATION_COAST_PHASE - timer;
			
			printf("Forward Test Results (Iteration %d):\n", testIteration);
			printf("Distance: %f rev\n", angleVar2);
			printf("Max Speed: %f rev/s\n", forwardTestVel1);
			printf("Final Speed: %f rev/s\n", finalSpeed);
			printf("Deceleration Time: %f s\n", decelerationTime);
			printf("Deceleration Rate: %f rev/s^2\n", (decelerationStartSpeed - finalSpeed) / decelerationTime);
			
			// Adjust drivePowerFactor based on both speed and distance errors
			float speedError = targetSpeed - forwardTestVel1;
			float distanceError = targetDriveDistance - angleVar2;
			drivePowerFactor += (speedError + distanceError) * drivePowerFactorAdjust;
			drivePowerFactor = MathExtras::ClampInclusive(drivePowerFactor, DRIVEPOWERFACTOR_MIN, DRIVEPOWERFACTOR_MAX);
			
			printf("New DrivePowerFactor: %f\n", drivePowerFactor);
			printf("Transition to backward test");
			
			motorCalibrationMode = 2; // Move to backward test
			calModetransition = true;
		}
	}
	else if(motorCalibrationMode == 2) {
		if(calModetransition) {
			calModetransition = false;
			timer = CALIBRATION_TEST_DURATION;
			angleVar1 = MotorAngleStateGetCurrentAccumulatedAnglef(&LocalMotorAngleState);
			driveTorque = -1.0f;
			decelerationStartTime = 0.0f;
			decelerationStartSpeed = 0.0f;
			hasStartedDeceleration = false;
		}

		// Start measuring deceleration when we stop driving
		if(timer <= CALIBRATION_COAST_PHASE && !hasStartedDeceleration) {
			driveTorque = 0.0f;
			decelerationStartTime = timer;
			decelerationStartSpeed = LocalMotorAngleState.currentAngularVelFiltered;
			hasStartedDeceleration = true;
		}

		timer -= sampleTime;
		if(timer <= 0.0f) {
			angleVar3 = MotorAngleStateGetCurrentAccumulatedAnglef(&LocalMotorAngleState) - angleVar1;
			float finalSpeed = LocalMotorAngleState.currentAngularVelFiltered;
			float decelerationTime = CALIBRATION_COAST_PHASE - timer;
			
			printf("Backward Test Results (Iteration %d):\n", testIteration);
			printf("Distance: %f rev\n", angleVar3);
			printf("Final Speed: %f rev/s\n", finalSpeed);
			printf("Deceleration Time: %f s\n", decelerationTime);
			printf("Deceleration Rate: %f rev/s^2\n", (decelerationStartSpeed - finalSpeed) / decelerationTime);
			
			// Calculate bias and range
			float bias = angleVar2 + angleVar3;
			float range = MathExtras::Abs(angleVar2) + MathExtras::Abs(angleVar3);
			
			// Adjust driveOffset based on bias
			driveOffset += bias * driveOffsetAdjust;
			driveOffset = MathExtras::ClampInclusive(driveOffset, DRIVEOFFSET_MIN, DRIVEOFFSET_MAX);
			
			// Adjust frictionCalFactor based on range and deceleration time
			float rangeError = targetPositionError - range;
			float decelError = targetDecelerationTime - decelerationTime;
			frictionCalFactor += (rangeError * frictionCalFactorAdjust + decelError * 0.1f);
			frictionCalFactor = MathExtras::ClampInclusive(frictionCalFactor, FRICTIONFACTOR_MIN, FRICTIONFACTOR_MAX);
			
			printf("Current Parameters:\n");
			printf("DrivePowerFactor: %f\n", drivePowerFactor);
			printf("FrictionCalFactor: %f\n", frictionCalFactor);
			printf("DriveOffset: %f\n", driveOffset);
			
			// Check if we've achieved target performance or max iterations
			if((MathExtras::Abs(bias)/range < 0.001f && 
				MathExtras::Abs(targetSpeed - forwardTestVel1) < 0.1f &&
				MathExtras::Abs(targetDecelerationTime - decelerationTime) < 0.05f &&
				MathExtras::Abs(targetDriveDistance - angleVar2) < 0.05f) || 
				testIteration >= maxTestIterations) {
				printf("Calibration Complete:\n");
				printf("Final DrivePowerFactor: %f\n", drivePowerFactor);
				printf("Final FrictionCalFactor: %f\n", frictionCalFactor);
				printf("Final DriveOffset: %f\n", driveOffset);
				motorCalibrationMode = 0;
			} else {
				testIteration++;
				printf("Transition to oscillation test");
				motorCalibrationMode = 3; // Move to oscillation test
			}
			calModetransition = true;
		}
	}
	else if(motorCalibrationMode == 3) {
		// Oscillation test
		if(calModetransition) {
			calModetransition = false;
			timer = 10.0f;
			angleVar1 = MotorAngleStateGetCurrentAccumulatedAnglef(&LocalMotorAngleState);
			oscilationRangeFiltered = 0.0f;
		}

		float posError = angleVar1 - MotorAngleStateGetCurrentAccumulatedAnglef(&LocalMotorAngleState);
		if(posError >= 0.0f && angleVar5 <= 0.0f) {
			angleVar2 = 0.0f;
			oscilationRangeFiltered += (angleVar4 - oscilationRangeFiltered)*0.1f;
		}
		else if(posError <= 0.0f && angleVar5 >= 0.0f) {
			angleVar3 = 0.0f;
			oscilationRangeFiltered += (angleVar4 - oscilationRangeFiltered)*0.1f;
		}

		if(posError > angleVar2) {
			angleVar2 = posError;
		}
		if(posError < angleVar3) {
			angleVar3 = posError;
		}
		angleVar4 = angleVar2 - angleVar3;

		angleVar5 = posError;
		driveTorque = -posError * 0.5f - LocalMotorAngleState.currentAngularVelFiltered * 0.1f;

		if(MathExtras::Abs(MotorAngleStateGetCurrentAccumulatedAnglef(&LocalMotorAngleState)) < 2.0f) {
			timer -= sampleTime;
			frictionCalFactor += (targetOscillationRange - oscilationRangeFiltered) * 0.001f;
			frictionCalFactor = MathExtras::ClampInclusive(frictionCalFactor, FRICTIONFACTOR_MIN, FRICTIONFACTOR_MAX);
		}
		else {
			frictionCalFactor = 0.0f;
		}
		if(timer <= 0.0f) {
			motorCalibrationMode = 1; // Start next iteration
			calModetransition = true;
		}
	}
	else {
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
		motorCalibrationMode=1;
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
	if(motorCalibrationMode > 0 && motorCalibrationMode < 4) {
//		printf("a: %f, md: %d, driveOffset: %f, range: %f, fcf: %f, dpf: %f, timer: %f\n",
//			MotorAngleStateGetCurrentAccumulatedAnglef(&LocalMotorAngleState),
//			calibrationMotorDirection,
//			driveOffset,
//			oscilationRangeFiltered,
//			frictionCalFactor,
//			drivePowerFactor,
//			timer);
	}
}

