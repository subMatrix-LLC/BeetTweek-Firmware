/*
 * Mode_Sequencer.h
 *
 *  Created on: Dec 14, 2020
 *      Author: casht
 */

#ifndef INC_MODES_MODE_SEQUENCER_H_
#define INC_MODES_MODE_SEQUENCER_H_

#include "Modes/Mode.h"

class Mode_Sequencer : public Mode{
public:
	Mode_Sequencer();
	virtual ~Mode_Sequencer();


	enum Scale {
		Scale_Chromatic = 0,
		Scale_Major     = 1,
		Scale_Minor     = 2,
		Scale_Pentatonic= 3,
	};



	//volts/oct for given scale root offset value
	float ScaleValue_To_VPO(Scale scale, int rootOffset)
	{
		if(scale == Scale_Chromatic)
		{
			return float(rootOffset/12.0f);
		}

		return float(rootOffset/12.0f);
	}

	void AdvanceSequence()
	{
		curSeqIdx += curSeqDirMult;

		if(curSeqIdx >= numSequences)
			curSeqIdx = 0;

		if(curSeqIdx < 0)
			curSeqIdx = numSequences;
	}


	static const int numSequences = 20;

	Scale curScale = Scale_Chromatic;


	int curSeqIdx = 0;
	int curSeqDirMult = 1;
	int seqLookup[numSequences] = {0};


	uint32_t clkPrevTime = 0;
	uint32_t clkDeltaTime = 0;
	float oldClkValue = 0.0f;

	//do setup things when the mode is entered
	//virtual void Initialize();

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


};


#endif /* INC_MODES_MODE_SEQUENCER_H_ */
