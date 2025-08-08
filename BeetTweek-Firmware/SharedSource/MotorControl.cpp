#include "MotorControl.h"
#include "main.h"
#include "math.h"
#include "us_delay.h"
#include "stdio.h"
#include "arm_math.h"

float AntiCoggLevel(float angle, int derivative)
{
     static const float a1 = 11.96f;
     static const float b1 = 0.1384f;
     static const float c1 = -1.178f;
     static const float a2 = 9.256f;
     static const float b2 = 0.1509f;
     static const float c2 = 0.2847f;
     static const float a3 = 19.49f;
     static const float b3 = 0.005847f;
     static const float c3 = -4.128f;
     static const float a4 = 163.6f;
     static const float b4 = 0.008491f;
     static const float c4 = -2.091f;
     static const float a5 = 2.611f;
     static const float b5 = 0.07549f;
     static const float c5 = -3.047f;
     static const float a6 = 147.5f;
     static const float b6 = 0.008752f;
     static const float c6 = 0.9549f;
     static const float a7 = 1.25f;
     static const float b7 = 0.2078f;
     static const float c7 = -3.179f;
     static const float a8 = 1.027f;
     static const float b8 = 0.1246f;
     static const float c8 = 1.734f;

     if(derivative == 0){
         return a1*sinf(b1*angle+c1) + a2*sinf(b2*angle+c2) + a3*sinf(b3*angle+c3)
              + a4*sinf(b4*angle+c4) + a5*sinf(b5*angle+c5) + a6*sinf(b6*angle+c6)
              + a7*sinf(b7*angle+c7) + a8*sinf(b8*angle+c8);
     } else {
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
    unsigned int timerChannelA,
    unsigned int timerChannelB,
    unsigned int timerChannelC,

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
    device->timerChannelA = timerChannelA;
    device->timerChannelB = timerChannelB;
    device->timerChannelC = timerChannelC;

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
        __HAL_TIM_SET_COMPARE(device->controlTimer, device->timerChannelA, compare);
    }
    else if(motorLine == 2)
    {
        __HAL_TIM_SET_COMPARE(device->controlTimer, device->timerChannelB, compare);
    }
    else if(motorLine == 3)
    {
        __HAL_TIM_SET_COMPARE(device->controlTimer, device->timerChannelC, compare);
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
    SetMotorPWMDuty(device, 1, (DriveWaveForm(waveform, 1, radians))*driveStrength);
    SetMotorPWMDuty(device, 2, (DriveWaveForm(waveform, 2, radians))*driveStrength);
    SetMotorPWMDuty(device, 3, (DriveWaveForm(waveform, 3, radians))*driveStrength);
}

void SetMotorPWMDrivesSpaceVector_ALTREV(MotorDevice* device, float driveStrength, float radians)
{
    int sector;
    float T1, T2, T0;
    CalculateSpaceVectorModulationTimes(radians, driveStrength, &T0,  &T1, &T2, &sector);
    float dutyU, dutyV, dutyW;
    CalculateCenterAlignedPWMDutyCycles_ALTREV( sector,  T0,  T1,  T2, &dutyU, &dutyV, &dutyW );
    SetMotorPWMDuty(device, 1, dutyU);
    SetMotorPWMDuty(device, 2, dutyV);
    SetMotorPWMDuty(device, 3, dutyW);
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
        phaseLineOffset = -2.094f;
    float sinLevel = arm_sin_f32(phaseOffset + phaseLineOffset);
    return sinLevel*0.5f + 0.5f;
}

void DriveMotor(MotorDevice* device, float driveStrength, int numPoles, float phaseOffset)
{
    float currentMecahnicalAngle = MotorAngleStateGetCurrentAnglef(&device->angleState);
    float currentPhase = currentMecahnicalAngle*numPoles*M_2_PI;
    SetMotorPWMDrives(device, MotorDriveWaveForm_Sinusoidal, driveStrength, currentPhase + phaseOffset);
}

void DriveMotorNoFeedback(MotorDevice* device, float driveStrength, float time)
{
    SetMotorPWMDrives(device, MotorDriveWaveForm_Sinusoidal, driveStrength, time);
}

void CalculateSpaceVectorModulationTimes(float rotorFluxAngle_Rad, float magnitude, float* T0, float* T1, float* T2, int* sector_out)
{
    float localSectorAngle = MathExtras::WrapMax(rotorFluxAngle_Rad, M_PI_OVER_3);
    *T1 = 1.0f * magnitude * arm_sin_f32(M_PI_OVER_3 - localSectorAngle);
    *T2 = 1.0f * magnitude * arm_sin_f32(localSectorAngle);
    *T0 = 1.0f - *T1 - *T2;
    *sector_out = int(rotorFluxAngle_Rad/M_PI_OVER_3);
}

void CalculateCenterAlignedPWMDutyCycles_ALTREV(int sector, float T0, float T1, float T2, float* dutyU, float* dutyV, float* dutyW)
{
    switch(sector)
    {
    case 0:
        *dutyU = T1 + T2 + 0.5f*T0;
        *dutyV = T2 + 0.5f*T0;
        *dutyW = 0.5f*T0;
        break;
    case 1:
        *dutyU = T1 + 0.5f*T0;
        *dutyV = T1 + T2 + 0.5f*T0;
        *dutyW = 0.5f*T0;
        break;
    case 2:
        *dutyU = 0.5f*T0;
        *dutyV = T1 + T2 + 0.5f*T0;
        *dutyW = T2 + 0.5f*T0;
        break;
    case 3:
        *dutyU = 0.5f*T0;
        *dutyV = T1 + 0.5f*T0;
        *dutyW = T1 + T2 + 0.5f*T0;
        break;
    case 4:
        *dutyU = T2 + 0.5f*T0;
        *dutyV = 0.5f*T0;
        *dutyW = T1 + T2 + 0.5f*T0;
        break;
    case 5:
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
        HAL_GPIO_WritePin(device->control2GPIOPort, device->control2GPIOPin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(device->control3GPIOPort, device->control3GPIOPin, GPIO_PIN_RESET);
        break;
    case 1:
        HAL_GPIO_WritePin(device->control1GPIOPort, device->control1GPIOPin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(device->control3GPIOPort, device->control3GPIOPin, GPIO_PIN_RESET);
        break;
    case 2:
        HAL_GPIO_WritePin(device->control1GPIOPort, device->control1GPIOPin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(device->control2GPIOPort, device->control2GPIOPin, GPIO_PIN_RESET);
        break;
    case 3:
        HAL_GPIO_WritePin(device->control2GPIOPort, device->control2GPIOPin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(device->control3GPIOPort, device->control3GPIOPin, GPIO_PIN_SET);
        break;
    case 4:
        HAL_GPIO_WritePin(device->control1GPIOPort, device->control1GPIOPin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(device->control3GPIOPort, device->control3GPIOPin, GPIO_PIN_SET);
        break;
    case 5:
        HAL_GPIO_WritePin(device->control1GPIOPort, device->control1GPIOPin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(device->control2GPIOPort, device->control2GPIOPin, GPIO_PIN_SET);
        break;
    default:
        break;
    }
}

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