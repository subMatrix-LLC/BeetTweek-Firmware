/*
 * Mode_Sequencer.cpp
 *
 *  Created on: Dec 14, 2020
 *      Author: casht
 */
#include "Modes/Mode_Sequencer.h"

Mode_Sequencer::Mode_Sequencer() : Mode()
{


	//initialize the sequencer with increasing values
	for(int i = 0; i < numSequences; i++)
	{
		seqLookup[i] = i;
	}

}


Mode_Sequencer::~Mode_Sequencer()
{

}


void Mode_Sequencer::UpdateLEDS(float sampleTime)
{

	WS2812_SETALLLED(0,0,0);

	//negate because motor angles are counter clockwise and led clock angles are clockwise.
	//float angleTotal = -MotorAngleStateGetCurrentAccumulatedAnglef(&LocalMotorAngleState);


	for(int i = 0; i < numSequences; i++)
	{
		MathExtras::Color checkBoardColor;


		if(i%2)
		{
			checkBoardColor = MathExtras::Color::RED;
		}
		else
		{
			checkBoardColor = MathExtras::Color::MAGENTA;
		}

		if(i == curSeqIdx)
			checkBoardColor = checkBoardColor * 2;


		LEDManager.SetLEDRingRangeLinear_Float(LEDPanelManager::RINGIDENTIFIER_INNER, (i/float(numSequences)),
				(i/float(numSequences)) + (1/float(numSequences)),
				(MathExtras::Color*)&checkBoardColor,
				(MathExtras::Color*)&checkBoardColor,
				0.25f);


		if(i == curSeqIdx)
		{
			LEDManager.SetLEDRingRangeLinear_Float(LEDPanelManager::RINGIDENTIFIER_OUTER, (i/float(numSequences)),
					(i/float(numSequences)) + (1/float(numSequences)),
					(MathExtras::Color*)&MathExtras::Color::BLUE,
					(MathExtras::Color*)&MathExtras::Color::BLUE,
					0.25f);
		}


	}







	//LEDManager.SetLEDInnerRing(targetAngleLEDRingIdx,  0, 32, 0);





	StandardPanelLEDDraw();


}

//Called Very Fast.  Holds DSP Code
void Mode_Sequencer::AudioDSPFunction(float sampleTime, int bufferSwap)
{


	dacOuts[0] = VoltsToDac(0,ScaleValue_To_VPO(Scale_Chromatic, seqLookup[curSeqIdx]))*0xFFFF;


}


//Called Somewhat Fast.  Holds Knob Update Processing.
void Mode_Sequencer::KnobDSPFunction(float sampleTime)
{
	uint32_t tick = HAL_GetTick();

	//CLK input updating
	float newClkValue = adcIns[0]/float(0xFFFF);
	if(newClkValue > 0.5f && oldClkValue <= 0.5f)
	{
		AdvanceSequence();

		clkDeltaTime = tick - clkPrevTime;
		clkPrevTime = tick;
	}
	oldClkValue = newClkValue;


	driveTorque = (0.0f - MotorAngleStateGetCurrentAccumulatedAnglef(&LocalMotorAngleState))*10.0f;

	driveTorque = MathExtras::ClampInclusive(driveTorque, -1.0f, 1.0f);


	if(driveTorque > 0.5f)
		curSeqDirMult = driveTorque*2;
	else if(driveTorque < -0.5f)
		curSeqDirMult = driveTorque*2;

	MasterProcess_Torque(sampleTime);

}
