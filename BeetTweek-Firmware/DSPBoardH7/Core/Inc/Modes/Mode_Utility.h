/*
 * Mode_Utility.h
 *
 *  Created on: Mar 12, 2021
 *      Author: casht
 */

#ifndef INC_MODES_MODE_UTILITY_H_
#define INC_MODES_MODE_UTILITY_H_


#include "Modes/Mode.h"
#include "MathExtras_DSP_Generators.h"
class Mode_Utility : public Mode {
public:
	Mode_Utility();
	virtual ~Mode_Utility();

	//do setup things when the mode is entered
	virtual void Initialize();

	//un-setup thing in prep for next mode
	//virtual void UnInitialize();

	//updates the front panel
	virtual void UpdateLEDS(float sampleTime);

	//Called Very Fast.  Holds DSP Code
	virtual void AudioDSPFunction(float sampleTime,int bufferSwap);

	//Called Somewhat Fast.  Holds Knob Update Processing.
	virtual void KnobDSPFunction(float sampleTime);

	//call from main thread.
	//virtual void MainThreadUpdateFunction(){}

	//call to tell the mode to handle the button down.
	//virtual void ButtonDown(int button);

	//call to tell the mode to handle the button up.
	virtual void ButtonUp(int button);

	virtual void OnFuncCombo(int button);

	//Prints Debug Information
	virtual void DebugPrint();


	int subMode = 1;
	int motorCalibrationMode = 0;
	int calibrationMotorDirection = 0;
	float timer = 0.0f;
	float timer2 = 0.0f;

	bool calModetransition = false;
	float forwardTestVel1 = 0.0f;
	float angleVar1 = 0.0f;
	float angleVar2 = 0.0f;
	float angleVar3 = 0.0f;
	float angleVar4 = 0.0f;
	float angleVar5 = 0.0f;
	float oscilationRangeFiltered = 0.0f;

	MathExtras::OscillatorSystem<double> osc;


};



#endif /* INC_MODES_MODE_UTILITY_H_ */
