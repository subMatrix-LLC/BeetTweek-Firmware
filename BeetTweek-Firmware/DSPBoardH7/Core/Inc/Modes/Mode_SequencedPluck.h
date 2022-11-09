/*
 * Mode_SequencedPluck.h
 *
 *  Created on: Apr 17, 2021
 *      Author: casht
 */

#ifndef INC_MODES_MODE_SEQUENCEDPLUCK_H_
#define INC_MODES_MODE_SEQUENCEDPLUCK_H_
#include "Modes/Mode.h"

extern ModeManager modeManager;

#define MODE_SP_MAX_PLUCKNOTES (16)
class Mode_SequencedPluck : public Mode
{
public:
	Mode_SequencedPluck()
	{
		SetBothNames("Mode_SequencedPluck");
	}
	virtual ~Mode_SequencedPluck();


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

	virtual void OnFuncCombo(int button);

	virtual void OnButtonQuickAction(int button);

	virtual bool WriteEEPROMState(uint32_t &ee_address);

	virtual bool ReadEEPROMState(uint32_t &ee_address);

	virtual int EEPromBytesUsed()
	{
		return Mode::EEPromBytesUsed() + 2*sizeof(uint32_t) + 1*sizeof(uint8_t) + sizeof(uint32_t) + MODE_SP_MAX_PLUCKNOTES*sizeof(float);
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
			byteOffset = sizeof(uint32_t);
			break;
		case 2:
			byteOffset = 2*sizeof(uint32_t);
			break;
		case 3:
			byteOffset = 2*sizeof(uint32_t) + sizeof(uint8_t);
			break;
		case 4:
			byteOffset = 2*sizeof(uint32_t) + sizeof(uint8_t) + sizeof(uint32_t);
			break;
		}

		int res = Mode::EEPromBytesUsed() + byteOffset;
		return res;
	}


	//called when gesture record starts.
	virtual void OnGestureRecordStart(){
		Mode::OnGestureRecordStart();
		angleOffset_gesture = angleOffset;
	}

	//called when gesture record ends.
	virtual void OnGestureRecordStop(){
		Mode::OnGestureRecordStop();

	}
	//called when gesture playback loops
	virtual void OnGestureLoop(){
		Mode::OnGestureLoop();
		rstPluck = true;
		angleOffset = angleOffset_gesture;
	}

	//called when gesture playback starts
	virtual void OnGesturePlay(){
		Mode::OnGesturePlay();
		rstPluck = true;
		angleOffset = angleOffset_gesture;
	}

	virtual void OnSaveTimerTimeout()
	{
		uint32_t noteDataAddr =modeManager.CurrentModeEEPromBase() + EEPromAddressOffsets(4);
		WriteNoteData(noteDataAddr);
	}

	bool WriteNoteData(uint32_t &ee_address);
	bool ReadNoteData(uint32_t &ee_address);



//	//call from main thread.
//	virtual void MainThreadUpdateFunction(){}
//
//	//call to tell the mode to handle the button down.
//	virtual void ButtonDown(int button);
//
//	//call to tell the mode to handle the button up.
//	virtual void ButtonUp(int button);
//
//	//Prints Debug Information
	virtual void DebugPrint();

	class PluckNote {
	public:
		float volts = 0.0f;
		float anglePos = 0.0f;
		float intensity = 1.0f;
	};

	double accumAngle = 0.0;
	int strumDirection = 1;
	bool doublePluckEnable = true;
	int plkPoint = 0;
	int lastPluckPoint = -1;
	int lastHitPluckPoint = -1;
	float lastDriveTorque = 0.0f;
	float lastRampV = 0.0f;
	float lastSurfaceFallLocation = 0.0f;

	bool rstPluck = false;
	double angleOffset_gesture = 0.0f;

	int triggerGateSeq = 0;
	int outGateVal = 0;
	float pluckVoltage = 0.0f;
	float pluckIntensity = 0.0f;
	double angleOffset = 0.0f;
	float shiftDirMult = 0.0f;
	int shiftDirCycle = 0;
	int numPluckNotes = MODE_SP_MAX_PLUCKNOTES;
	float dynamics = 0.0f;

	float highpassed_haptic_1 = 0.0f;
	float hapticSig_1 = 0.0f;

	MathExtras::Threshold_Trigger<float> directionGate;
	MathExtras::Threshold_Trigger<float> muteGate;
	MathExtras::StepTracker<float> stepTracker;

	PluckNote notes[MODE_SP_MAX_PLUCKNOTES];
	int curNoteIdx = 0;

	double rampCycleDuty = 0.5f;

	void SampleNewNote();

	float pluckTrain(double angle);
	float dynamicsCurveTrain(double angle);
	//double pluckAngle(int pluckIdx);
	int pluckPoint(double angle);

	void UpdateShiftDirMultFromCycle();
};




#endif /* INC_MODES_MODE_SEQUENCEDPLUCK_H_ */
