/*
 * Mode_DJ.h
 *
 *  Created on: Mar 31, 2021
 *      Author: casht
 */

#ifndef INC_MODES_MODE_TURNTABLE_H_
#define INC_MODES_MODE_TURNTABLE_H_


#include "Modes/Mode.h"
#include "adcdacbuffers.h"
#include "SDPagedBuffer.h"


#define DJ_NUM_MARKS (8)
#define DJ_REC_BUFF_NUM_SAMPLES (8096*12)


class Mode_TurnTable : public Mode {
public:
	Mode_TurnTable();
	virtual ~Mode_TurnTable();

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
	virtual void MainThreadUpdateFunction(float sampleTime);

	virtual void OnSaveTimerTimeout();

	//writes the entire state of the mode to eeprom
	virtual bool WriteEEPROMState(uint32_t &ee_address);
	//reads the entire state of mode from eeprom immediately.
	virtual bool ReadEEPROMState(uint32_t &ee_address);

	virtual int EEPromBytesUsed()
	{
		return Mode::EEPromBytesUsed() + sizeof(float)*3 + sizeof(unsigned int)*1;
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
			byteOffset = sizeof(float)*2;
			break;
		case 3:
			byteOffset = sizeof(float)*3;
			break;
		}

		return (Mode::EEPromBytesUsed()) + byteOffset;
	}

	virtual bool ReadSDFileData();


	//call to tell the mode to handle the button down.
	//virtual void ButtonDown(int button);

	//call to tell the mode to handle the button up.
	virtual void ButtonUp(int button);

	virtual void OnButtonClickedLongHold(int button);

	virtual void OnFuncCombo(int button);


	virtual void OnButtonQuickAction(int button);

	virtual void OnPostCycleAugment(int buttonIdx, int augmentIdx);





	virtual void OnGestureLoop();
	virtual void OnGesturePlay();

	//Prints Debug Information
	virtual void DebugPrint();

	void FlipTurnDir();

	float preDriveTorque = 0.0f;
	double tableTargetAngle = 0.0f;
	double tableTargetAngle_1 = 0.0f;

	double tableTargetAngleFiltered = 0.0f;
	double tableTargetAngleFiltered_1 = 0.0f;

	void ResetTableTargetAndFilters();

	float tableTargetSpeed = 0.0f;
	float tableTargetSpeedSaved = 0.0f;
	int startupSampleDelayCount = 10000;//todo base on sampletime..
	float stillCount = 0;
	float speedCount = 0;

	int marksPerTable = DJ_NUM_MARKS;
	int tableTurnDir = 1;
	float beatProximity = 0.0f;
	float beatOffsetError = 0.0f;

	float wrapPerc_1 = 0.0f;
	float wrapPerc = 0.0f;
	float smp_1;

	float W1 = 0.0f;
	float W2 = 1.0f;

	bool switchedToPlayModeFlag = false;

	MathExtras::Threshold_Trigger<float> recordingPlaybackTriggerL;

	MathExtras::SinglePoleHPFilter<float> recordHPF;

	MathExtras::MultiLevel_Threshold_Trigger<float,3> tableDirTrigger;

	MathExtras::WrapAware_SinglePoleFilter<float> bpmOutPhaseFilter;
	MathExtras::Threshold_Trigger<float> bpmOutTrigger;

	MathExtras::WrapAware_SinglePoleFilter<float> knobAngleOutPhaseFilter;
	MathExtras::Threshold_Trigger<float> knobAngleOutTrigger;

	float tapeVisLEDS[LED_NUM_LEDS_PER_RING] = {0};

	//Record Buffer
	MathExtras::CircularTapeRecordPlayback_Buffer2<float, float, DJ_REC_BUFF_NUM_SAMPLES> signalBuffer;
	//SDBuffer<uint16_t, 512> audioBuffer;

	void SaveAudioBuffer(char* str);
	uint32_t saveNumber = 1;

	FIL audioFile;

};







#endif /* INC_MODES_MODE_TURNTABLE_H_ */
