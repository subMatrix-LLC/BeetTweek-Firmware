/*
 * Mode_Utility.cpp
 *
 *  Created on: Mar 12, 2021
 *      Author: casht
 */




#include "Modes/Mode_Utility.h"
#include "BlockProcessing.h"
#include "PCM3168.h"

// Add these at the top of the file, after the includes
static float frictionStep = 0.2f;  // Step size for friction
static float powerStep = 0.25f;    // Step size for power
static float currentFriction = -2.0f;
static float currentPower = 0.25f;

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




	if(subMode == 0)//white
	{
		for(int i = 0; i < 4; i++)
		{
			dacOuts[i] = adc0DMA[i];
		}
	}
	else if(subMode == 1)//cyan
	{

		BLOCK_PROCESS_ALL()
			dac0DMA[sampleIdxDMA] = -adc0DMA[sampleIdxDMA] ;
			dac1DMA[sampleIdxDMA] = -adc1DMA[sampleIdxDMA] ;
		}


	}
	else if(subMode == 2)//VOLT to VOLT Pass through. purple
	{
		for(int i = 0; i < 4; i++)
		{
			SetDacVolts(i, adcVolts[i]);
		}
	}
	else if(subMode == 3)//Drive DAC at 0. yellow
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
			timer = 1.0f;  // 1 second drive pulse
			driveTorque = 1.0f;
			
			// Set current parameters
			frictionCalFactor = currentFriction;
			drivePowerFactor = currentPower;
			
			hasStartedDeceleration = false;
			decelerationStartTime = 0.0f;
			decelerationStartSpeed = 0.0f;
			printf("Testing parameters - Friction: %f, Power: %f\n", currentFriction, currentPower);
		}

		// Track maximum speed during drive phase
		if(LocalMotorAngleState.currentAngularVelFiltered > forwardTestVel1) {
			forwardTestVel1 = LocalMotorAngleState.currentAngularVelFiltered;
		}

		// Start measuring deceleration when drive pulse ends
		if(timer <= 0.0f && !hasStartedDeceleration) {
			driveTorque = 0.0f;
			decelerationStartTime = 0.0f;
			decelerationStartSpeed = LocalMotorAngleState.currentAngularVelFiltered;
			hasStartedDeceleration = true;
			printf("Drive phase complete. Max speed: %f rev/s\n", forwardTestVel1);
			printf("Starting deceleration measurement...\n");
		}

		// Continue measuring deceleration
		if(hasStartedDeceleration) {
			decelerationStartTime += sampleTime;
			
			// Check if motor has stopped or timeout reached
			if(MathExtras::Abs(LocalMotorAngleState.currentAngularVelFiltered) < 0.01f || decelerationStartTime >= 5.0f) {
				printf("Motor stopped or timeout reached. Results:\n");
				printf("Deceleration time: %f seconds\n", decelerationStartTime);
				printf("Initial deceleration speed: %f rev/s\n", decelerationStartSpeed);
				printf("Deceleration rate: %f rev/s^2\n", decelerationStartSpeed / decelerationStartTime);
				
				// Force motor to stop completely
				frictionCalFactor = 0.0f;
				driveTorque = 0.0f;
				drivePowerFactor = 0.25f;
				
				// Wait for complete stop
				if(MathExtras::Abs(LocalMotorAngleState.currentAngularVelFiltered) < 0.001f) {
					// Update parameters for next iteration
					currentFriction += frictionStep;  // Increment friction
					if(currentFriction > 2.0f) {
						currentFriction = -2.0f;  // Reset friction
						currentPower += powerStep;    // Increment power
						if(currentPower > 1.0f) {
							printf("Parameter sweep complete!\n");
							motorCalibrationMode = 0;  // Exit calibration mode
							return;
						}
					}
					
					// Reset for next iteration
					calModetransition = true;
					forwardTestVel1 = 0.0f;
				}
			}
		}

		timer -= sampleTime;
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
		printf("Utility Mode Cycle...\n");

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
	else if(button == 4)
	{
		motorCalibrationMode = 1;
		calModetransition = true;
		forwardTestVel1 = 0.0f;
		printf("Starting motor calibration test\n");
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

