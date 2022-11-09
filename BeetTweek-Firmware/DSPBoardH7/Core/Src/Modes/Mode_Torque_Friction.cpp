/*
 * Mode_Torque1.cpp
 *
 *  Created on: Dec 15, 2020
 *      Author: casht
 */




#include <Modes/Mode_Torque_Friction.h>


Mode_TorqueFriction::Mode_TorqueFriction() : Mode()
{
	SetBothNames("Mode_TorqueFriction");

}
Mode_TorqueFriction::~Mode_TorqueFriction()
{

}

void Mode_TorqueFriction::Initialize() {
	Mode::Initialize();


	inputOutputDescriptors[0].enabled = true;
	inputOutputDescriptors[0].numAugments = 2;
	inputOutputDescriptors[0].augments[0].baseColor = MathExtras::Color::YELLOW;
	inputOutputDescriptors[0].augments[0].defaultVoltVal = 0.0f;
	inputOutputDescriptors[0].augments[1].baseColor = MathExtras::Color::YELLOW;
	inputOutputDescriptors[0].augments[1].defaultVoltVal = 0.0f;
	inputOutputDescriptors[0].augments[1].signalType = EuroRack::SignalType_SMSIGNAL_1;


	inputOutputDescriptors[1].enabled = true;
	inputOutputDescriptors[1].numAugments = 2;
	inputOutputDescriptors[1].prefferedAugment = 0;
	inputOutputDescriptors[1].augments[0].baseColor = MathExtras::Color::RED;
	inputOutputDescriptors[1].augments[0].defaultVoltVal = 0.0f;

	inputOutputDescriptors[1].augments[1].baseColor = MathExtras::Color::YELLOW;
	inputOutputDescriptors[1].augments[1].defaultVoltVal = 0.0f;



	inputOutputDescriptors[4].enabled = true;
	inputOutputDescriptors[4].augments[0].baseColor = MathExtras::Color::BLUE;

	inputOutputDescriptors[5].enabled = true;
	inputOutputDescriptors[5].augments[0].baseColor = MathExtras::Color::BLUE;

	inputOutputDescriptors[6].enabled = true;
	inputOutputDescriptors[6].augments[0].baseColor = MathExtras::Color::YELLOW;


	inputOutputDescriptors[7].enabled = true;
	inputOutputDescriptors[7].augments[0].baseColor = MathExtras::Color::MAGENTA;


}

//inline void Mode_Torque1::UnInitialize() {
//}



inline void Mode_TorqueFriction::UpdateLEDS(float sampleTime)
{
	WS2812_SETALLLED(0,0,0);

	//basic led scheme
	float angleLEDRing = -MotorAngleStateGetCurrentAccumulatedAnglef(&LocalMotorAngleState);






	if(curGestureLevel != GUESTURE_LEVEL_PLAY)//dont show yellow torque indicator when playing.
	{
		LEDManager.SetLEDRingRangeLinear_Float(LEDPanelManager::RINGIDENTIFIER_INNER, angleLEDRing-driveTrqMinInd*(1.0f/8.0f), angleLEDRing-driveTrqMaxInd*(1.0f/8.0f),
			(MathExtras::Color*)&MathExtras::Color::YELLOW,
			(MathExtras::Color*)&MathExtras::Color::YELLOW,
			1/4.0f);
	}

	LEDManager.SetLEDOuterRing_Int(0,  LED_BASE_BRIGHTNESS_255,LED_BASE_BRIGHTNESS_255,LED_BASE_BRIGHTNESS_255);

	LEDManager.SetLEDOuterRing_Float(angleLEDRing,  0,0,LED_BASE_BRIGHTNESS_255);


	StandardPanelLEDDraw();
}

inline void Mode_TorqueFriction::AudioDSPFunction(float sampleTime, int bufferSwap)
{


	//put angle on 1st output
	SetDacVolts(0, MotorAngleToVolts(MathExtras::WrapMinMax(LocalMotorAngleState.currentAngle,-0.5f,0.5f)*2.0f));


	//Sin and cos of current angle
	float theta = LocalMotorAngleState.currentAngle*M_TWO_PI;
	float curOutputSin = arm_sin_f32(theta);
	SetDacVolts(1, MotorAngleToVolts(curOutputSin));

	SetDacVolts(2, MotorAngleToVolts(driveTorque));

	SetDacVolts(3, -MotorAngleToVolts(LocalMotorAngleState.currentAngularVelFiltered*0.1f));

}

inline void Mode_TorqueFriction::KnobDSPFunction(float sampleTime)
{

	float adc0Term = adcVolts[0]/ER_CV_VOLTAGE_MAX;
	float adc1Term = adcVolts[1]/ER_CV_VOLTAGE_MAX;


	//filter adc terms
	float adc0Attenuation = 1.0f;
	if(inputOutputDescriptors[0].curAugment == 1) adc0Attenuation = 10.0f;

	adc0Filtered += ANTIALIAS_FIRSTORDER_FILT_COEFF*sampleTime*(adc0Term*adc0Attenuation - adc0Filtered);
	adc1Filtered += ANTIALIAS_FIRSTORDER_FILT_COEFF*sampleTime*(adc1Term - adc1Filtered);


	float primaryTorqueTerm =  adc0Filtered;
	if(inputOutputDescriptors[1].curAugment == 1)
		primaryTorqueTerm += adc1Filtered;

	primaryTorqueTerm = MathExtras::ClampInclusive(primaryTorqueTerm, -1.0f, 1.0f);


	float frictionTerm = MathExtras::Abs(adcVolts[1]/ER_CV_VOLTAGE_MAX);
	if(inputOutputDescriptors[1].curAugment == 0)
		frictionTerm = MathExtras::ClampInclusive(frictionTerm, 0.0f, 1.0f);
	else
		frictionTerm = 0.0f;

	driveTorque = primaryTorqueTerm*(1.0f - frictionTerm) - (LocalMotorAngleState.currentAngularVelFiltered/MAX_RPM_FULL_TORQUE)*frictionTerm*20.0f;


	//update min/max torque indicator for minimizing led aliasing
	driveTrqMaxInd += 20.0f*sampleTime*(MathExtras::Max(0.0f,driveTorque) - driveTrqMaxInd);
	driveTrqMinInd += 20.0f*sampleTime*(MathExtras::Min(0.0f,driveTorque) - driveTrqMinInd);



	MasterProcess_Torque(sampleTime);
}



//
//inline void Mode_Torque1::ButtonDown(int button) {
//}
//
//inline void Mode_Torque1::ButtonUp(int button) {
//}
//
//inline void Mode_Torque1::DebugPrint() {
//}
