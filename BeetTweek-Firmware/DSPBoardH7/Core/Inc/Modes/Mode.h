/*
 * Mode.h
 *
 *  Created on: Nov 19, 2020
 *      Author: casht
 */

#ifndef INC_MODE_H_
#define INC_MODE_H_

#include "main.h"
#include "DACX0504.h"
#include "LEDPanelManager.h"
#include "DualBoardComHandler.h"
#include "MotorAngleState.h"
#include "MathExtras_DSP.h"
#include "EuroRack.h"
#include "adcdacbuffers.h"
#include "BlockProcessing.h"
#include "guestures.h"
#include "InputOutputDescriptor.h"
#include "DoubleBuffered_FileBacked.h"
#include "EEPromMap.h"

#include "stdio.h"
#include "string.h"
#include "clockinput.h"

#include <string>
#include "fatfs.h"
#include "ee24.h"
#include "arm_math.h"


extern uint16_t MotorSpiOutData_Latest ;
extern uint16_t MotorAngleData_Latest ;
extern MotorAngleState GlobalMotorAngleState;

extern LEDPanelManager LEDManager;
extern DACX0504Chip DACChip;
extern DualBoardComHandler ComHandler;

extern float DACCalibrationPoints[4][NUM_DAC_ADC_CAL_POINTS];
extern float ADCCalibrationPoints[4][NUM_DAC_ADC_CAL_POINTS];
extern float DACADCCalibrationPointsVolts[NUM_DAC_ADC_CAL_POINTS];
extern arm_linear_interp_instance_f32 VoltToADCCalInterpInst[4];
extern arm_linear_interp_instance_f32 VoltToDACCalInterpInst[4];

//tune with fat allocation size (16kbytes sd)
#define KNOB_GESTURE_DOUBLE_PAGE_SIZE (256)
#define KNOB_GESTURE_SAMPLE_SKIP (5)


#define VOLTS_PER_MOTOR_ROTATION (ER_CV_VOLTAGE_MAX)

#define ANTIALIAS_FIRSTORDER_FILT_COEFF (50)


#define WHITE_BUTTON_HOLD_CNT 50

//Experimentally Found...
#define MAX_RPM_FULL_TORQUE (5.0f)


#define MODE_DESC_MAX_CHARS 256
#define FAST_DIGI_TO_VOLTS

#define ANTCOGGPOINTS (720)
#define ANTICOGGMINSAMPLES (200)

#define KNOB_DEAD_ZONE_ANGLE_HALF (0.05)


//Base Class For Modes - Minimal Functionality.
class Mode {
public:
	Mode();
	virtual ~Mode();



	//do setup things when the mode is entered
	virtual void Initialize();

	//un-setup thing in prep for next mode
	virtual void UnInitialize();

	//updates the front panel
	virtual void UpdateLEDS(float sampleTime);

	//Called Very Fast.  Holds DSP Code
	virtual void AudioDSPFunction(float sampleTime, int bufferSwap);

	//Called Somewhat Fast.  Holds Knob Update Processing.
	virtual void KnobDSPFunction(float sampleTime);

	//call from main thread.
	virtual void MainThreadUpdateFunction(float sampleTime);

	//call to tell the mode to handle the button down.
	virtual void ButtonDown(int button);

	//call to tell the mode to handle the button up.
	virtual void ButtonUp(int button);

	//called when FUNC + a button is pressed
	virtual void OnFuncCombo(int button);

	virtual void OnButtonQuickAction(int button);

	virtual void OnButtonClickedLongHold(int button);

	virtual void OnButtonLongHold(int button);

	virtual void OnADCPlugChange(int adc);


	virtual void OnSaveTimerTimeout();


	//called when gesture record starts.
	virtual void OnGestureRecordStart(){
		//printf("OnGestureRecordStarting %d\r\n", knobGestureIdx);


	}

	//called when gesture record ends.
	virtual void OnGestureRecordStop(){
		//printf("OnGestureRecordStopping %d\r\n", knobGestureIdx);

		knobGestureRecordBPS = tempo.bps;

		RestartSaveTimer();
		saveKnobGestureRecordBPS = true;
	}
	//called when gesture playback loops
	virtual void OnGestureLoop(){
		//printf("OnGestureLoop\r\n");
	}

	//called when gesture playback starts
	virtual void OnGesturePlay(){
		//printf("OnGesturePlay %d\r\n", knobGestureIdx);
	}

	virtual void WriteExtraGuestureData(float* data){}
	virtual void ReadExtraGuestureData(float* data){}

	//Prints Debug Information
	virtual void DebugPrint();

	//writes the entire state of the mode to eeprom
	virtual bool WriteEEPROMState(uint32_t &ee_address)
	{
		bool success = true;
		for(int i = 0; i < 8; i++)
		{
			success &= ee24_write_32(ee_address, inputOutputDescriptors[i].curAugment, 1000);
			ee_address += sizeof(uint32_t);
		}



		return success;
	}

	//reads the entire state of mode from eeprom immediately.
	virtual bool ReadEEPROMState(uint32_t &ee_address)
	{
		bool success = true;
		for(int i = 0; i < 8; i++)
		{
			success &= ee24_read_32(ee_address, (uint32_t*)&inputOutputDescriptors[i].curAugment,  1000);
			ee_address += sizeof(uint32_t);

			if(inputOutputDescriptors[i].curAugment >= inputOutputDescriptors[i].numAugments || inputOutputDescriptors[i].curAugment < 0 /*|| (i < 4 && !ADCPluggedIn(i))*/)
			{
				inputOutputDescriptors[i].curAugment = inputOutputDescriptors[i].prefferedAugment;
			}

		}




		return success;
	}

	//method for reading extra SDFileData for mode operation.
	virtual bool ReadSDFileData()
	{
		return true;
	}



	const int eePromBytesUsed = 8*sizeof(uint32_t);

	virtual int EEPromBytesUsed()
	{
		return eePromBytesUsed;
	}
	virtual int EEPromAddressOffsets(int offset)
	{
		return offset*sizeof(uint32_t);
	}

	virtual void OnPostCycleAugment(int buttonIdx, int augmentIdx);



	float virtualAngleOffset;
	MotorAngleState LocalMotorAngleState;
	MotorAngleState LocalMotorAngleStatePure;
	MotorAngleState LocalMotorAngleStateGesture;
	float loopOffsetAngle = 0;


	struct GESTUREDATA {
		float angle;
		float vel;
		float modeData[4];
	};


	static DoubleBuffer_FileBacked<GESTUREDATA, KNOB_GESTURE_DOUBLE_PAGE_SIZE> gestureBuffer;
	static FIL gestureFile;

	static uint32_t knobGestureSampleStretch ;
	static float knobGestureRecordBPS ;
	static bool saveKnobGestureRecordBPS;

	enum GUESTURE_LEVEL
	{
	GUESTURE_LEVEL_REC_RST  = 0,
	GUESTURE_LEVEL_REC      = 1,
	GUESTURE_LEVEL_PLAY     = 2,
	GUESTURE_LEVEL_PLAY_RST = 3
	};

	MathExtras::MultiLevel_Threshold_Trigger<float,4> gestTrgtLevelTrigger;
	static GUESTURE_LEVEL curGestureLevel;
	static double knobAngleGesturePlaybackLatest;
	static MotorAngleState knobAngleGestureInitial;
	static bool gestureSaveRecInfo;
	static uint32_t gestureRecEndIdx;
	static bool bypassGuestureInput;


	uint32_t lastGestLrgIdxSync = 0;

	GESTUREDATA latestGDREAD;


	//currently applied driveTorque -1 to 1
	float driveTorque = 0.0f;
	float driveTorqueFiltered = 0.0f;




	//calibration and anticogging
	/*
	enum CalibrationMode {
		CalibrationMode_None = 0,
		CalibrationMode_AntiCoggCW,
		CalibrationMode_AntiCoggCCW,
		CalibrationMode_AntiCoggTESTCW,
		CalibrationMode_AntiCoggTESTCCW,
	};
	static MathExtras::PID<float> antiCoggPID;
	static CalibrationMode calibrationMode;
	static int calibrationDelayCountdown;

	static float coggDriveLookupCW[ANTCOGGPOINTS];
	//static float coggDriveCWMean;
	static int coggDriveLookupCW_SMPLCNT[ANTCOGGPOINTS];
	static bool coggDriveLookupCW_SMPLCMPL;//all samples acquired

	static float coggDriveLookupCCW[ANTCOGGPOINTS];
	//static float coggDriveCCWMean;
	static int coggDriveLookupCCW_SMPLCNT[ANTCOGGPOINTS];
	static bool coggDriveLookupCCW_SMPLCMPL;//all samples acquired
    */
	static float frictionCalFactor;
	static float drivePowerFactor;
	static float driveOffset;

	static void PrintCoggStats(int i);
	static void DoAntiCoggCalibration();


	int bpmTapFlag = 0;
	float tapBlinkTim = 0;
	bool saveTapBpm = false;
	bool waitingSync = false;
	int mainThreadSyncInd = 0;

	float recentTapBps = 1.0f;
	float recentTapBps_1 = 1.0f;
	int recentTapBps_Counter = 1000;

	int saveTimer = 0;
	bool saveTimerEn = 0;

	uint32_t audioTick = 0;
	uint32_t knobDspTick = 0;

	bool gesturePlayLoopPrimed = false;
	int justLoopedInd = 0;

	bool firstLEDFrame = 1;

	bool funcMode = false;

	bool isHidden = false;

	//set both unique name and modeName
	void SetBothNames(const char* name)
	{
		strcpy(modeUniqueName, name);
		strcpy(modeName, name);
	}

	//unique name for the mode
	char modeUniqueName[256];

	//name for the mode
	char modeName[256];

	//description for the mode
	char modeDescription[256];


	inline void RestartSaveTimer()
	{
		saveTimer = 500;
		saveTimerEn = true;
	}


	DualBoardComHandler::PackScheme packScheme;


	void StandardPanelLEDDraw();

	//draws the color on the button.
	void DrawLEDButtonStandard(InputOutputDescriptor* desc);


	void TorqueGuestureProcessFilter(float sampleTime);

	void MasterProcess_Torque(float sampleTime);
	void PreKnobDSP(float sampleTime);

	void PreAudioDSP(float sampleTime);//common stuff before audio dsp function.

	void PostLEDUpdate(float sampleTime);//common led stuff after led function.


	bool GuestureStateWantedTransition(GUESTURE_LEVEL from, GUESTURE_LEVEL to)
	{
		if(Mode::curGestureLevel == from && gestTrgtLevelTrigger.TriggerRegion() == to)
			return true;
		else
			return false;
	}


	void RecordButtonLevelCycle();

	//do a standard remap from angle (-1 to 1) to and output (-1 to 1) with dead zone equal to 0 with no jump out of deadzone.
	float KnobAngleDeadZoneRemap(float angle);
	bool AngleInDeadZone(float knobAngle);

	//functions to be called from mode dsp code.
	inline void SetDacValue(int channel, float value)
	{
		dacOuts[channel] = value;
		dacVolts[channel] = DacToVolts(channel, value);
	}

	inline void SetDacVolts(int channel, float volts)
	{
		dacOuts[channel] = VoltsToDac(channel, volts);
		dacVolts[channel] = volts;
	}



	//Convert a ADC or DAC to Volts
	inline float DigiToVolts(int channel, float value, float calibrationPoints[][NUM_DAC_ADC_CAL_POINTS])
	{
		//float volts = float(value)*8.0f;
		//return volts;

		#ifdef FAST_DIGI_TO_VOLTS
			//interp from -8 to 8
			float bucket1Dist = value - calibrationPoints[channel][1];
			float bucket2Dist = calibrationPoints[channel][NUM_DAC_ADC_CAL_POINTS-2] - value;

			return (DACADCCalibrationPointsVolts[1]*bucket2Dist + DACADCCalibrationPointsVolts[NUM_DAC_ADC_CAL_POINTS-2]*bucket1Dist)/(calibrationPoints[channel][NUM_DAC_ADC_CAL_POINTS-2] - calibrationPoints[channel][1]);
		#else

		//SLOW Code...
		//find what bucket digivalue is in out of the 16.
		int bucketIdx;
		float bucket1Digi, bucket2Digi;
		static int missCount = 0;

		bucketIdx = (value/float(0xFFFF))*float(NUM_DAC_ADC_CAL_POINTS-2.0f);

		//correct the bucket if needed.
		bucket1Digi = calibrationPoints[channel][bucketIdx];
		bucket2Digi = calibrationPoints[channel][bucketIdx+1];

		int slopeDir = MathExtras::Sign(bucket2Digi -bucket1Digi);

		if(slopeDir == 1)
		{

			while(value > bucket2Digi && bucketIdx < (NUM_DAC_ADC_CAL_POINTS-2))
			{
				bucketIdx++;
				bucket1Digi = calibrationPoints[channel][bucketIdx];
				bucket2Digi = calibrationPoints[channel][bucketIdx+1];
				missCount++;
			}
			while(value < bucket1Digi && bucketIdx > 0)
			{
				bucketIdx--;
				bucket1Digi = calibrationPoints[channel][bucketIdx];
				bucket2Digi = calibrationPoints[channel][bucketIdx+1];
				missCount++;
			}

		}
		else if(slopeDir == -1)
		{

			while(value < bucket2Digi && bucketIdx < (NUM_DAC_ADC_CAL_POINTS-2))
			{
				bucketIdx++;
				bucket1Digi = calibrationPoints[channel][bucketIdx];
				bucket2Digi = calibrationPoints[channel][bucketIdx+1];
				missCount++;
			}
			while(value > bucket1Digi && bucketIdx > 0)
			{
				bucketIdx--;
				bucket1Digi = calibrationPoints[channel][bucketIdx];
				bucket2Digi = calibrationPoints[channel][bucketIdx+1];
				missCount++;
			}

		}


		float bucket1Volts = DACADCCalibrationPointsVolts[bucketIdx];
		float bucket2Volts = DACADCCalibrationPointsVolts[bucketIdx+1];

		int bucket1Dist = value - bucket1Digi;
		int bucket2Dist = bucket2Digi - value;


		float volts = (bucket1Volts*(bucket2Dist) + bucket2Volts*(bucket1Dist))/(bucket2Digi - bucket1Digi);

		if(isnan(volts))
		{
			Error_Handler();
		}

		return volts;
		#endif
	}

//	inline float VoltsToDigi(int channel, float volts, arm_linear_interp_instance_f32 interpolator[])
//	{
//
//
//		volts = MathExtras::ClampInclusive(volts, DACADCCalibrationPointsVolts[1], DACADCCalibrationPointsVolts[NUM_DAC_ADC_CAL_POINTS-2]);
//
//		float span =
//
//
//		return arm_linear_interp_f32(&interpolator[channel], volts);
//	}



	inline float ADCToVolts(int channel, float adc)
	{
		return DigiToVolts(channel, adc, ADCCalibrationPoints);
	}

	inline float VoltsToADC(int channel, float volts)
	{
		float span = ADCCalibrationPoints[channel][NUM_DAC_ADC_CAL_POINTS-2] - ADCCalibrationPoints[channel][1];

		return ((volts+8.0f)/16.0f)*span + ADCCalibrationPoints[channel][1];
	}

	inline float VoltsToDac(int channel, float volts)
	{
		//return VoltsToDigi(channel, volts, VoltToDACCalInterpInst);

		float span = DACCalibrationPoints[channel][NUM_DAC_ADC_CAL_POINTS-2] - DACCalibrationPoints[channel][1];

		return ((volts+8.0f)/16.0f)*span + DACCalibrationPoints[channel][1];
	}

	inline float DacToVolts(int channel, float dac)
	{
		return DigiToVolts(channel, dac, DACCalibrationPoints);
	}

	inline float VoltsToMotorAngle(float volts, EuroRack::SignalType sigType)
	{
		//2.5v maps to +0.5, -2.5v maps to -0.5
		return (volts)/(EuroRack::SignalVoltRange(sigType));
	}

	inline float MotorAngleToVolts(float motorAngle)
	{
		//MAX TURN RULE
		motorAngle = MathExtras::ClampInclusive(motorAngle, -1.0f, 1.0f);
		return motorAngle*VOLTS_PER_MOTOR_ROTATION;
	}



	inline void SaveSafeGestureState(float voltageLevel)
	{
		bool writeAsIs = false;
		writeAsIs |= gestTrgtLevelTrigger.ValueInRegion(voltageLevel, GUESTURE_LEVEL_PLAY);
		writeAsIs |= gestTrgtLevelTrigger.ValueInRegion(voltageLevel, GUESTURE_LEVEL_PLAY_RST);


		if(writeAsIs)
			ee24_write_float(EE_GESTURE_STATE_32bits, voltageLevel, 1000);
		else
			ee24_write_float(EE_GESTURE_STATE_32bits, gestTrgtLevelTrigger.RegionMid(GUESTURE_LEVEL_PLAY_RST), 1000);

		ee24_write_8(EE_GESTURE_STATESAVED_BOOL_8bits, 1, 1000);

	}

};



//class for managing the collections of modes, and there transitions. As well as mode selection menu routines etc...
#define MODE_MANAGER_MAX_MODES 64
class ModeManager : public Mode
{
public:
	const float PanelHues[8] = {
			61.0/360.0f,
			131.0/360.0f,
			182.0/360.0f,
			246.0/360.0f,
			300.0/360.0f,
			0.0/360.0f,
			61.0/360.0f,
			131.0/360.0f
	};

//	//do setup things when the mode is entered

//
//	//un-setup thing in prep for next mode
//	virtual void UnInitialize();
//
//	//updates the front panel
	virtual void UpdateLEDS(float sampleTime)
	{
		static float blinky = 0.5f;
		static float timeAccum = 0.0f;

		timeAccum += sampleTime;
		blinky = (arm_sin_f32(timeAccum*10.0f) + 1.0f)*0.5f;

		if(photoMode)
			blinky = 0.5f;

		int numPages = MathExtras::Ceil(modesListEndIdx/8.0f);

		if(modeSelectMode == 1)
		{
			WS2812_SETALLLED(0,0,0);


			LEDManager.SetLEDRingRangeLinear_Float(LEDPanelManager::RINGIDENTIFIER_OUTER, -LocalMotorAngleState.currentAngle + 0.5f/(numPages*2.0f) - 1.0f/LED_NUM_LEDS_PER_RING, -LocalMotorAngleState.currentAngle - 0.5f/(numPages*2.0f) - 1.5f/LED_NUM_LEDS_PER_RING,
					(MathExtras::Color*)&MathExtras::Color::BLUE,
					(MathExtras::Color*)&MathExtras::Color::BLUE,
					1.0f);

			for(int clockIndex = 0; clockIndex < LED_NUM_LEDS_PER_RING; clockIndex++)
			{

				float ledAngle = float(clockIndex)/(LED_NUM_LEDS_PER_RING);
				float torqueIndicator = arm_sin_f32(((ledAngle)*M_TWOPI) * numPages - M_PI_2 ) + 1.0f ;


				float fillFactor = (numPages*2)/float(LED_NUM_LEDS_PER_RING);


				if(torqueIndicator >= 1.0f-fillFactor-0.01)
				{

					MathExtras::Color color;
					float precIn = clockIndex/float(LED_NUM_LEDS_PER_RING);
					int pageIdx = precIn*numPages;
					color.FromHSL(float(pageIdx)*0.5f, 1.0f, 0.5f*(MathExtras::ClampInclusive(blinky, 0.5f, 1.0f)));

					LEDManager.SetLEDInnerRing_Int(clockIndex,  color.r_*LED_BASE_BRIGHTNESS_255, color.g_*LED_BASE_BRIGHTNESS_255, color.b_*LED_BASE_BRIGHTNESS_255);
				}

			}


			int curPageIdx = LocalMotorAngleState.currentAngle*numPages;

			//turn all panel lights colored indicating menu.
			for(int i = 0; i < MathExtras::Min(8, modesListEndIdx-8*curPageIdx); i++)
			{
				if(modes[i]->isHidden)
					continue;


				MathExtras::Color color;


				color.FromHSL(PanelHues[i], 1.0f, 0.5f*(MathExtras::ClampInclusive(blinky, 0.5f, 1.0f)));

				LEDManager.SetLEDButton(i, color.r_*LED_BASE_BRIGHTNESS_255,
						color.g_*LED_BASE_BRIGHTNESS_255,
						color.b_*LED_BASE_BRIGHTNESS_255);
			}












		}
		else if(firmwareShowMode)
		{
			WS2812_SETALLLED(0,0,0);
			for(uint32_t i = 0; i < firmwareVersion; i++)
			{
				LEDManager.SetLEDInnerRing_Int(i, 0, LED_BASE_BRIGHTNESS_255,(i%2)*LED_BASE_BRIGHTNESS_255);
			}
		}


	}
//
//	//Called Very Fast.  Holds DSP Code
	void AudioDSPFunction(float sampleTime, int bufferSwap)
	{

		//silence.
		BLOCK_PROCESS_ALL()
			dac0DMA[sampleIdxDMA] = 0;
			dac1DMA[sampleIdxDMA] = 0;
		}


		//for(int i = 0; i <4; i++)
		//	dacOuts[i] = 0.0f;


	}
//
//
//	//Called Somewhat Fast.  Holds Knob Update Processing.
    void KnobDSPFunction(float sampleTime)
    {
		int numPages = MathExtras::Ceil(modesListEndIdx/8.0f);

    	//torque curve
    	driveTorque = arm_sin_f32((LocalMotorAngleState.currentAngle*M_TWOPI) * numPages ) - LocalMotorAngleState.currentAngularVelFiltered*0.5f;//add drag.



    	MasterProcess_Torque(sampleTime);

    }
//
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
//	virtual void DebugPrint();



	//add an existing mode
	Mode* AddMode(Mode* mode)
	{
		if(modesListEndIdx >= MODE_MANAGER_MAX_MODES)
		{
			strcpy(errorCode, "AddMode1");
			Error_Handler();
		}
		modes[modesListEndIdx] = mode;
		modesListEndIdx++;
		return mode;
	}

	bool TransitionToMode(std::string name)
	{
		//find the mode by its name
		for(int i = 0; i < modesListEndIdx; i++)
		{
			if(modes[i]->modeName == name)
			{
				return TransitionToMode(modes[i]);
			}
		}

		return false;
	}
	bool TransitionToMode(Mode* mode)
	{
		if(mode == currentMode())
			return false;

		__disable_irq();

		int foundNodeIdx = -1;
		for(int i = 0; i < MODE_MANAGER_MAX_MODES; i++)
		{
			if(mode == modes[i])
			{
				foundNodeIdx = i;
				break;
			}
		}

		if(foundNodeIdx == -1)
		{
			strcpy(errorCode, "AddMode2");
			Error_Handler();
		}

		if(currentModeIdx != -1)
			modes[currentModeIdx]->UnInitialize();

		modes[foundNodeIdx]->Initialize();
		PostTransitionDescriptorPreferenceSetup();


		prevModeIdx = currentModeIdx;
		currentModeIdx = foundNodeIdx;

		__enable_irq();


		PostTransitionEEPromActions();
		return true;

	}

	void TransitionToNextModeInList()
	{
		 __disable_irq();

		modes[currentModeIdx]->UnInitialize();

		prevModeIdx = currentModeIdx;
		currentModeIdx++;
		if(currentModeIdx >= modesListEndIdx)
		{
			currentModeIdx = 0;
		}


		modes[currentModeIdx]->Initialize();
		PostTransitionDescriptorPreferenceSetup();

		__enable_irq();

		PostTransitionEEPromActions();

	}
	void TransitionToPrevModeInList()
	{
		 __disable_irq();

		modes[currentModeIdx]->UnInitialize();
		prevModeIdx = currentModeIdx;
		currentModeIdx--;
		if(currentModeIdx < 0)
		{
			currentModeIdx = modesListEndIdx-1;
		}

		modes[currentModeIdx]->Initialize();
		PostTransitionDescriptorPreferenceSetup();


		__enable_irq();

		PostTransitionEEPromActions();
	}

	void PostTransitionDescriptorPreferenceSetup()
	{
		for(int i = 0 ; i < 8; i++)
		{
			inputOutputDescriptors[i].curAugment = inputOutputDescriptors[i].prefferedAugment;
		}
	}

	uint32_t CurrentModeEEPromBase(){ return EE_MODE_MEMORY_0+currentModeIdx*EE_BYTES_PER_MODE; }
	uint32_t ModeEEPromBase(int i){ return EE_MODE_MEMORY_0+i*EE_BYTES_PER_MODE; }

	void PostTransitionEEPromActions()
	{
		//save the mode
		uint8_t modeIdx = currentModeIdx;
		ee24_write(EE_LASTMODE, &modeIdx, 1, 1000);

		//retrieve the mode data
		uint32_t baseEEPromAddress = CurrentModeEEPromBase();
		if(!modes[modeIdx]->ReadEEPROMState(baseEEPromAddress))
			Error_Handler();

		//read gesture record/playback state.
		uint8_t gestureDataSaved = 0;
		ee24_read_8(EE_GESTURE_STATESAVED_BOOL_8bits, &gestureDataSaved, 1000);
		if(gestureDataSaved == 1)
			ee24_read_float(EE_GESTURE_STATE_32bits, &inputOutputDescriptors[2].augments[0].defaultVoltVal, 1000, 0.0f);




		ee24_read_32(EE_GESTURE_ENDIDX_32bits, &gestureBuffer.endIdx, 1000);
		ee24_read(EE_GESTURE_MOTORSTATE, (uint8_t*)&Mode::knobAngleGestureInitial, sizeof(MotorAngleState), 1000);


		ee24_read_float(EE_GESTURE_TEMPO_FLOAT_32bits, &Mode::knobGestureRecordBPS, 1000, 1.0f);


		//read fake-plug state
		for(int i = 0; i < 4; i++)
		{
			ee24_read_32(EE_FAKE_PLUG_STATES_32bits + i*sizeof(int), (uint32_t*)&adcFakePlugStates[i],  1000);

			if(adcFakePlugStates[i] < 0 || adcFakePlugStates[i] > 1 || adcPlugStates[i])
			{
				adcFakePlugStates[i] = 0;
			}
		}





		if(adcFakePlugStates[3] == 1)
		{
			inputOutputDescriptors[3].augments[0].useCustomColorIntensity = true;
			inputOutputDescriptors[3].augments[0].customColorIntensity = 0.3f;
		}
		else
		{
			inputOutputDescriptors[3].augments[0].useCustomColorIntensity = false;
		}


		//retrieve SD data
		modes[modeIdx]->ReadSDFileData();

	}


	bool TransitionToPrevMode()
	{
		if(prevModeIdx < 0)
			return false;

		return TransitionToMode(modes[prevModeIdx]);
	}


	inline Mode* currentMode(){

		if(modeSelectMode || firmwareShowMode)
			return this;
		else
			return modes[currentModeIdx];
	}

	//returns the actual mode regardless of modeSelectMode
	inline Mode* currentEffectiveMode(){
		return modes[currentModeIdx];
	}

	GPIO_PinState DebouncedRead(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
	{
		const int numDebounces = 0;
		bool goodToGo = true;
		GPIO_PinState initialState;
		do
		{
			initialState = HAL_GPIO_ReadPin(GPIOx,GPIO_Pin);
			for(int i = 0 ; i < numDebounces; i++)
			{
				if(HAL_GPIO_ReadPin(GPIOx,GPIO_Pin) != initialState)
				{
					goodToGo = false;
					break;
				}
			}
		}
		while(goodToGo == false);
		return initialState;
	}


	void CycleAugment(int descriptorIdx)
	{
		inputOutputDescriptors[descriptorIdx].curAugment++;

		if(inputOutputDescriptors[descriptorIdx].curAugment >= inputOutputDescriptors[descriptorIdx].numAugments)
			inputOutputDescriptors[descriptorIdx].curAugment = 0;

		//save the augment change to eeprom
		ee24_write_32(CurrentModeEEPromBase()+descriptorIdx*sizeof(uint32_t), inputOutputDescriptors[descriptorIdx].curAugment, 1000);

		currentMode()->OnPostCycleAugment(descriptorIdx, inputOutputDescriptors[descriptorIdx].curAugment);


		//also increment every link
		for(int i = 0; i < inputOutputDescriptors[descriptorIdx].numLinks; i++)
		{
			int linkedDescIdx = inputOutputDescriptors[descriptorIdx].linkedAugmentsButtons[i];

			inputOutputDescriptors[linkedDescIdx].curAugment++;

			if(inputOutputDescriptors[linkedDescIdx].curAugment >= inputOutputDescriptors[linkedDescIdx].numAugments)
				inputOutputDescriptors[linkedDescIdx].curAugment = 0;
		}
	}



	void SetAugmentsToPreffered()
	{
		for( int desc = 0; desc < 8; desc++)
		{
			if(inputOutputDescriptors[desc].curAugment != inputOutputDescriptors[desc].prefferedAugment)
			{
				inputOutputDescriptors[desc].curAugment = inputOutputDescriptors[desc].prefferedAugment;
				//save the augment change to eeprom
				ee24_write_32(CurrentModeEEPromBase()+desc*sizeof(uint32_t), inputOutputDescriptors[desc].curAugment, 1000);
				currentMode()->OnPostCycleAugment(desc, inputOutputDescriptors[desc].curAugment);
			}
		}
	}

	//poll hardware buttons and update current mode accordingly
	void UpdateButtons()
	{
		Mode* currentMode = modes[currentModeIdx];
		GPIO_PinState* statesPrev = buttonStates_Prev;


		buttonStates[0] = DebouncedRead(PUSH_BUTTON_IN_1_GPIO_Port, PUSH_BUTTON_IN_1_Pin);
		buttonStates[1] = DebouncedRead(PUSH_BUTTON_IN_2_GPIO_Port, PUSH_BUTTON_IN_2_Pin);
		buttonStates[2] = DebouncedRead(PUSH_BUTTON_IN_3_GPIO_Port, PUSH_BUTTON_IN_3_Pin);
		buttonStates[3] = DebouncedRead(PUSH_BUTTON_IN_4_GPIO_Port, PUSH_BUTTON_IN_4_Pin);

		buttonStates[4] = DebouncedRead(PUSH_BUTTON_OUT_1_GPIO_Port, PUSH_BUTTON_OUT_1_Pin);
		buttonStates[5] = DebouncedRead(PUSH_BUTTON_OUT_2_GPIO_Port, PUSH_BUTTON_OUT_2_Pin);
		buttonStates[6] = DebouncedRead(PUSH_BUTTON_OUT_3_GPIO_Port, PUSH_BUTTON_OUT_3_Pin);
		buttonStates[7] = DebouncedRead(PUSH_BUTTON_OUT_4_GPIO_Port, PUSH_BUTTON_OUT_4_Pin);

		buttonStates[8] = DebouncedRead(PUSH_BUTTON_FUNC_GPIO_Port, PUSH_BUTTON_FUNC_Pin);
		buttonStates[9] = DebouncedRead(PUSH_BUTTON_FUNC2_GPIO_Port, PUSH_BUTTON_FUNC2_Pin);


		for(int i = 0; i < 10; i++)
		{
			if(buttonStates[i] != statesPrev[i])
			{
				if(buttonStates[i] == GPIO_PIN_SET)
				{

					modes[currentModeIdx]->ButtonDown(i);
					//printf("button %d pressed down\r\n", i);
				}
				else
				{
					buttonStates_FullClicked[i] = true;
					modes[currentModeIdx]->ButtonUp(i);
					//printf("button %d released (clicked)\r\n", i);
				}
			}

			//inc hold counts
			if(buttonStates[i] == GPIO_PIN_SET && buttonHoldCounts[i] != -1)
			{
				buttonHoldCounts[i] = MathExtras::ClampInclusive(buttonHoldCounts[i] + 1, 0, 1000);
			}

		}

		if(buttonStates_FullClicked[8] && modeSelectMode)//if mode button clicked again, exit mode select mode...
		{
			modeSelectMode--;
			currentMode->firstLEDFrame = true;
		}
		else if(buttonStates_FullClicked[8] && !currentMode->funcMode && modeSelectMode == 0)
		{
			modeSelectMode = 1;//go into mode select mode

			//stop recording or playback
			inputOutputDescriptors[2].augments[0].defaultVoltVal = gestTrgtLevelTrigger.RegionMid(GUESTURE_LEVEL_PLAY_RST);

			firmwareShowMode = false;
		}


		if(HAL_GetTick() - funcPressFirmwareCounterTicks > 10000)
		{
			funcPressFirmwareCounter = 0;
		}


		if(buttonStates_FullClicked[9] && !buttonStates[8])
		{

			currentMode->funcMode = !currentMode->funcMode;

			if(funcPressFirmwareCounter == 0)
				funcPressFirmwareCounterTicks = HAL_GetTick();

			funcPressFirmwareCounter++;
		    if(funcPressFirmwareCounter >= 10 || firmwareShowMode)
			{
				funcPressFirmwareCounter = 0;
				firmwareShowMode = !firmwareShowMode;
				currentMode->funcMode = false;
			}
		}


		bool buttonClicked = false;
		if(currentMode->funcMode)//FUNC + button
		{

			if(funcPressFirmwareCounter == 5)//special options
			{
				if(buttonStates_FullClicked[0])
				{
					//flip the front plate.
					LEDManager.panelReversed_ = !LEDManager.panelReversed_;
					ee24_write_32(EE_FLIPPED_FRONT_PLATE, static_cast<uint32_t>(LEDManager.panelReversed_), 1000);

					buttonClicked = true;
				}
				else if(buttonStates_FullClicked[1])
				{
					photoMode = !photoMode;
					buttonClicked = true;
				}

				if(buttonClicked)
				{
					//return to normal.
					funcPressFirmwareCounter = 0;
					currentMode->funcMode = false;
					firmwareShowMode = false;
				}
			}
			else
			{
				for(int i = 0; i < 8; i++)
				{
					if(buttonStates_FullClicked[i])
					{
						buttonClicked = true;
						currentMode->funcMode = false;
						if(inputOutputDescriptors[i].quickAction && inputOutputDescriptors[i].numAugments > 1)
						{
							CycleAugment(i);
						}
						else if(inputOutputDescriptors[i].funcCombo)
						{
							modes[currentModeIdx]->OnFuncCombo(i);
						}
						else if(inputOutputDescriptors[i].quickAction && !inputOutputDescriptors[i].CurAugment().noQuickAction)
						{
							modes[currentModeIdx]->OnButtonQuickAction(i);
						}
						firmwareShowMode = false;
						break;
					}
				}

				if(buttonStates_FullClicked[8])//FUNC + MODE
				{
					currentMode->funcMode = false;
					firmwareShowMode = false;
				}
			}
		}
		else if(modeSelectMode == 1)//if in primary mode select mode.
		{
			int numPages = MathExtras::Ceil(modesListEndIdx/8.0f);
			int curPageIdx = LocalMotorAngleState.currentAngle*numPages;

			for(int i = 0; i < MathExtras::Min(8, modesListEndIdx-8*curPageIdx); i++)
			{
				if(buttonStates_FullClicked[i])
				{
					if(modes[i+8*curPageIdx] != currentMode)
					{
						//save all of the current mode's state because its a good time to do it, as a visible hickup delay is OK at this point.
						uint32_t eeprombase = CurrentModeEEPromBase();
						currentMode->WriteEEPROMState(eeprombase);

						TransitionToMode(modes[i+8*curPageIdx]);

						//since the user switches to the mode - set all augments to preferred.
						SetAugmentsToPreffered();

					}
					modeSelectMode = 0;//return to normal mode operation
					break;
				}
			}
		}
		else if(!buttonStates[9] && !buttonStates[8])// just a button
		{
			for(int i = 0; i < 8; i++)
			{
				if(buttonHoldCounts[i] >= WHITE_BUTTON_HOLD_CNT)
				{
					modes[currentModeIdx]->OnButtonLongHold(i);
					if(buttonStates_FullClicked[i])
					{
						buttonClicked = true;
						buttonHoldCounts[i] = -1;
						modes[currentModeIdx]->OnButtonClickedLongHold(i);
					}
				}
				else if(buttonStates_FullClicked[i])
				{
					buttonClicked = true;
					if(inputOutputDescriptors[i].quickAction && !inputOutputDescriptors[i].CurAugment().noQuickAction)
					{
						modes[currentModeIdx]->OnButtonQuickAction(i);
					}
					else if(inputOutputDescriptors[i].numAugments == 1)
					{
						if(inputOutputDescriptors[i].funcCombo)
							modes[currentModeIdx]->OnFuncCombo(i);
					}
					else
						CycleAugment(i);
				}
			}
		}
		if(buttonClicked)
		{
			firmwareShowMode = false;
		}





		//Module Reset Panic COMBO
		if(buttonStates_Prev[6] && buttonStates_Prev[7] && (buttonStates_FullClicked[6] || buttonStates_FullClicked[7]))
		{
			ResetBoards();
		}


		for(int i = 0; i < 8+2; i++)
		{
			if(buttonStates_FullClicked[i])
				buttonHoldCounts[i] = 0;
		}


		//advance.
		for(int i = 0; i < 8+2; i++)
		{
			buttonStates_Prev[i] = buttonStates[i];
			buttonStates_FullClicked[i] = false;
		}

	}

	bool calibrationMode = false;


	Mode* modes[MODE_MANAGER_MAX_MODES] = {nullptr};
	int modesListEndIdx = 0;
	int currentModeIdx = -1;
	int prevModeIdx = -1;

	int modeSelectMode = 0;


	int funcPressFirmwareCounter = 0;
	uint32_t funcPressFirmwareCounterTicks = 0;

	bool photoMode = false;
	bool firmwareShowMode = false;


	GPIO_PinState buttonStates[8 + 2];
	GPIO_PinState buttonStates_Prev[8 + 2];
	int buttonHoldCounts[ 8 + 2 ];
	bool buttonStates_FullClicked[8 + 2];
};




#endif /* INC_MODE_H_ */
