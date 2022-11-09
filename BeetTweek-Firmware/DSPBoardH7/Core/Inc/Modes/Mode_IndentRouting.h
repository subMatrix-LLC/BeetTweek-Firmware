/*
 * Mode_IndentRouting.h
 *
 *  Created on: Dec 11, 2020
 *      Author: casht
 */

#ifndef INC_MODES_MODE_INDENTROUTING_H_
#define INC_MODES_MODE_INDENTROUTING_H_

#include "Modes/Mode.h"

//Routes the 4 inputs to outputs
class Mode_IndentRouting : public Mode {
public:
	Mode_IndentRouting();
	virtual ~Mode_IndentRouting();

	//do setup things when the mode is entered
	virtual void Initialize();

	//un-setup thing in prep for next mode
	virtual void UnInitialize(){}

	//updates the front panel
	virtual void UpdateLEDS(float sampleTime);

	//Called Very Fast.  Holds DSP Code
	virtual void AudioDSPFunction(float sampleTime, int bufferSwap);


	//Called Somewhat Fast.  Holds Knob Update Processing.
	virtual void KnobDSPFunction(float sampleTime);

	//call from main thread.
	virtual void MainThreadUpdateFunction(float sampleTime){	Mode::MainThreadUpdateFunction(sampleTime); }

	//call to tell the mode to handle the button down.
	virtual void ButtonDown(int button){}

	//call to tell the mode to handle the button up.
	virtual void ButtonUp(int button){}

	//Prints Debug Information
	virtual void DebugPrint(){Mode::DebugPrint();}


	int prevAngleLEDIdx = -9999;
	uint8_t prevAngleLEDColor[3];

};



#endif /* INC_MODES_MODE_INDENTROUTING_H_ */
