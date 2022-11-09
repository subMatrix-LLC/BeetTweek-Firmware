/*
 * Mode_GlobalAtten.h
 *
 *  Created on: Mar 29, 2021
 *      Author: casht
 */

#ifndef INC_MODES_MODE_KNOB1_H_
#define INC_MODES_MODE_KNOB1_H_


#include "Modes/Mode.h"
class Mode_Knob1 : public Mode {
public:
	Mode_Knob1();
	virtual ~Mode_Knob1();

	//do setup things when the mode is entered
	virtual void Initialize();

	//updates the front panel
	virtual void UpdateLEDS(float sampleTime);

	//Called Very Fast.  Holds DSP Code
	virtual void AudioDSPFunction(float sampleTime, int bufferSwap);

	//Called Somewhat Fast.  Holds Knob Update Processing.
	virtual void KnobDSPFunction(float sampleTime);


	float frictionFactor = 0.0f;
	float clampedAngle;
	bool isOutOfBounds = false;
	float boundsGap = 0.05f;
	float limits[2] = {-0.75*0.5f,0.75*0.5f};






//	bool KnobTweekMode = false;
//	float* KnobTweekParam = nullptr;
//	float KnobTweekParam_Start = 0.0f;
//	float KnobTweekMode_StartingAngle = 0.0f;
//	float KnobTweekMode_highLimit = 1.0f;
//	float KnobTweekMode_lowLimit = -1.0f;
//	float KnobTweekMode_scale = 1.0f;
//	float KnobTweekMode_limitForce = 0.0f;
//	bool KnobTweekMode_roundToInts = false;
//
//
//
//	MathExtras::Movement_Jump<float> KnobTweekModeHisteresis;
//
//	virtual void KnobTweekMode_UpdateLEDS(float sampleTime);
//	virtual void KnobTweekMode_KnobDSPFunction(float sampleTime);

//	virtual void EnterKnobTweekMode();
	//exit knobtweekmode, restore = true will original old values.
//	virtual void ExitKnobTweekMode(bool restore = false);
//	void ToggleKnobTweekMode(float* param, float highLimit, float lowLimit, float scale);

};


#endif /* INC_MODES_MODE_KNOB1_H_ */
