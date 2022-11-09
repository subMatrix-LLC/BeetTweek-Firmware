/*
 * Mode_Indent2.cpp
 *
 *  Created on: Dec 2, 2020
 *      Author: casht
 */




#include <MathExtras_Color.h>
#include "Modes/Mode_Indent2.h"


Mode_Indent2::Mode_Indent2() : Mode()
{
	SetBothNames("Mode_Indent2");

}
Mode_Indent2::~Mode_Indent2()
{

}

void Mode_Indent2::Initialize()
{



}
void Mode_Indent2::MainThreadUpdateFunction(float sampleTime)
{
	Mode::MainThreadUpdateFunction(sampleTime);

	//update dynamic indenting.


	//bring map values back to zero over time
	for(int i = 0; i < INDENT_MODE_INDENTMAP_SIZE; i++)
	{
		float curValue = GetIndentValue(i);
		float error = 0 - curValue;

		SetIndentValue(i, GetIndentValue(i) + error*0.01f);
	}


	int idx = int(LocalMotorAngleState.currentAngle*(INDENT_MODE_INDENTMAP_SIZE-1));


	int detentHalfSize = INDENT_MODE_INDENTMAP_SIZE/8;


	float indentSidePercentage = 0;
	float targetVal = detentFunction(indentSidePercentage*M_PI);


	float Kp = 0.02f;
	indentMap[wrapMap(idx)] += (targetVal - indentMap[wrapMap(idx)] )*Kp;

	for(int offset = 1; offset < detentHalfSize; offset++)
	{
		indentSidePercentage = float(offset)/detentHalfSize;
		targetVal = detentFunction(indentSidePercentage*M_PI);

		indentMap[wrapMap(idx-offset)] += (targetVal - indentMap[wrapMap(idx-offset)] )*Kp;
		indentMap[wrapMap(idx+offset)] += (targetVal - indentMap[wrapMap(idx+offset)] )*Kp;
	}





}

void Mode_Indent2::KnobDSPFunction(float sampleTime)
{

	float mapSlopeValue =  0.5f*GetIndentDerivativeApproximate(int(LocalMotorAngleState.currentAngle*INDENT_MODE_INDENTMAP_SIZE));

	driveTorque = mapSlopeValue;

	MasterProcess_Torque(sampleTime);

}


//Prints Debug Information
void Mode_Indent2::DebugPrint()
{
	//for(int i = 0; i < INDENT_MODE_INDENTMAP_SIZE; i++)
	//{
//		//printf("%f\r\n", indentMap[i]);
		//printf("%f\r\n", 0.5f*GetIndentDerivativeApproximate(i));

//	}


}

void Mode_Indent2::UpdateLEDS(float sampleTime)
{
	WS2812_SETALLLED(0,0,0);

	//basic led scheme
	float angleLEDRing = 1.0f-LocalMotorAngleState.currentAngle;


	for(int i = 0; i < LED_NUM_LEDS_PER_RING; i++)
	{
		int closestIndentIdx = (float(i)/LED_NUM_LEDS_PER_RING) * INDENT_MODE_INDENTMAP_SIZE;
		float val = indentMap[wrapMap(INDENT_MODE_INDENTMAP_SIZE-closestIndentIdx)];

		MathExtras::Color color;
		color.FromHSL(val, 1.0f, 0.1f);
		LEDManager.SetLEDInnerRing_Int(i,  val*255, 0,0);
	}




	LEDManager.SetLEDOuterRing_Float(angleLEDRing,  0, 0, LED_BASE_BRIGHTNESS_255);

	LEDManager.SetLEDOuterRing_Int(0,  LED_BASE_BRIGHTNESS_255, LED_BASE_BRIGHTNESS_255, LED_BASE_BRIGHTNESS_255);

}


