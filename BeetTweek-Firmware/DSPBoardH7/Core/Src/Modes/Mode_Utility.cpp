/*
 * Mode_Utility.cpp
 *
 *  Created on: Mar 12, 2021
 *      Author: casht
 */




#include "Modes/Mode_Utility.h"
#include "BlockProcessing.h"
#include "PCM3168.h"


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

	inputOutputDescriptors[2].funcCombo = true;



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




	if(subMode == 0)
	{
		for(int i = 0; i < 4; i++)
		{
			dacOuts[i] = adc0DMA[i];
		}
	}
	else if(subMode == 1)//
	{

		BLOCK_PROCESS_ALL()
			dac0DMA[sampleIdxDMA] = -adc0DMA[sampleIdxDMA] ;
			dac1DMA[sampleIdxDMA] = -adc1DMA[sampleIdxDMA] ;
		}


	}
	else if(subMode == 2)//VOLT to VOLT Pass through.
	{
		for(int i = 0; i < 4; i++)
		{
			SetDacVolts(i, adcVolts[i]);
		}
	}
	else if(subMode == 3)//Drive DAC at 0
	{



		for(int i = 0; i < 4; i++)
		{
			dacOuts[i] = VoltsToDac(i, 0.0f);
		}
	}
	else if(subMode == 4)
	{
		static float s = 0.0f;
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


//	driveTorque = (adcInsRaw[0]/float(0xFFFF)-0.5f)*2.0f;
//
//
	driveTorque = 0;
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
	else if(button == 2)
	{


		PCM3168_Toggle48DeEmph();

	}
}
//
//inline void Mode_Utility::DebugPrint() {
//}

