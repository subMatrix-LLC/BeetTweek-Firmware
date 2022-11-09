/*
 * Mode_Torque1.h
 *
 *  Created on: Dec 15, 2020
 *      Author: casht
 */

#ifndef INC_MODES_MODE_TORQUE_FRICTION_H_
#define INC_MODES_MODE_TORQUE_FRICTION_H_

#include "Modes/Mode.h"
#include "adcdacbuffers.h"
class Mode_TorqueFriction : public Mode {
public:
	Mode_TorqueFriction();
	virtual ~Mode_TorqueFriction();

	//do setup things when the mode is entered
	virtual void Initialize();

	//un-setup thing in prep for next mode
	//virtual void UnInitialize();

	//updates the front panel
	virtual void UpdateLEDS(float sampleTime);

	//Called Very Fast.  Holds DSP Code
	virtual void AudioDSPFunction(float sampleTime, int bufferSwap);

	//Called Somewhat Fast.  Holds Knob Update Processing.
	virtual void KnobDSPFunction(float sampleTime);

	//call from main thread.
	//virtual void MainThreadUpdateFunction(){}

	//call to tell the mode to handle the button down.
	//virtual void ButtonDown(int button);

	//call to tell the mode to handle the button up.
	//virtual void ButtonUp(int button);

	//Prints Debug Information
	//virtual void DebugPrint();

	float driveTrqMaxInd = 0.0f;
	float driveTrqMinInd = 0.0f;

	float adc0Filtered = 0.0f;
	float adc1Filtered = 0.0f;

};

#endif /* INC_MODES_MODE_TORQUE_FRICTION_H_ */
