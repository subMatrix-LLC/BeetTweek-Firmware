/*
 * Mode_Chords.cpp
 *
 *  Created on: Aug 29, 2022
 *      Author: casht
 */

/*
 * Mode_Indent1.cpp
 *
 *  Created on: Jan 9, 2021
 *      Author: casht
 */


#include "Modes/Mode_Chords.h"

#include "clockinput.h"

extern ModeManager modeManager;

Mode_Chords::Mode_Chords() : Mode()
{
	SetBothNames("Mode_Chords");
}


Mode_Chords::~Mode_Chords()
{

}

void Mode_Chords::Initialize() {
	Mode::Initialize();

	inputOutputDescriptors[0].enabled = true;
	inputOutputDescriptors[0].augments[0].signalType = EuroRack::SignalType_VPerOct;
	inputOutputDescriptors[1].augments[0].baseColor = MathExtras::Color::RED;

	inputOutputDescriptors[1].enabled = true;
	inputOutputDescriptors[1].numAugments = 2;
	inputOutputDescriptors[1].augments[0].signalType = EuroRack::SignalType_CV;
	inputOutputDescriptors[1].augments[0].baseColor = MathExtras::Color::YELLOW;
	inputOutputDescriptors[1].augments[1].signalType = EuroRack::SignalType_CV;
	inputOutputDescriptors[1].augments[1].baseColor = MathExtras::Color::BLUE;


	inputOutputDescriptors[4].enabled = true;
	inputOutputDescriptors[4].numAugments = 2;
	inputOutputDescriptors[4].augments[0].signalType = EuroRack::SignalType_VPerOct;

	inputOutputDescriptors[4].augments[1].baseColor = MathExtras::Color::WHITE;
	inputOutputDescriptors[4].augments[1].signalType = EuroRack::SignalType_Audio;


	inputOutputDescriptors[5].enabled = true;
	inputOutputDescriptors[5].numAugments = 1;
	inputOutputDescriptors[5].augments[0].baseColor = MathExtras::Color::GREEN;
	inputOutputDescriptors[5].augments[0].signalType = EuroRack::SignalType_VPerOct;

	inputOutputDescriptors[5].augments[1].baseColor = MathExtras::Color::WHITE;
	inputOutputDescriptors[5].augments[1].signalType = EuroRack::SignalType_Audio;







	inputOutputDescriptors[6].enabled = true;
	inputOutputDescriptors[6].numAugments = 1;
	inputOutputDescriptors[6].augments[0].baseColor = MathExtras::Color::GREEN;
	inputOutputDescriptors[6].augments[0].signalType = EuroRack::SignalType_VPerOct;
	inputOutputDescriptors[6].augments[1].baseColor = MathExtras::Color::WHITE;
	inputOutputDescriptors[6].augments[1].signalType = EuroRack::SignalType_Audio;


	inputOutputDescriptors[7].enabled = true;
	inputOutputDescriptors[7].numAugments = 1;
	inputOutputDescriptors[7].augments[0].baseColor = MathExtras::Color::GREEN;
	inputOutputDescriptors[7].augments[0].signalType = EuroRack::SignalType_VPerOct;
	inputOutputDescriptors[7].augments[1].baseColor = MathExtras::Color::WHITE;
	inputOutputDescriptors[7].augments[1].signalType = EuroRack::SignalType_Audio;






//

//
//




	detentColors[EuroRack::ChordType_MajorSenventh] = MathExtras::Color::RED_HUE;
	detentColors[EuroRack::ChordType_MajorSixth] = MathExtras::Color::RED_HUE;
	detentColors[EuroRack::ChordType_Majortriad] =  MathExtras::Color::RED_HUE;

	detentColors[EuroRack::ChordType_MinorSeventh] = MathExtras::Color::GREEN_HUE;
	detentColors[EuroRack::ChordType_MinorSixth] = MathExtras::Color::GREEN_HUE;
	detentColors[EuroRack::ChordType_MinorTriad] =  MathExtras::Color::GREEN_HUE;

	detentColors[EuroRack::ChordType_AugmentedSeventh] = MathExtras::Color::BLUE_HUE;
	detentColors[EuroRack::ChordType_AugmentedTriad] = MathExtras::Color::BLUE_HUE;

	detentColors[EuroRack::ChordType_DiminishedSeventh] =  MathExtras::Color::YELLOW_HUE;
	detentColors[EuroRack::ChordType_DiminishedTriad] =MathExtras::Color::YELLOW_HUE;

	detentColors[EuroRack::ChordType_MinorMajorSeventh] = MathExtras::Color::ORANGE_HUE;


	detentColors[EuroRack::ChordType_DominantSeventh] =MathExtras::Color::MAGENTA_HUE;
	detentColors[EuroRack::ChordType_HalfDiminishedSeventh] =MathExtras::Color::MAGENTA_HUE;


//

}



//call to tell the mode to handle the button down.
void Mode_Chords::ButtonDown(int button)
{


}

//call to tell the mode to handle the button up.
void Mode_Chords::ButtonUp(int button)
{
}



void Mode_Chords::UpdateLEDS(float sampleTime)
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

			highDensityColor.FromHSL( EuroRack::VOCT_TO_HUE(detentColors[int(curNumDetents-(float(clockIndex)/LED_NUM_LEDS_PER_RING)*curNumDetents)]) + rampOff*0.5, 1.0f, 0.5f);


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
				c.FromHSL( EuroRack::VOCT_TO_HUE(detentColors[int(curNumDetents-(float(clockIndex)/LED_NUM_LEDS_PER_RING)*curNumDetents)]),1.0f,0.5f);

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
void Mode_Chords::AudioDSPFunction(float sampleTime, int bufferSwap)
{
	float rootFreq = EuroRack::VOCT_TO_FREQ(adcVolts[0]);
	int inversion = (VoltsToMotorAngle(adcVolts[1], inputOutputDescriptors[1].CurAugment().signalType)+0.5f)*4;

	int chord = MathExtras::WrapMax(CurDetentIdx() + ChordOffset(), (int)EuroRack::ChordType_NUMCHORDS);

	float freqs[4];
	if(inputOutputDescriptors[4].curAugment == 1)
		freqs[0] = EuroRack::CHORD_TO_FREQ(static_cast<EuroRack::ChordType>(chord), 0, inversion, rootFreq);
	if(inputOutputDescriptors[5].curAugment == 1)
		freqs[1] = EuroRack::CHORD_TO_FREQ(static_cast<EuroRack::ChordType>(chord), 1, inversion, rootFreq);
	if(inputOutputDescriptors[6].curAugment == 1)
		freqs[2] = EuroRack::CHORD_TO_FREQ(static_cast<EuroRack::ChordType>(chord), 2, inversion, rootFreq);
	if(inputOutputDescriptors[7].curAugment == 1)
		freqs[3] = EuroRack::CHORD_TO_FREQ(static_cast<EuroRack::ChordType>(chord), 3, inversion, rootFreq);


	const float atten = 0.2f;

	float ws[4];
	for(int i = 0; i < 4; i++)
	{
		ws[i] = M_TWO_PI*0.5*freqs[i];
	}

	BLOCK_PROCESS_CHANNELS_0_2()

		oscs[0].Process(ws[0],SAMPLETIME);
		oscs[1].Process(ws[1],SAMPLETIME);
//		oscs[2].Process(ws[2],SAMPLETIME);
//		oscs[3].Process(ws[3],SAMPLETIME);


		//float mix = rootFreq*(oscs[0].x/ws[0] + oscs[1].x/ws[1] + oscs[2].x/ws[2] + oscs[3].x/ws[3]);
		if(inputOutputDescriptors[4].curAugment == 1)
		{
			if(sampleIdxDMA%2 == 0)
				dac0DMA[sampleIdxDMA] = int32_t(oscs[0].x*atten*INT32_MAX) >> 8;
		}

	}


//	//output a chord
	if(inputOutputDescriptors[4].curAugment == 0)
		SetDacVolts(0, EuroRack::CHORD_TO_VOCT(static_cast<EuroRack::ChordType>(chord), 0, inversion, rootFreq) );
	if(inputOutputDescriptors[5].curAugment == 0)
		SetDacVolts(1, EuroRack::CHORD_TO_VOCT(static_cast<EuroRack::ChordType>(chord), 1, inversion, rootFreq) );
	if(inputOutputDescriptors[6].curAugment == 0)
		SetDacVolts(2, EuroRack::CHORD_TO_VOCT(static_cast<EuroRack::ChordType>(chord), 2, inversion, rootFreq) );
	if(inputOutputDescriptors[7].curAugment == 0)
		SetDacVolts(3, EuroRack::CHORD_TO_VOCT(static_cast<EuroRack::ChordType>(chord), 3, inversion, rootFreq) );


}


int Mode_Chords::CurDetentIdx()
{
	return LocalMotorAngleState.currentAngle*curNumDetents;
}


float Mode_Chords::CurDetentIdxf()
{
	return LocalMotorAngleState.currentAngle*curNumDetents;
}


void Mode_Chords::KnobDSPFunction(float sampleTime)
{
	float w1 = 1.0f;



	//torque curve
	driveTorque = arm_sin_f32((LocalMotorAngleState.currentAngle*M_TWOPI) * curNumDetents )*w1 - LocalMotorAngleState.currentAngularVelFiltered*0.5f;//add drag.



	MasterProcess_Torque(sampleTime);

}



void Mode_Chords::MainThreadUpdateFunction(float sampleTime)
{
	Mode::MainThreadUpdateFunction(sampleTime);

	channelInterpEnables[0] = static_cast<int>(inputOutputDescriptors[4].curAugment == 0);
	channelInterpEnables[1] = static_cast<int>(inputOutputDescriptors[5].curAugment == 0);
	channelInterpEnables[2] = static_cast<int>(inputOutputDescriptors[6].curAugment == 0);
	channelInterpEnables[3] = static_cast<int>(inputOutputDescriptors[7].curAugment == 0);
}



