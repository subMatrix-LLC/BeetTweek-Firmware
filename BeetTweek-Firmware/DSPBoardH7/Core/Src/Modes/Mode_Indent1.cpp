/*
 * Mode_Indent1.cpp
 *
 *  Created on: Jan 9, 2021
 *      Author: casht
 */



#include "Modes/Mode_Indent1.h"

#include "clockinput.h"

extern ModeManager modeManager;

Mode_Indent1::Mode_Indent1() : Mode()
{
	SetBothNames("Mode_Indent1");
}


Mode_Indent1::~Mode_Indent1()
{

}

void Mode_Indent1::Initialize() {
	Mode::Initialize();

	inputOutputDescriptors[0].enabled = true;
	inputOutputDescriptors[0].augments[0].signalType = EuroRack::SignalType_VPerOct;

	inputOutputDescriptors[1].enabled = true;
	inputOutputDescriptors[1].augments[0].signalType = EuroRack::SignalType_GATE;
	inputOutputDescriptors[1].augments[0].baseColor = MathExtras::Color::YELLOW;
	inputOutputDescriptors[1].augments[0].gateTrigger = &captureGate;

	inputOutputDescriptors[4].enabled = true;
	inputOutputDescriptors[4].augments[0].signalType = EuroRack::SignalType_VPerOct;

	inputOutputDescriptors[5].enabled = true;
	inputOutputDescriptors[5].augments[0].baseColor = MathExtras::Color::GREEN;
	inputOutputDescriptors[5].augments[0].signalType = EuroRack::SignalType_VPerOct;



	inputVoltJmp.SetHalfGap(1.0f);

	captureGate.SetThreshold(ER_GATE_THRESH_VOLT);
	captureGate.SetThresholdHalfGap(ER_GATE_HALF_HIST_VOLT);
}


//call to tell the mode to handle the button down.
void Mode_Indent1::ButtonDown(int button)
{


}

//call to tell the mode to handle the button up.
void Mode_Indent1::ButtonUp(int button)
{

	if(button == 3 && modeManager.buttonStates[9])//-
	{
		numDetentsTarget-= 1.0f;
	}
	else if(button == 2 && modeManager.buttonStates[9])//+
	{
		numDetentsTarget+= 1.0f;

	}
	numDetentsTarget = MathExtras::ClampInclusive(int(numDetentsTarget), 1, 80);


	if(button == 9)
	{
		//toggle button tweek mode
		//KnobTweekMode_roundToInts = true;
		//ToggleKnobTweekMode(&numDetentsTarget, 80.0f, 1.0f, -10.0f);
	}
}



void Mode_Indent1::UpdateLEDS(float sampleTime)
{

	WS2812_SETALLLED(0,0,0);

	//negate because motor angles are counter clockwise and led clock angles are clockwise.
	//float angleTotal = -MotorAngleStateGetCurrentAccumulatedAnglef(&LocalMotorAngleState);


	LEDManager.SetLEDRingRangeLinear_Float(LEDPanelManager::RINGIDENTIFIER_OUTER, -LocalMotorAngleState.currentAngle + 0.5f/(curNumDetents*2.0f) - 1.0f/LED_NUM_LEDS_PER_RING, -LocalMotorAngleState.currentAngle - 0.5f/(curNumDetents*2.0f) - 1.5f/LED_NUM_LEDS_PER_RING,
			(MathExtras::Color*)&MathExtras::Color::BLUE,
			(MathExtras::Color*)&MathExtras::Color::BLUE,
			1.0f);

	for(int clockIndex = 0; clockIndex < LED_NUM_LEDS_PER_RING; clockIndex++)
	{
		if(curNumDetents*2 > LED_NUM_LEDS_PER_RING)
		{
			MathExtras::Color highDensityColor;
			float rampOff = ((curNumDetents)-LED_NUM_LEDS_PER_RING/8)/float(LED_NUM_LEDS_PER_RING);
			//rampOff = MathExtras::ClampInclusive(rampOff, 0.0f, 1.0f);

			highDensityColor.FromHSL( EuroRack::VOCT_TO_HUE(detentVolts[int(curNumDetents-(float(clockIndex)/LED_NUM_LEDS_PER_RING)*curNumDetents)]) + rampOff*0.5, 1.0f, 0.5f);


			LEDManager.SetLEDInnerRing_Int(clockIndex, highDensityColor.r_*LED_BASE_BRIGHTNESS_255, highDensityColor.g_*LED_BASE_BRIGHTNESS_255, highDensityColor.b_*LED_BASE_BRIGHTNESS_255);
		}
		else
		{
			float ledAngle = float(clockIndex)/(LED_NUM_LEDS_PER_RING);
			float torqueIndicator = arm_sin_f32(((ledAngle)*M_TWOPI) * curNumDetents - M_PI_2 ) + 1.0f ;


//			if(!KnobTweekMode &&  MathExtras::Abs(MathExtras::WrapMax(-KnobAngle_Latest - ledAngle + (1.0f/int(curNumDetents))*0.25f, 1.0f)) < 1.0f/int(curNumDetents*2))
//			{
//				LEDManager.SetLEDOuterRing_Int(clockIndex,  0, 0, LED_BASE_BRIGHTNESS_255);
//
//			}

			float fillFactor = (curNumDetents*2)/float(LED_NUM_LEDS_PER_RING);


			if(torqueIndicator >= 1.0f-fillFactor-0.01)
			{

				MathExtras::Color c;
				c.FromHSL( EuroRack::VOCT_TO_HUE(detentVolts[int(curNumDetents-(float(clockIndex)/LED_NUM_LEDS_PER_RING)*curNumDetents)]),1.0f,0.5f);

				LEDManager.SetLEDInnerRing_Int(clockIndex,  c.r_*LED_BASE_BRIGHTNESS_255, c.g_*LED_BASE_BRIGHTNESS_255, c.b_*LED_BASE_BRIGHTNESS_255);
			}
		}
	}


//	MathExtras::Color outputColor;
//	outputColor.FromHSL(EuroRack::VOLTS_TO_HUE(detentVolts[CurDetentIdx()]),1.0f,0.5f);
//	inputOutputDescriptors[4].augments[0].baseColor = outputColor;

	StandardPanelLEDDraw();


}

//Called Very Fast.  Holds DSP Code
void Mode_Indent1::AudioDSPFunction(float sampleTime, int bufferSwap)
{
	//Angle Value



	//volt level
	SetDacVolts(0, detentVolts[CurDetentIdx()]);

	//interpolated volt level (between indents)
	float vInterp = MathExtras::WrappedArrayInterp<float,float>(&detentVolts[0], curNumDetents, CurDetentIdxf()-0.5f);


	SetDacVolts(1, vInterp);

	inputVoltJmp.Process(adcVolts[0]);
	captureGate.Process(adcVolts[1]);


	if(inputVoltJmp.Jumped() || captureGate.Triggered())
	{
		sampleLock = 1;
	}
	if(lastDetentIdx != CurDetentIdx())//transition
	{
		sampleLock = 0;
		doEepromWrite = true;

	}


	if(sampleLock)
		detentVolts[CurDetentIdx()] = adcVolts[0];


	lastDetentIdx = CurDetentIdx();



	//SetDacVolts(1, arm_sin_f32((KnobAngle_Latest*M_TWOPI) * curNumDetents )*ER_VOLTAGE_OUT_MAX);

	//Snapped Angle Value
//	float snapped = float(int((1.0f-KnobAngle_Latest)*(curNumDetents+1.0f)))/curNumDetents;

	//SetDacVolts(1, (-snapped)*ER_VOLTAGE_OUT_MAX);



}


int Mode_Indent1::CurDetentIdx()
{
	return LocalMotorAngleState.currentAngle*curNumDetents;
}
float Mode_Indent1::CurDetentIdxf()
{
	return LocalMotorAngleState.currentAngle*curNumDetents;
}


void Mode_Indent1::KnobDSPFunction(float sampleTime)
{
	float w1 = 1.0f;



	//torque curve
	driveTorque = arm_sin_f32((LocalMotorAngleState.currentAngle*M_TWOPI) * curNumDetents )*w1 - LocalMotorAngleState.currentAngularVelFiltered*0.5f;//add drag.



	MasterProcess_Torque(sampleTime);

}



void Mode_Indent1::MainThreadUpdateFunction(float sampleTime)
{
	Mode::MainThreadUpdateFunction(sampleTime);

	//curNumDetents = MathExtras::ClampInclusive(int((1.0f-(adcIns[0]/float(0xFFFF)))*20), 1, 20);

	//write values to eeprom if needed
	if(doEepromWrite)
	{
	for(int i = 0; i < 8; i++)
	{
		if(eepromDetentVolts[i] != detentVolts[i])
		{
			ee24_write_float(modeManager.CurrentModeEEPromBase()+EEPromAddressOffsets(i), detentVolts[i], 1000);
			eepromDetentVolts[i] = detentVolts[i];
		}
	}
	doEepromWrite = false;
	}

}



