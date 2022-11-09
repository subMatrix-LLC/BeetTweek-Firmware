/*
 * MotorControl.c
 *
 *  Created on: Dec 17, 2019
 *      Author: casht
 */

#include "MotorControl.h"

#include "main.h"
#include "math.h"
#include "us_delay.h"
#include "stdio.h"
#include "arm_math.h"

extern TIM_HandleTypeDef htim1;
extern SPI_HandleTypeDef hspi2;
//extern ADC_HandleTypeDef hadc1;





//General model Sin8:
//     f(x) =
//                    a1*sin(b1*x+c1) + a2*sin(b2*x+c2) + a3*sin(b3*x+c3) +
//                    a4*sin(b4*x+c4) + a5*sin(b5*x+c5) + a6*sin(b6*x+c6) +
//                    a7*sin(b7*x+c7) + a8*sin(b8*x+c8)
//Coefficients (with 95% confidence bounds):
//       a1 =       11.96  (11.83, 12.1)
//       b1 =      0.1384  (0.1384, 0.1385)
//       c1 =      -1.178  (-1.203, -1.154)
//       a2 =       9.256  (9.118, 9.393)
//       b2 =      0.1509  (0.1509, 0.151)
//       c2 =      0.2847  (0.2537, 0.3157)
//       a3 =       19.49  (-949.8, 988.7)
//       b3 =    0.005847  (-0.03608, 0.04778)
//       c3 =      -4.128  (-23.02, 14.76)
//       a4 =       163.6  (-3.54e+05, 3.543e+05)
//       b4 =    0.008491  (-0.3153, 0.3323)
//       c4 =      -2.091  (-123.2, 119)
//       a5 =       2.611  (2.472, 2.749)
//       b5 =     0.07549  (0.0753, 0.07567)
//       c5 =      -3.047  (-3.153, -2.942)
//       a6 =       147.5  (-3.55e+05, 3.553e+05)
//       b6 =    0.008752  (-0.2719, 0.2894)
//       c6 =      0.9549  (-100.7, 102.6)
//       a7 =        1.25  (1.113, 1.386)
//       b7 =      0.2078  (0.2075, 0.2082)
//       c7 =      -3.179  (-3.397, -2.961)
//       a8 =       1.027  (0.8879, 1.166)
//       b8 =      0.1246  (0.1241, 0.1251)
//       c8 =       1.734  (1.455, 2.014)
//
//Goodness of fit:
//  SSE: 2336
//  R-square: 0.9815
//  Adjusted R-square: 0.9811
//  RMSE: 1.547
//


float AntiCoggLevel(float angle, int derivative)
{


	 static const float        a1 =       11.96;//(11.83, 12.1)
	 static const float        b1 =      0.1384;//(0.1384, 0.1385)
	 static const float        c1 =      -1.178;//(-1.203, -1.154)
	 static const float        a2 =       9.256;//(9.118, 9.393)
	 static const float        b2 =      0.1509;//(0.1509, 0.151)
	 static const float        c2 =      0.2847;//(0.2537, 0.3157)
	 static const float        a3 =       19.49;//(-949.8, 988.7)
	 static const float        b3 =    0.005847;//(-0.03608, 0.04778)
	 static const float        c3 =      -4.128;//(-23.02, 14.76)
	 static const float        a4 =       163.6;//(-3.54e+05, 3.543e+05)
	 static const float        b4 =    0.008491;//(-0.3153, 0.3323)
	 static const float        c4 =      -2.091;//(-123.2, 119)           )
	 static const float        a5 =       2.611;//(2.472, 2.749)
	 static const float        b5 =     0.07549;//(0.0753, 0.07567)
	 static const float        c5 =      -3.047;//(-3.153, -2.942)
	 static const float        a6 =       147.5;//(-3.55e+05, 3.553e+05)
	 static const float        b6 =    0.008752;//(-0.2719, 0.2894)
	 static const float        c6 =      0.9549;//(-100.7, 102.6)
	 static const float        a7 =        1.25;//(1.113, 1.386)
	 static const float        b7 =      0.2078;//(0.2075, 0.2082)
	 static const float        c7 =      -3.179;//(-3.397, -2.961)
	 static const float        a8 =       1.027;//(0.8879, 1.166)
	 static const float        b8 =      0.1246;//(0.1241, 0.1251)
	 static const float        c8 =       1.734;//(1.455, 2.014)


	 if(derivative == 0){
		 return a1*sinf(b1*angle+c1) + a2*sinf(b2*angle+c2) + a3*sinf(b3*angle+c3)
		 + a4*sinf(b4*angle+c4) + a5*sinf(b5*angle+c5) + a6*sinf(b6*angle+c6)
		 + a7*sinf(b7*angle+c7) + a8*sinf(b8*angle+c8);
	 }
	 else
	 {
	     return a1*cosf(b1*angle+c1) + a2*cosf(b2*angle+c2) + a3*cosf(b3*angle+c3)
	     + a4*cosf(b4*angle+c4) + a5*cosf(b5*angle+c5) + a6*cosf(b6*angle+c6)
	     + a7*cosf(b7*angle+c7) + a8*cosf(b8*angle+c8);
	 }
}












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
)
{
	device->enable1GPIOPort = enable1GPIOPort;
	device->enable2GPIOPort = enable2GPIOPort;
	device->enable3GPIOPort = enable3GPIOPort;

	device->enable1GPIOPin = enable1GPIOPin;
	device->enable2GPIOPin = enable2GPIOPin;
	device->enable3GPIOPin = enable3GPIOPin;

	device->control1GPIOPort = control1GPIOPort;
	device->control2GPIOPort = control2GPIOPort;
	device->control3GPIOPort = control3GPIOPort;

	device->control1GPIOPin = control1GPIOPin;
	device->control2GPIOPin = control2GPIOPin;
	device->control3GPIOPin = control3GPIOPin;

	device->controlTimer = controlTimer;

	device->resetGPIOPort = resetGPIOPort;
	device->resetGPIOPin = resetGPIOPin;

	device->sleepGPIOPort = sleepGPIOPort;
	device->sleepGPIOPin = sleepGPIOPin;

	device->faultGPIOPort = faultGPIOPort;
	device->faultGPIOPin = faultGPIOPin;

	device->direction  = direction;

	device->resetHoldTimeMS = resetHoldTimeMS;

	device->numPoles = numPoles;
	device->energyBaseLimit = energyBaseLimit;

}


MotorErrorState ReadMotorErrorState(MotorDevice* device)
{
	GPIO_PinState state = HAL_GPIO_ReadPin(device->faultGPIOPort, device->faultGPIOPin);

	if(state == GPIO_PIN_RESET)
	{
		device->chipState = MotorErrorState_Fault;
		return MotorErrorState_Fault;
	}
	else
	{
		device->chipState = MotorErrorState_OK;
		return MotorErrorState_OK;
	}
}

void ResetMotorChip(MotorDevice* device)
{
	HAL_GPIO_WritePin(device->resetGPIOPort, device->resetGPIOPin, GPIO_PIN_RESET);
	HAL_Delay(device->resetHoldTimeMS);
	HAL_GPIO_WritePin(device->resetGPIOPort, device->resetGPIOPin, GPIO_PIN_SET);
}





void SetMotorPWMDuty(MotorDevice* device, int motorLine, float dutycycle)
{
	uint16_t ARR = __HAL_TIM_GET_AUTORELOAD(device->controlTimer);
	uint16_t compare = dutycycle*device->energyBaseLimit*(ARR+1);

	if(compare >= ARR)
		compare = ARR;

	if(motorLine == 1)
	{
		__HAL_TIM_SET_COMPARE(device->controlTimer, TIM_CHANNEL_1, compare);
	}
	else if(motorLine == 2)
	{
		__HAL_TIM_SET_COMPARE(device->controlTimer, TIM_CHANNEL_2, compare);
	}
	else if(motorLine == 3)
	{
		__HAL_TIM_SET_COMPARE(device->controlTimer, TIM_CHANNEL_3, compare);
	}
}



void SetMotorEnable(MotorDevice* device, int motorLine, int enable)
{
	if(motorLine == 1){
		HAL_GPIO_WritePin(DRV8313_EN1_GPIO_Port, DRV8313_EN1_Pin, GPIO_PinState(enable));
	}
	else if(motorLine == 2) {
		HAL_GPIO_WritePin(DRV8313_EN2_GPIO_Port, DRV8313_EN2_Pin, GPIO_PinState(enable));
	}
	else if(motorLine == 3) {
		HAL_GPIO_WritePin(DRV8313_EN3_GPIO_Port, DRV8313_EN3_Pin, GPIO_PinState(enable));
	}
}

void SetMotorPWMDrives(MotorDevice* device, MotorDriveWaveForm waveform, float driveStrength, float radians)
{
	 //driveStrength = MathExtras::ClampInclusive(driveStrength, -1.0f, 1.0f);

	 SetMotorPWMDuty(device, 1, (DriveWaveForm(waveform, 1, radians))*driveStrength);
	 SetMotorPWMDuty(device, 2, (DriveWaveForm(waveform, 2, radians))*driveStrength);
	 SetMotorPWMDuty(device, 3, (DriveWaveForm(waveform, 3, radians))*driveStrength);
}

void SetMotorPWMDrivesSpaceVector(MotorDevice* device, float driveStrength, float radians)
{
	 driveStrength = MathExtras::ClampInclusive(driveStrength, -1.0f, 1.0f);


	 int sector;
	 float T1, T2, T0;

	 CalculateSpaceVectorModulationTimes(radians, driveStrength, &T0,  &T1, &T2, &sector);

	 float dutyU, dutyV, dutyW;
	 CalculateCenterAlignedPWMDutyCycles( sector,  T0,  T1,  T2, &dutyU, &dutyV, &dutyW );


	 //printf("%f,%f,%f \r\n", dutyU,dutyV,dutyW);


	 SetMotorPWMDuty(device, 1, dutyU); //purple
	 SetMotorPWMDuty(device, 2, dutyV); //yellow
	 SetMotorPWMDuty(device, 3, dutyW);//blue
}


float DetermineMechanicalRotorFluxAngleOffset(MotorDevice* device)
{

	 SetMotorPWMDrives(device, MotorDriveWaveForm_Sinusoidal, 1.0f, 0.0f);

	 HAL_Delay(1000);

	 return fmod(MotorAngleStateGetCurrentAnglef(&device->angleState) * M_TWOPI, (1.0f/device->numPoles));
}




float DriveWaveForm(MotorDriveWaveForm waveform, int motorLine, float phaseOffset)
{
	float phaseLineOffset = 0.0f;
	if(motorLine == 2)
		phaseLineOffset = 2.094f;
	if(motorLine == 1)
		phaseLineOffset = -2.094;

	float sinLevel = arm_sin_f32(phaseOffset + phaseLineOffset);

	//if(waveform == MotorDriveWaveForm_Sinusoidal){
	return sinLevel*0.5f + 0.5f;
	//}

}

void DriveMotor(MotorDevice* device, float driveStrength, int numPoles, float phaseOffset)
{

  //get current mechanical angle
  float currentMecahnicalAngle = MotorAngleStateGetCurrentAnglef(&device->angleState);


  //convert the mechanical angle into phase for 3phase waveform (Ideally matched 1:1)...
  float currentPhase = currentMecahnicalAngle*numPoles*M_2_PI;


  SetMotorPWMDrives(device, MotorDriveWaveForm_Sinusoidal, driveStrength, currentPhase + phaseOffset);
}


void DriveMotorNoFeedback(MotorDevice* device, float driveStrength, float time)
{
	SetMotorPWMDrives(device, MotorDriveWaveForm_Sinusoidal, driveStrength, time);
}











void CalculateSpaceVectorModulationTimes(float rotorFluxAngle_Rad, float magnitude, float* T0, float* T1, float* T2, int* sector_out)
{

	*T1 = 1.0f * magnitude * sin(M_PI_OVER_3 - rotorFluxAngle_Rad);
	*T2 = 1.0f * magnitude * sin(rotorFluxAngle_Rad);
	*T0 = 1.0f - *T1 - *T2;


	if(rotorFluxAngle_Rad >= 0.0f && (rotorFluxAngle_Rad < M_PI_OVER_3))//sector 1
	{
		//*state1_out = 1;//v1
		//*state2_out = 3;//v3
		*sector_out = 1;
	}
	else if((rotorFluxAngle_Rad >= M_PI_OVER_3 ) && (rotorFluxAngle_Rad < 2.0f * M_PI_OVER_3))//sector 2
	{
		//*state1_out = 3;//v3
		//*state2_out = 2;//v2
		*sector_out = 2;
	}
	else if((rotorFluxAngle_Rad >=  2.0f * M_PI_OVER_3 ) && (rotorFluxAngle_Rad < 3.0f * M_PI_OVER_3))//sector 3
	{
		//*state1_out = 2;//v2
		//*state2_out = 6;//v6
		*sector_out = 3;
	}
	else if((rotorFluxAngle_Rad >= 3.0f * M_PI_OVER_3) && ( rotorFluxAngle_Rad < 4.0f * M_PI_OVER_3))//sector 4
	{
		//*state1_out = 6;//v6
		//*state2_out = 4;//v4
		*sector_out = 4;
	}
	else if((rotorFluxAngle_Rad >=  4.0f * M_PI_OVER_3 ) && (rotorFluxAngle_Rad < 5.0f * M_PI_OVER_3))//sector 5
	{
		//*state1_out = 4;//v4
		//*state2_out = 5;//v5

		*sector_out = 5;
	}
	else if((rotorFluxAngle_Rad >= 5.0f * M_PI_OVER_3) && (rotorFluxAngle_Rad < 6.0f * M_PI_OVER_3))//sector 6
	{
		//*state1_out = 5;//v3
		//*state2_out = 1;//v2

		*sector_out = 6;
	}


}

void CalculateCenterAlignedPWMDutyCycles(int sector, float T0, float T1, float T2, float* dutyU, float* dutyV, float* dutyW)
{


	//null V0
//	switch(sector)
//	{
//	case 1:
//		*dutyU = T1 + T2;
//		*dutyV = T2;
//		*dutyW = 0;
//		break;
//
//	case 2:
//		*dutyU = T1;
//		*dutyV = T1 + T2 ;
//		*dutyW = 0;
//		break;
//
//	case 3:
//		*dutyU =0;
//		*dutyV = T1 + T2 ;
//		*dutyW = T2 ;
//		break;
//
//	case 4:
//		*dutyU = 0;
//		*dutyV = T1;
//		*dutyW = T1 + T2 ;
//		break;
//
//	case 5:
//		*dutyU = T2;
//		*dutyV = 0;
//		*dutyW = T1 + T2;
//		break;
//
//	case 6:
//		*dutyU = T1 + T2;
//		*dutyV = 0;
//		*dutyW = T1;
//		break;
//	default:
//		Error_Handler();
//		break;
//	}

	//alt rev sequence TODO Something not yet right...
	switch(sector)
	{
	case 1:
		*dutyU = T1 + T2 + 0.5f*T0;
		*dutyV = T2 + 0.5f*T0;
		*dutyW = 0.5f*T0;
		break;

	case 2:
		*dutyU = T1 + 0.5f*T0;
		*dutyV = T1 + T2 + 0.5f*T0;
		*dutyW = 0.5f*T0;
		break;

	case 3:
		*dutyU = 0.5f*T0;
		*dutyV = T1 + T2 + 0.5f*T0;
		*dutyW = T2 + 0.5f*T0;
		break;

	case 4:
		*dutyU = 0.5f*T0;
		*dutyV = T1 + 0.5f*T0;
		*dutyW = T1 + T2 + 0.5f*T0;
		break;

	case 5:
		*dutyU = T2 + 0.5f*T0;
		*dutyV = 0.5f*T0;
		*dutyW = T1 + T2 + 0.5f*T0;
		break;

	case 6:
		*dutyU = T1 + T2 + 0.5f*T0;
		*dutyV = 0.5f*T0;
		*dutyW = T1 + 0.5f*T0;
		break;
	default:
		Error_Handler();
		break;
	}
}


void SetMotorTrapezoidalState(MotorDevice* device, int trapState)
{
	  switch (trapState)
	  {

	  case 0:
		  //HAL_GPIO_WritePin(DRV8313_EN1_GPIO_Port, DRV8313_EN1_Pin, GPIO_PIN_RESET);
		  //HAL_GPIO_WritePin(DRV8313_EN2_GPIO_Port, DRV8313_EN2_Pin, GPIO_PIN_SET);
		  //HAL_GPIO_WritePin(DRV8313_EN3_GPIO_Port, DRV8313_EN3_Pin, GPIO_PIN_SET);

		  //HAL_GPIO_WritePin(DRV8313_B_IN1_GPIO_Port, DRV8313_B_IN1_Pin, GPIO_PIN_RESET);//X
		  HAL_GPIO_WritePin(device->control2GPIOPort, device->control2GPIOPin, GPIO_PIN_SET);
		  HAL_GPIO_WritePin(device->control3GPIOPort, device->control3GPIOPin, GPIO_PIN_RESET);


		  break;
	  case 1:
		  //HAL_GPIO_WritePin(DRV8313_EN1_GPIO_Port, DRV8313_EN1_Pin, GPIO_PIN_SET);
		  //HAL_GPIO_WritePin(DRV8313_EN2_GPIO_Port, DRV8313_EN2_Pin, GPIO_PIN_RESET);
		  //HAL_GPIO_WritePin(DRV8313_EN3_GPIO_Port, DRV8313_EN3_Pin, GPIO_PIN_SET);

		  HAL_GPIO_WritePin(device->control1GPIOPort, device->control1GPIOPin, GPIO_PIN_SET);
		  //HAL_GPIO_WritePin(DRV8313_B_IN2_GPIO_Port, DRV8313_B_IN2_Pin, GPIO_PIN_SET);//X
		  HAL_GPIO_WritePin(device->control3GPIOPort, device->control3GPIOPin, GPIO_PIN_RESET);


	      break;
	  case 2:
		  //HAL_GPIO_WritePin(DRV8313_EN1_GPIO_Port, DRV8313_EN1_Pin, GPIO_PIN_SET);
		 // HAL_GPIO_WritePin(DRV8313_EN2_GPIO_Port, DRV8313_EN2_Pin, GPIO_PIN_SET);
		  //HAL_GPIO_WritePin(DRV8313_EN3_GPIO_Port, DRV8313_EN3_Pin, GPIO_PIN_RESET);

		  HAL_GPIO_WritePin(device->control1GPIOPort, device->control1GPIOPin, GPIO_PIN_SET);
		  HAL_GPIO_WritePin(device->control2GPIOPort, device->control2GPIOPin, GPIO_PIN_RESET);
		  //HAL_GPIO_WritePin(DRV8313_B_IN3_GPIO_Port, DRV8313_B_IN3_Pin, GPIO_PIN_RESET);//X

	  	  break;
	  case 3:
		  //HAL_GPIO_WritePin(DRV8313_EN1_GPIO_Port, DRV8313_EN1_Pin, GPIO_PIN_RESET);
		  //HAL_GPIO_WritePin(DRV8313_EN2_GPIO_Port, DRV8313_EN2_Pin, GPIO_PIN_SET);
		  //HAL_GPIO_WritePin(DRV8313_EN3_GPIO_Port, DRV8313_EN3_Pin, GPIO_PIN_SET);

		  //HAL_GPIO_WritePin(DRV8313_B_IN1_GPIO_Port, DRV8313_B_IN1_Pin, GPIO_PIN_SET);//X
		  HAL_GPIO_WritePin(device->control2GPIOPort, device->control2GPIOPin, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(device->control3GPIOPort, device->control3GPIOPin, GPIO_PIN_SET);
	  		  break;
	  case 4:
		  //HAL_GPIO_WritePin(DRV8313_EN1_GPIO_Port, DRV8313_EN1_Pin, GPIO_PIN_SET);
		  //HAL_GPIO_WritePin(DRV8313_EN2_GPIO_Port, DRV8313_EN2_Pin, GPIO_PIN_RESET);
		  //HAL_GPIO_WritePin(DRV8313_EN3_GPIO_Port, DRV8313_EN3_Pin, GPIO_PIN_SET);

		  HAL_GPIO_WritePin(device->control1GPIOPort, device->control1GPIOPin, GPIO_PIN_RESET);
		  //HAL_GPIO_WritePin(DRV8313_B_IN2_GPIO_Port, DRV8313_B_IN2_Pin, GPIO_PIN_RESET);//X
		  HAL_GPIO_WritePin(device->control3GPIOPort, device->control3GPIOPin, GPIO_PIN_SET);
	  		  break;
	  case 5:
		 // HAL_GPIO_WritePin(DRV8313_EN1_GPIO_Port, DRV8313_EN1_Pin, GPIO_PIN_SET);
		  //HAL_GPIO_WritePin(DRV8313_EN2_GPIO_Port, DRV8313_EN2_Pin, GPIO_PIN_SET);
		  //HAL_GPIO_WritePin(DRV8313_EN3_GPIO_Port, DRV8313_EN3_Pin, GPIO_PIN_RESET);

		  HAL_GPIO_WritePin(device->control1GPIOPort, device->control1GPIOPin, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(device->control2GPIOPort, device->control2GPIOPin, GPIO_PIN_SET);
		  //HAL_GPIO_WritePin(DRV8313_B_IN3_GPIO_Port, DRV8313_B_IN3_Pin, GPIO_PIN_SET);//X
	  		  break;

	  default:
		  break;
	  }



}








/*

void ReadMotorCurrents(float* i_a, float* i_b, float* i_c)
{
	  HAL_ADC_Start(&hadc1);
	  HAL_ADC_PollForConversion(&hadc1, 1);
	  *i_a = HAL_ADC_GetValue(&hadc1)/4095.0f;

	  HAL_ADC_Start(&hadc1);
	  HAL_ADC_PollForConversion(&hadc1, 1);
	  *i_c = HAL_ADC_GetValue(&hadc1)/4095.0f;

	  HAL_ADC_Start(&hadc1);
	  HAL_ADC_PollForConversion(&hadc1, 1);
	  *i_b = HAL_ADC_GetValue(&hadc1)/4095.0f;
}


*/

void ForwardClark(float a, float b, float c, float* alpha, float* beta)
{
	*alpha = (3.0f/2.0f)*a;
	*beta  = (b - c)*SQRT_3_OVER_2;
}

void ReverseClark(float alpha, float beta, float* a, float* b, float* c)
{
	*a = (2.0f/3.0f)*alpha;
	*b = -(1/3.0f)*alpha + (1/SQRT_3)*beta;
	*c = -(1/3.0f)*alpha - (1/SQRT_3)*beta;
}

void ParkTransform(float rotorFluxAngle_Rad, float alpha, float beta, float* d, float* q)
{
	float sin_fluxAng = sinf(rotorFluxAngle_Rad);
	float cos_fluxAng = cosf(rotorFluxAngle_Rad);

	*d = alpha*cos_fluxAng + beta*sin_fluxAng;
	*q = -alpha*sin_fluxAng + beta*cos_fluxAng;
}

void InvParkTransform(float rotorFluxAngle_Rad, float d, float q, float* alpha, float* beta)
{
	float sin_fluxAng = sinf(rotorFluxAngle_Rad);
	float cos_fluxAng = cosf(rotorFluxAngle_Rad);

	*alpha = d*cos_fluxAng - q*sin_fluxAng;
	*beta = d*sin_fluxAng + q*cos_fluxAng;
}
















