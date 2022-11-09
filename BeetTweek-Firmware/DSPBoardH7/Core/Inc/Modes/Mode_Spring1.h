/*
 * Mode_Spring1.h
 *
 *  Created on: Dec 2, 2020
 *      Author: casht
 */

#ifndef INC_MODES_MODE_SPRING1_H_
#define INC_MODES_MODE_SPRING1_H_


#include "Mode.h"

class Mode_Spring1 : public Mode {
public:
	Mode_Spring1()
	{
		SetBothNames("Mode_Spring1");

	}
	virtual ~Mode_Spring1();

	//do setup things when the mode is entered
	virtual void Initialize();

	//un-setup thing in prep for next mode
	virtual void UnInitialize();


	//updates the front panel
	virtual void UpdateLEDS(float sampleTime);

	//Called Very Fast.  Holds DSP Code
	virtual void AudioDSPFunction(float sampleTime, int bufferSwap);

	//Called Somewhat Fast.  Holds Knob Update Processing.
	virtual void KnobDSPFunction(float sampleTime);




	//call to tell the mode to handle the button down.
	virtual void ButtonDown(int button);

	//call to tell the mode to handle the button up.
	virtual void ButtonUp(int button);

	virtual void OnFuncCombo(int button);

	//Prints Debug Information
	virtual void DebugPrint();

	virtual void OnPostCycleAugment(int buttonIdx, int augmentIdx);



	virtual bool WriteEEPROMState(uint32_t &ee_address);

	virtual bool ReadEEPROMState(uint32_t &ee_address);

	virtual int EEPromBytesUsed()
	{
		return Mode::EEPromBytesUsed() + 3*sizeof(float);
	}
	virtual int EEPromAddressOffsets(int offset)
	{
		int byteOffset = 0;
		switch(offset)
		{
		case 0:
			byteOffset = 0;
			break;
		case 1:
			byteOffset = sizeof(float);
			break;
		case 2:
			byteOffset = 2*sizeof(float);
			break;
		}

		int res = Mode::EEPromBytesUsed() + byteOffset;
		return res;
	}





	MathExtras::Threshold_Trigger<float> Gate;

	float integralAccumulator = 0.0f;

	float rotaryScaler = 1.0f;
	float targetAngle = 0.0f;
	float motor_trgfiltered_angleError = 0.0f;
	float targetAngFiltered = 0.0f;
	float snapFilteredMotorAngle = 0.0f;
	float snapFilteredMotorAngleError = 0.0f;

	float timeAccum = 0.0f;
	float timeAccum2 = 0.0f;
	float timeAccum1LEDCapped = 0.0f;
	float timeAccum2LEDCapped = 0.0f;


	int timeAccum2Revs = 0;
	int timeAccum2LEDCappedRevs = 0;

	float curOsc1OutputSin = 0.0f;
	float curOsc1OutputCos = 0.0f;
	float curLFO1OutputFreq = 0.0f;
	float theta1 = 0.0f;

	float curOsc1OutputSinLEDCapped = 0.0f;
	float curOsc1OutputCosLEDCapped = 0.0f;
	float curLFO1OutputFreqLEDCapped = 0.0f;



	float curOsc2OutputSin = 0.0f;
	float curOsc2OutputCos = 0.0f;
	float curLFO2OutputFreq = 0.0f;
	float curLFO2OutputFreq_1 = 0.0f;
	float curLFO2OutputFreq_saved = 0.0f;
	bool saveLFO2 = false;
	float theta2 = 0.0f;

	float curOsc2OutputSinLEDCapped = 0.0f;
	float curOsc2OutputCosLEDCapped = 0.0f;
	float curLFO2OutputFreqLEDCapped = 0.0f;


	int clockRingLedPrev = 0;


	float curOsc2OutputCos_1 = 0.0f;
	int curOsc2OutputCosDir = 0;
	int curOsc2OutputCosSync = 0;
	float LEDOsc2OutputCosLastSync = 0.0f;
	float LEDOsc2OutputSinLastSync = 0.0f;
	float LEDOsc2OutputCosLastDiff = 0.0f;
	float LEDOsc2OutputLastSyncClkAng = 0.0f;
	float LEDOsc2OutputLastClkAng = 0.0f;



	float curOsc1OutputCos_1 = 0.0f;
	int   curOsc1OutputCosDir = 0;
	int   curOsc1OutputCosSync = 0;
	float LEDOsc1OutputCosLastSync = 0.0f;
	float LEDOsc1OutputSinLastSync = 0.0f;
	float LEDOsc1OutputCosLastDiff = 0.0f;
	float LEDOsc1OutputLastSyncClkAng = 0.0f;
	float LEDOsc1OutputLastClkAng = 0.0f;


	float driveTrqMaxInd = 0.0f;
	float driveTrqMinInd = 0.0f;

	void DrawErrorRing(float targetAngleLEDRing, float angleTotalLED);

};





#endif /* INC_MODES_MODE_SPRING1_H_ */
