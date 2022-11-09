/*
 * Mode_Indent1.h
 *
 *      Author: casht
 */

#ifndef INC_MODES_MODE_CHORDS_H_
#define INC_MODES_MODE_CHORDS_H_




#include "Mode.h"
#include "MathExtras_DSP.h"
#include "MathExtras_DSP_Generators.h"

class Mode_Chords : public Mode {
public:
	Mode_Chords();
	virtual ~Mode_Chords() ;



	virtual void Initialize();
	//updates the front panel
	virtual void UpdateLEDS(float sampleTime);


	//Called Somewhat Fast.  Holds Knob Update Processing.
	virtual void KnobDSPFunction(float sampleTime);

	virtual void AudioDSPFunction(float sampleTime, int bufferSwap);

	//call to tell the mode to handle the button down.
	virtual void ButtonDown(int button);

	//call to tell the mode to handle the button up.
	virtual void ButtonUp(int button);


	virtual void MainThreadUpdateFunction(float sampleTime);



	virtual int EEPromBytesUsed()
	{
		return Mode::EEPromBytesUsed();
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
		case 3:
			byteOffset = 3*sizeof(float);
			break;
		case 4:
			byteOffset = 4*sizeof(float);
			break;
		case 5:
			byteOffset = 5*sizeof(float);
			break;
		case 6:
			byteOffset = 6*sizeof(float);
			break;
		case 7:
			byteOffset = 7*sizeof(float);
			break;

		}

		int res = Mode::EEPromBytesUsed() + byteOffset;
		return res;
	}


	bool WriteEEPROMState(uint32_t &ee_address)
	{
		bool success = Mode::WriteEEPROMState(ee_address);

		return success;
	}


	bool ReadEEPROMState(uint32_t &ee_address)
	{
		bool success = Mode::ReadEEPROMState(ee_address);

		return success;
	}

	int ChordOffset()
	{
		if(inputOutputDescriptors[1].curAugment == 1)
		{
			return (VoltsToMotorAngle(adcVolts[1], inputOutputDescriptors[1].CurAugment().signalType)+0.5)*((int)EuroRack::ChordType_NUMCHORDS);
		}
		else
			return 0;
	}


	float curNumDetents = static_cast<float>(EuroRack::ChordType_NUMCHORDS);
	float detentColors[EuroRack::ChordType_NUMCHORDS];
	int CurDetentIdx();
	float CurDetentIdxf();
	int lastDetentIdx = 0;
	int sampleLock = 0;
	MathExtras::OscillatorSystem<float> oscs[4];
};




#endif /* INC_MODES_MODE_INDENT1_H_ */
