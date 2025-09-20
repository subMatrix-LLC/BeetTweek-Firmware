/*
 * Mode_Indent1.h
 *
 *      Author: casht
 */

#ifndef INC_MODES_Mode_Clocks_H_
#define INC_MODES_Mode_Clocks_H_




#include "Mode.h"
#include "MathExtras_DSP.h"
#include "MathExtras_DSP_Generators.h"

class Mode_Clocks : public Mode {
public:
	Mode_Clocks();
	virtual ~Mode_Clocks() ;



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
		}

		int res = Mode::EEPromBytesUsed() + byteOffset;
		return res;
	}


	bool WriteEEPROMState(uint32_t &ee_address) override;
	bool ReadEEPROMState(uint32_t &ee_address) override;
	void OnSaveTimerTimeout() override;


	int marksPerTable = 4;
	float angFactor = 0.0f;
	float rawAngle = 0.0f;
	float noDeadAngle = 0.0f;

	float deltaErrorSig = 0.0f;
	float deltaErrorSigFiltered = 0.0f;
	bool isSyncedAB = false;

	float speedVariable = 0.0f;
	float timeAccumSyncError = 0.0f;
	float timeAccumBSyncError = 0.0f;
	float phaseErrorAB = 0.0f;
	float targetSpeed = 0.0f;
	float timeDelta = 0.0f;
	float timeDeltaB = 0.0f;
	float curSpeed = 0.0f;
	const float highSpeedRingSpeedThresh = 2.0f;
	bool highSpeedRingIndicatorFlag = false;
	MathExtras::Threshold_Trigger<float> triggers[4];
	MathExtras::TapTempo<float> clockBTracker;
	MathExtras::ClockOutGate<float, true, true, 0,  0> outputTimeGates[4];



	double timeAccumCur = 0.0;
	double timeAccumCurB = 0.0;

	const float directPlayAngle = -0.5f;
	bool inDirectPlay = false;
	float directPlayStartTime = 0.0f;
	float directPlayLastAngle = 0.0f;
	float errorAccum = 0.0f;
};




#endif /* INC_MODES_MODE_INDENT1_H_ */
