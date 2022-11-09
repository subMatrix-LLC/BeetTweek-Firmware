/*
 * Mode_IndentRouting.cpp
 *
 *  Created on: Dec 11, 2020
 *      Author: casht
 */




#include "Modes/Mode_IndentRouting.h"
#include "arm_math.h"
#include "MathExtras_Curves.h"

Mode_IndentRouting::Mode_IndentRouting() : Mode()
{

}
Mode_IndentRouting::~Mode_IndentRouting()
{

}

void Mode_IndentRouting::Initialize()
{
	Mode::Initialize();
}

void Mode_IndentRouting::UpdateLEDS(float sampleTime)
{


	if(firstLEDFrame){
		WS2812_SETALLLED(0,0,0);

		const float oneForth = 1.0f/4;
		const float oneEighth = 1.0f/8;

		LEDManager.SetLEDRingRangeLinear_Float(LEDPanelManager::RINGIDENTIFIER_INNER, 0 - oneEighth, oneForth - oneEighth,
				(MathExtras::Color*)&MathExtras::Color::RED,
				(MathExtras::Color*)&MathExtras::Color::RED,
				0.25f);

		LEDManager.SetLEDRingRangeLinear_Float(LEDPanelManager::RINGIDENTIFIER_INNER, oneForth - oneEighth, oneForth*2 - oneEighth,
				(MathExtras::Color*)&MathExtras::Color::GREEN,
				(MathExtras::Color*)&MathExtras::Color::GREEN,
				0.25f);

		LEDManager.SetLEDRingRangeLinear_Float(LEDPanelManager::RINGIDENTIFIER_INNER,  oneForth*2 - oneEighth, oneForth*3 - oneEighth,
				(MathExtras::Color*)&MathExtras::Color::BLUE,
				(MathExtras::Color*)&MathExtras::Color::BLUE,
				0.25f);


		LEDManager.SetLEDRingRangeLinear_Float(LEDPanelManager::RINGIDENTIFIER_INNER, oneForth*3 - oneEighth, oneForth*4 - oneEighth,
				(MathExtras::Color*)&MathExtras::Color::YELLOW,
				(MathExtras::Color*)&MathExtras::Color::YELLOW,
				0.25f);

		prevAngleLEDIdx = -9999;
		prevAngleLEDColor[0] = 0;
		prevAngleLEDColor[1] = 0;
		prevAngleLEDColor[2] = 0;

	}


	{
		int angleLEDRingIdx = -LocalMotorAngleState.currentAngle*LED_NUM_LEDS_PER_RING;

		//restore old led and draw new one.
		if(angleLEDRingIdx != prevAngleLEDIdx)
		{
			LEDManager.SetLEDOuterRing_Int(prevAngleLEDIdx, prevAngleLEDColor[0], prevAngleLEDColor[1],  prevAngleLEDColor[2]);

			LEDManager.GetLEDOuterRing_Int(angleLEDRingIdx, &prevAngleLEDColor[0], &prevAngleLEDColor[1], &prevAngleLEDColor[2]);
			LEDManager.SetLEDOuterRing_Int(angleLEDRingIdx,        0, 0,  LED_BASE_BRIGHTNESS_255);
		}
		prevAngleLEDIdx = angleLEDRingIdx;
	}



//	//inputs
//	DrawLEDButtonStandard(0, (MathExtras::Color*)&MathExtras::Color::RED, EuroRack::SignalType_CV5);
//	DrawLEDButtonStandard(1, (MathExtras::Color*)&MathExtras::Color::GREEN, EuroRack::SignalType_CV5);
//	DrawLEDButtonStandard(2, (MathExtras::Color*)&MathExtras::Color::BLUE, EuroRack::SignalType_CV5);
//	DrawLEDButtonStandard(3, (MathExtras::Color*)&MathExtras::Color::YELLOW, EuroRack::SignalType_CV5);
//
//
//	//outputs
//	DrawLEDButtonStandard(4, (MathExtras::Color*)&MathExtras::Color::RED, EuroRack::SignalType_CV5);
//	DrawLEDButtonStandard(5, (MathExtras::Color*)&MathExtras::Color::GREEN, EuroRack::SignalType_CV5);
//	DrawLEDButtonStandard(6, (MathExtras::Color*)&MathExtras::Color::BLUE, EuroRack::SignalType_CV5);
//	DrawLEDButtonStandard(7, (MathExtras::Color*)&MathExtras::Color::YELLOW, EuroRack::SignalType_CV5);

	firstLEDFrame = false;
}

void Mode_IndentRouting::KnobDSPFunction(float sampleTime)
{

	float w1 = 0.5f;
	float ws = 1 - w1;

	float accumulatedAngle = MotorAngleStateGetCurrentAccumulatedAnglef(&LocalMotorAngleState);
	float accumulatedAngleWrapped = MathExtras::WrapMinMax(accumulatedAngle, -0.5f, 0.5f);

	//snapping to 4 regions.
	driveTorque = arm_sin_f32((accumulatedAngleWrapped*M_TWOPI + M_PI_4) * 4 )*w1;

//	float sig1dist = MathExtras::ClampInclusive(1.0f - (float)MathExtras::Abs((float)KnobAngle_Latest - 0     + 0.125f), 0.0f, 1.0f);
//	float sig2dist = MathExtras::ClampInclusive(1.0f - (float)MathExtras::Abs((float)KnobAngle_Latest - 0.25f + 0.125f), 0.0f, 1.0f);
//	float sig3dist = MathExtras::ClampInclusive(1.0f - (float)MathExtras::Abs((float)KnobAngle_Latest - 0.5f  + 0.125f), 0.0f, 1.0f);
//	float sig4dist = MathExtras::ClampInclusive(1.0f - (float)MathExtras::Abs((float)KnobAngle_Latest - 0.75f + 0.125f), 0.0f, 1.0f);

	float sig1 = ((adcIns[0]/float(0xFFFF)) - 0.5f)*2.0f;
	float sig1Contribution = sig1*MathExtras::CosCurveFilterF(accumulatedAngleWrapped - 0.0f, 0.25f);

	float sig2 = ((adcIns[1]/float(0xFFFF)) - 0.5f)*2.0f;
	float sig2Contribution = sig2*MathExtras::CosCurveFilterF(accumulatedAngleWrapped - 0.25f, 0.25f);

	float sig3 = ((adcIns[2]/float(0xFFFF)) - 0.5f)*2.0f;
	float sig3Contribution = sig3*MathExtras::CosCurveFilterF(accumulatedAngleWrapped - 0.5f, 0.25f);

	float sig4 = ((adcIns[3]/float(0xFFFF)) - 0.5f)*2.0f;
	float sig4Contribution = sig4*MathExtras::CosCurveFilterF(accumulatedAngleWrapped - 0.75f, 0.25f);




	driveTorque += sig1Contribution*ws;
	driveTorque += sig2Contribution*ws;
	driveTorque += sig3Contribution*ws;
	driveTorque += sig4Contribution*ws;


	//torque from the signals added in
	//driveTorque = arm_sin_f32(KnobAngle_Latest*M_TWOPI * 4)*0.2f;



	driveTorque = MathExtras::ClampInclusive(driveTorque, -1.0f, 1.0f);


	MasterProcess_Torque(sampleTime);


}

void Mode_IndentRouting::AudioDSPFunction(float sampleTime, int bufferSwap)
{
	float accumulatedAngle = MotorAngleStateGetCurrentAccumulatedAnglef(&LocalMotorAngleState);
	float accumulatedAngleWrapped = MathExtras::WrapMinMax(accumulatedAngle, -0.5f, 0.5f);


	float sig1 = (adcIns[0]/float(0xFFFF));
	float sig1Contribution = sig1*MathExtras::CosCurveFilterF(accumulatedAngleWrapped - 0.0f, 0.25f);

	float sig2 = (adcIns[1]/float(0xFFFF));
	float sig2Contribution = sig2*MathExtras::CosCurveFilterF(accumulatedAngleWrapped - 0.25f, 0.25f);

	float sig3 = (adcIns[2]/float(0xFFFF));
	float sig3Contribution = sig3*MathExtras::CosCurveFilterF(accumulatedAngleWrapped - 0.5f, 0.25f);

	float sig4 = (adcIns[3]/float(0xFFFF));
	float sig4Contribution = sig4*MathExtras::CosCurveFilterF(accumulatedAngleWrapped - 0.75f, 0.25f);

	dacOuts[0] = sig1Contribution*0xFFFF;
	dacOuts[1] = sig2Contribution*0xFFFF;
	dacOuts[2] = sig3Contribution*0xFFFF;
	dacOuts[3] = sig4Contribution*0xFFFF;


}



