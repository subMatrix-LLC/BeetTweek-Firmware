/*
 * Mode_Indent1.h
 *
 *  Created on: Jan 9, 2021
 *      Author: casht
 */

#ifndef INC_MODES_MODE_INDENT1_H_
#define INC_MODES_MODE_INDENT1_H_




#include "Mode.h"
#include "MathExtras_DSP.h"

class Mode_Indent1 : public Mode {
public:
	Mode_Indent1();
	virtual ~Mode_Indent1() ;



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
		return Mode::EEPromBytesUsed() + 8*sizeof(float);
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

		for(int i = 0; i < 8; i++)
		{
			ee24_write_float(ee_address, detentVolts[i], 1000);
			ee_address += sizeof(float);
		}

		return success;
	}


	bool ReadEEPROMState(uint32_t &ee_address)
	{
		bool success = Mode::ReadEEPROMState(ee_address);

		for(int i = 0; i < 8; i++)
		{
			success &= ee24_read_float(ee_address, (float*)&detentVolts[i], 1000);
			eepromDetentVolts[i] = detentVolts[i];
			ee_address += sizeof(float);
		}
		return success;
	}


	float curNumDetents = 8.0f;
	float numDetentsTarget = 20.0f;

	float eepromDetentVolts[8];
	float detentVolts[8];

	bool doEepromWrite = false;

	MathExtras::Movement_Jump<float> inputVoltJmp;
	MathExtras::Threshold_Trigger<float> captureGate;

	int CurDetentIdx();
	float CurDetentIdxf();
	int lastDetentIdx = 0;
	int sampleLock = 0;

};




#endif /* INC_MODES_MODE_INDENT1_H_ */
