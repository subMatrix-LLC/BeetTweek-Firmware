/*
 * Mode_GlobalAtten.cpp
 *
 *  Created on: Mar 29, 2021
 *      Author: casht
 */




#include "Modes/Mode_Knob1.h"




Mode_Knob1::Mode_Knob1() : Mode()
{
	SetBothNames("Mode_Knob1");
	strcpy(modeDescription,"A Mode For altering A Value");
	isHidden = true;
}
Mode_Knob1::~Mode_Knob1()
{

}

void Mode_Knob1::Initialize() {
	Mode::Initialize();



		inputOutputDescriptors[4].enabled = true;
		inputOutputDescriptors[4].augments[0].baseColor = MathExtras::Color::BLUE;
		inputOutputDescriptors[4].numAugments = 2;


}

//inline void Mode_Torque1::UnInitialize() {
//}

inline void Mode_Knob1::UpdateLEDS(float sampleTime) {

	WS2812_SETALLLED(0,0,0);

	//basic led scheme
	float angleLEDRing = -clampedAngle;


	LEDManager.SetLEDRingRangeLinear_Float(LEDPanelManager::RINGIDENTIFIER_INNER, limits[0], limits[1],
		(MathExtras::Color*)&MathExtras::Color::RED,
		(MathExtras::Color*)&MathExtras::Color::RED,
		1/4.0f);


	float angleOrigin = limits[0];
	if(inputOutputDescriptors[4].curAugment == 1)
		angleOrigin = 0.0f;

	LEDManager.SetLEDRingRangeLinear_Float(LEDPanelManager::RINGIDENTIFIER_OUTER, angleOrigin, angleLEDRing,
		(MathExtras::Color*)&MathExtras::Color::BLUE,
		(MathExtras::Color*)&MathExtras::Color::BLUE,
		1/4.0f);

	LEDManager.SetLEDOuterRing_Float(angleLEDRing,  0,0,LED_BASE_BRIGHTNESS_255);

	//marks
	const float inc = (limits[1] - limits[0])/6.0;
	for(float tick = limits[0]; tick <= limits[1]; tick += inc)
	{
		LEDManager.SetLEDOuterRing_Float(tick,  LED_BASE_BRIGHTNESS_255,LED_BASE_BRIGHTNESS_255,LED_BASE_BRIGHTNESS_255);
	}


	if(isOutOfBounds)
	{
		LEDManager.SetKnobCenterLEDS(LED_BASE_BRIGHTNESS_255, 0, 0);

	}


	StandardPanelLEDDraw();
}

inline void Mode_Knob1::AudioDSPFunction(float sampleTime, int bufferSwap) {

	float angle = MotorAngleStateGetCurrentAccumulatedAnglef(&LocalMotorAngleState);
	if(inputOutputDescriptors[4].curAugment == 0)
		SetDacVolts(0, ((limits[0] - angle)/(limits[1] - limits[0]))*ER_CV_VOLTAGE_MAX + ER_CV_VOLTAGE_MAX);
	else if(inputOutputDescriptors[4].curAugment == 1)
		SetDacVolts(0, -(angle/(limits[1] - limits[0]))*ER_CV_VOLTAGE_MAX*2);


}

inline void Mode_Knob1::KnobDSPFunction(float sampleTime)
{
	driveTorque = MathExtras::ClampInclusive(-LocalMotorAngleState.currentAngularVelFiltered*frictionFactor, -1.0f, 1.0f);

	float angle = MotorAngleStateGetCurrentAccumulatedAnglef(&LocalMotorAngleState);
	if(angle > limits[1])
	{
		driveTorque = -MathExtras::LinearRamp_2(angle - limits[1], 0.01f);
		isOutOfBounds = true;
	}
	else if(angle < limits[0])
	{
		driveTorque = MathExtras::LinearRamp_2(limits[0] - angle,0.01f);
		isOutOfBounds = true;
	}
	else
	{
		isOutOfBounds = false;

	}


	clampedAngle = MathExtras::ClampInclusive(MotorAngleStateGetCurrentAccumulatedAnglef(&LocalMotorAngleState), limits[0], limits[1]);


	MasterProcess_Torque(sampleTime);
}


































//
//
//void Mode::KnobTweekMode_UpdateLEDS(float sampleTime)
//{
//	static int timeAccum = 0;
//	timeAccum += 1;
//
//
//	if(MathExtras::Abs(KnobTweekMode_limitForce) > 0.0f)
//	{
//		LEDManager.SetKnobCenterLEDS(64, 0, 0);
//	}
//	else
//	{
//		float blinkVal = (arm_sin_f32(0.01f*timeAccum*4)+1.0f)*0.5f;
//		LEDManager.SetKnobCenterLEDS(blinkVal*64, blinkVal*64, blinkVal*64);
//	}
//}
//void Mode::KnobTweekMode_KnobDSPFunction(float sampleTime)
//{
//
//
//
//	KnobTweekModeHisteresis.SetHalfGap(0.01f);
//
//
//	*KnobTweekParam = (KnobTweekModeHisteresis.Process(MotorAngleStateGetCurrentAccumulatedAnglef(&motorAngleState) - KnobTweekMode_StartingAngle))*KnobTweekMode_scale + KnobTweekParam_Start;
//
//
//	//some friction while the value is dialed in.
//
//
//	if(*KnobTweekParam < KnobTweekMode_lowLimit)
//	{
//		KnobTweekMode_limitForce = -(KnobTweekMode_lowLimit - *KnobTweekParam)*100.0f;
//	}
//	else if(*KnobTweekParam > KnobTweekMode_highLimit)
//	{
//		KnobTweekMode_limitForce = (*KnobTweekParam - KnobTweekMode_highLimit)*100.0f;
//	}
//	else
//	{
//		KnobTweekMode_limitForce = 0.0f;
//	}
//	KnobTweekMode_limitForce = MathExtras::ClampInclusive(KnobTweekMode_limitForce, -1.0f,1.0f);
//
//	driveTorque =  KnobTweekMode_limitForce + -motorAngleState.currentAngularVelFiltered*0.5f*(1.0f-MathExtras::Abs(KnobTweekMode_limitForce));
//
//
//
//	*KnobTweekParam = MathExtras::ClampInclusive(*KnobTweekParam, KnobTweekMode_lowLimit, KnobTweekMode_highLimit);
//
//	if(KnobTweekMode_roundToInts)
//		*KnobTweekParam = MathExtras::RoundToInt(*KnobTweekParam);
//
//
//
//	MasterProcess_Torque(sampleTime);
//}
//
//void Mode::EnterKnobTweekMode()
//{
//	KnobTweekMode = true;
//	KnobTweekMode_StartingAngle = MotorAngleStateGetCurrentAccumulatedAnglef(&motorAngleState);
//}
//
//
//void Mode::ExitKnobTweekMode(bool restore)
//{
//	if(restore)
//	{
//		*KnobTweekParam = KnobTweekParam_Start;
//	}
//	else
//	{
//		KnobTweekParam_Start = *KnobTweekParam;//commit the values
//	}
//	KnobTweekMode = false;
//}
//
//void Mode::ToggleKnobTweekMode(float* param, float highLimit, float lowLimit, float scale)
//{
//
//	if(KnobTweekMode)
//	{
//		ExitKnobTweekMode();
//
//	}
//	else
//	{
//		KnobTweekMode_lowLimit = lowLimit;
//		KnobTweekMode_highLimit = highLimit;
//		KnobTweekMode_scale = scale;
//		KnobTweekParam = param;
//		KnobTweekParam_Start = *param;
//
//		EnterKnobTweekMode();
//	}
//}



