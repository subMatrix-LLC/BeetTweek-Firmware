#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include "main.h"
#include "MathExtras.h"
#include "MotorAngleState.h"



float AntiCoggLevel(float angle, int derivative);



typedef enum {
	MotorErrorState_Fault = 0,
	MotorErrorState_OK
} MotorErrorState;
typedef enum {
	MotorDriveWaveForm_Sinusoidal = 0,
	MotorDriveWaveForm_Trapezoidal,

} MotorDriveWaveForm;

//defines pins for a single motor device (DRV831*, etc)
typedef struct MotorDevice
{
	GPIO_TypeDef* enable1GPIOPort;  uint16_t enable1GPIOPin;
	GPIO_TypeDef* enable2GPIOPort;  uint16_t enable2GPIOPin;
	GPIO_TypeDef* enable3GPIOPort;  uint16_t enable3GPIOPin;

	GPIO_TypeDef* control1GPIOPort; uint16_t control1GPIOPin;
	GPIO_TypeDef* control2GPIOPort; uint16_t control2GPIOPin;
	GPIO_TypeDef* control3GPIOPort; uint16_t control3GPIOPin;

	TIM_HandleTypeDef* controlTimer; 								//address of timer used for driving control pins.


	GPIO_TypeDef* resetGPIOPort;    uint16_t resetGPIOPin;
	GPIO_TypeDef* sleepGPIOPort;    uint16_t sleepGPIOPin;
	float resetHoldTimeMS;	        								//how long to hold the reset line in order to fully reset the device.

	GPIO_TypeDef* faultGPIOPort;    uint16_t faultGPIOPin;


	int direction;													//spin direction (for wire flips) (-1,1)

	MotorAngleState angleState;										//Angle Tracking
	MotorErrorState chipState;										//Error State


	int numPoles;													//how many poles the stator has
	float energyBaseLimit;											//max percentage (PWM DUTY) to give the motor that corresponds to max drive.
} MotorDevice;




void MotorDeviceInit(MotorDevice* device,
				    GPIO_TypeDef* enable1GPIOPort, uint16_t enable1GPIOPin,
					GPIO_TypeDef* enable2GPIOPort,  uint16_t enable2GPIOPin,
					GPIO_TypeDef* enable3GPIOPort,  uint16_t enable3GPIOPin,

					GPIO_TypeDef* control1GPIOPort, uint16_t control1GPIOPin,
					GPIO_TypeDef* control2GPIOPort, uint16_t control2GPIOPin,
					GPIO_TypeDef* control3GPIOPort, uint16_t control3GPIOPin,

					TIM_HandleTypeDef* controlTimer,

					GPIO_TypeDef* resetGPIOPort,   uint16_t resetGPIOPin,
					GPIO_TypeDef* sleepGPIOPort,   uint16_t sleepGPIOPin,
					float resetHoldTimeMS,

					GPIO_TypeDef* faultGPIOPort,    uint16_t faultGPIOPin,
					int direction,
					int numPoles, float energyBaseLimit
);


MotorErrorState ReadMotorErrorState(MotorDevice* device);

void ResetMotorChip(MotorDevice* device);

void SetMotorPWMDuty(MotorDevice* device, int motorLine, float dutycycle);


void SetMotorEnable(MotorDevice* device, int motorLine, int enable);

void SetMotorPWMDrives(MotorDevice* device, MotorDriveWaveForm waveform, float driveStrength, float radians);

void SetMotorPWMDrivesSpaceVector(MotorDevice* device, float driveStrength, float radians);


float DetermineMechanicalRotorFluxAngleOffset(MotorDevice* device);


//returns a value from the given drive waveform on the given motor line and offset radians.  range from 0 to 1
float DriveWaveForm(MotorDriveWaveForm waveform, int motorLine, float phaseOffset);

//void ReadMotorCurrents(float* current1, float* current2, float* current3);

//Drives the motor
void DriveMotor(MotorDevice* device, float driveStrength, int numPoles, float phaseOffset);


void DriveMotorNoFeedback(MotorDevice* device, float driveStrength, float time);






void CalculateSpaceVectorModulationTimes(float rotorFluxAngle_Rad, float magnitude, float* T1, float* T2, float* T0, int* sector_out);

void CalculateCenterAlignedPWMDutyCycles(int sector, float T0, float T1, float T2, float* dutyU, float* dutyV, float* dutyW);


//sets the motor line outputs direct to the trapState immediately
void SetMotorTrapezoidalState(MotorDevice* device, int trapState);









void ForwardClark(float a, float b, float c, float* alpha, float* beta);

void ReverseClark(float alpha, float beta, float* a, float* b, float* c);

void ParkTransform(float rotorFluxAngle_Rad, float alpha, float beta, float* d, float* q);

void InvParkTransform(float rotorFluxAngle_Rad, float d, float q, float* alpha, float* beta);















#endif
