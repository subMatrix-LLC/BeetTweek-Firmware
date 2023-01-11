/*
 * Mode_DJ.cpp
 *
 *  Created on: Mar 31, 2021
 *      Author: casht
 */


#include <clockinput.h>
#include <Modes/Mode_TurnTable.h>
#include "MathExtras.h"
#include "BlockProcessing.h"
#include "fatfs.h"

extern ModeManager modeManager;

Mode_TurnTable::Mode_TurnTable() : Mode()
{
	SetBothNames("Mode_DJ");

}
Mode_TurnTable::~Mode_TurnTable()
{

}

void Mode_TurnTable::Initialize() {
	Mode::Initialize();


	inputOutputDescriptors[0].enabled = true;
	inputOutputDescriptors[0].funcCombo = true;
	inputOutputDescriptors[0].augments[0].baseColor = MathExtras::Color::RED;

	inputOutputDescriptors[1].enabled = true;
	inputOutputDescriptors[1].quickAction = true;
	inputOutputDescriptors[1].augments[0].signalType = EuroRack::SignalType_GATE;
	inputOutputDescriptors[1].augments[0].baseColor = MathExtras::Color::RED;
	inputOutputDescriptors[1].augments[0].defaultVoltVal = ER_CV_VOLTAGE_MAX;
	inputOutputDescriptors[1].augments[0].useCustomColorIntensity = true;

	inputOutputDescriptors[2].numAugments = 2;
	inputOutputDescriptors[2].augments[1].signalType = EuroRack::SignalType_GATE;
	inputOutputDescriptors[2].augments[1].signalBehalvior = EuroRack::SignalBehalvior_NormalLinear_Inverted;
	inputOutputDescriptors[2].augments[1].baseColor = MathExtras::Color::ORANGE;
	inputOutputDescriptors[2].augments[1].useCustomColorIntensity = true;
	inputOutputDescriptors[2].augments[1].gateTrigger = &recordingPlaybackTriggerL;
	inputOutputDescriptors[2].prefferedAugment = 1;
	inputOutputDescriptors[2].funcCombo = true;


	inputOutputDescriptors[3].numAugments = 2;
	inputOutputDescriptors[3].augments[1].noQuickAction = true;
	inputOutputDescriptors[3].augments[1].baseColor = MathExtras::Color::MAGENTA;
	inputOutputDescriptors[3].augments[1].signalBehalvior = EuroRack::SignalBehalvior_NormalLinear;
	inputOutputDescriptors[3].augments[1].signalType = EuroRack::SignalType_CV;
	inputOutputDescriptors[3].augments[1].defaultVoltVal = ER_CV_VOLTAGE_MAX;


	inputOutputDescriptors[4].enabled = true;
	inputOutputDescriptors[4].augments[0].baseColor = MathExtras::Color::RED;
	inputOutputDescriptors[4].augments[0].signalBehalvior = EuroRack::SignalBehalvior_NormalLinear;
	inputOutputDescriptors[4].augments[0].signalType = EuroRack::SignalType_CV;



	inputOutputDescriptors[5].enabled = true;
	inputOutputDescriptors[5].numAugments = 3;
	inputOutputDescriptors[5].augments[0].baseColor = MathExtras::Color::CYAN;
	inputOutputDescriptors[5].augments[1].baseColor = MathExtras::Color::CYAN;
	inputOutputDescriptors[5].augments[2].baseColor = MathExtras::Color::CYAN;
	inputOutputDescriptors[5].augments[0].signalBehalvior = EuroRack::SignalBehalvior_NormalLinear_Inverted;
	inputOutputDescriptors[5].augments[1].signalBehalvior = EuroRack::SignalBehalvior_NormalLinear_Inverted;
	inputOutputDescriptors[5].augments[2].signalBehalvior = EuroRack::SignalBehalvior_NormalLinear;
	inputOutputDescriptors[5].augments[2].signalType = EuroRack::SignalType_GATE;


	inputOutputDescriptors[6].enabled = true;
	inputOutputDescriptors[6].augments[0].baseColor = MathExtras::Color::BLUE;
	inputOutputDescriptors[6].augments[1].baseColor = MathExtras::Color::BLUE;
	inputOutputDescriptors[6].augments[2].baseColor = MathExtras::Color::BLUE;
	inputOutputDescriptors[6].numAugments = 3;

	inputOutputDescriptors[7].enabled = true;
	inputOutputDescriptors[7].augments[0].baseColor = MathExtras::Color::MAGENTA;



	recordingPlaybackTriggerL.SetThreshold(ER_GATE_THRESH_VOLT);
	recordingPlaybackTriggerL.SetThresholdHalfGap(ER_GATE_HALF_HIST_VOLT);




	bpmOutPhaseFilter.mHalfGapPerc = 0.05f;
	bpmOutPhaseFilter.SetCeilings(1.0f, 0.0f);
	bpmOutPhaseFilter.mNoFilter = true;
	bpmOutPhaseFilter.mGapTravel = true;
	//bpmOutPhaseFilter.filterCoeff = 100.0f;
	bpmOutTrigger.SetThreshold(0.01f);
	bpmOutTrigger.SetThresholdHalfGap(0);



	knobAngleOutPhaseFilter.mHalfGapPerc = 0.01f;
	knobAngleOutPhaseFilter.SetCeilings(1.0f, 0.0f);
	knobAngleOutPhaseFilter.filterCoeff = 100.0f;

	knobAngleOutTrigger.SetThreshold(0.01f);
	knobAngleOutTrigger.SetThresholdHalfGap(0);

	tableDirTrigger.Initialize(ER_GATE_THRESH_VOLT*0.5f, -ER_GATE_THRESH_VOLT*0.5f, 0.1f);


	channelInterpEnables[0] = 0;
	channelInterpEnables[1] = 1;
	channelInterpEnables[2] = 1;
	channelInterpEnables[3] = 1;


	//audioBuffer.SetSDStartAddress(0);

}

//inline void Mode_Torque1::UnInitialize() {
//}



inline void Mode_TurnTable::UpdateLEDS(float sampleTime) {


	WS2812_SETALLLED(0,0,0);

	//basic led scheme
	float angleLEDRing = MathExtras::WrapMax<double>(-tableTargetAngle, 1.0f);





	//LEDManager.SetLEDOuterRing_Float(angleLEDRing,  0,0,LED_BASE_BRIGHTNESS_255);

	//Rotating Grid With Center led blue for reference
	const int inc = LED_NUM_LEDS_PER_RING/(float(marksPerTable-1));
	int i = 0;
	for(int tick = angleLEDRing*LED_NUM_LEDS_PER_RING; tick <=  angleLEDRing*LED_NUM_LEDS_PER_RING + LED_NUM_LEDS_PER_RING; tick += inc)
	{
		if(i == 0)
		{
			LEDManager.SetLEDOuterRing_Int(tick,  0, 0 ,LED_BASE_BRIGHTNESS_255);
			if((tick+1)%LED_NUM_LEDS_PER_RING == 0)
			{

			}

		}
		else
		{
			LEDManager.SetLEDOuterRing_Int(tick,  0, LED_BASE_BRIGHTNESS_255, LED_BASE_BRIGHTNESS_255);



		}
		i++;
	}


	//Tape visualization
	int start = angleLEDRing*LED_NUM_LEDS_PER_RING;
	for(int tick =start; tick <= start + LED_NUM_LEDS_PER_RING; tick += 1)
	{


		int wrappedTick = MathExtras::WrapMax(tick, LED_NUM_LEDS_PER_RING);


		if(tick == start)
		{
			if(!recordingPlaybackTriggerL.TriggerLevel())
				tapeVisLEDS[wrappedTick] = 1.0f;

		}

		//decrement value of all visleds
		tapeVisLEDS[wrappedTick] = MathExtras::ClampInclusive(tapeVisLEDS[wrappedTick]-0.01f, 0.0f, 1.0f);


		float actuallyRecording = float(int(ADCPluggedIn(0)));


		//calculate variance of all data in "Tick Bounds" use that to get a color.
		int remap = MathExtras::WrapMax(-(wrappedTick - start), LED_NUM_LEDS_PER_RING);
		LEDManager.SetLEDInnerRing_Int(remap,
					LED_BASE_BRIGHTNESS_255,
					actuallyRecording*tapeVisLEDS[wrappedTick]*0.75*LED_BASE_BRIGHTNESS_255,
					0);
	}




	LEDManager.SetLEDOuterRing_Int(0,
			inputOutputDescriptors[2].augments[1].baseColor.r_ *LED_BASE_BRIGHTNESS_255,
			inputOutputDescriptors[2].augments[1].baseColor.g_ *LED_BASE_BRIGHTNESS_255,
			inputOutputDescriptors[2].augments[1].baseColor.b_ *LED_BASE_BRIGHTNESS_255);
	LEDManager.SetLEDInnerRing_Int(0,
			inputOutputDescriptors[2].augments[1].baseColor.r_*LED_BASE_BRIGHTNESS_255,
			inputOutputDescriptors[2].augments[1].baseColor.g_*LED_BASE_BRIGHTNESS_255,
			inputOutputDescriptors[2].augments[1].baseColor.b_ *LED_BASE_BRIGHTNESS_255);




	//if(centerPass)
	//	LEDManager.SetKnobCenterLEDS(0, LED_BASE_BRIGHTNESS_255, 0);

	StandardPanelLEDDraw();
}

inline void Mode_TurnTable::AudioDSPFunction(float sampleTime, int bufferSwap) {
	float positionKnobSpace = -tableTargetAngle;
	float localPositionKnobSpace = MathExtras::WrapMax(positionKnobSpace, 1.0f/(marksPerTable));


	if(inputOutputDescriptors[6].curAugment == 0)
	{
		knobAngleOutPhaseFilter.Process(MathExtras::WrapMax(positionKnobSpace,1.0f), sampleTime);
		// ramp
		SetDacVolts(2, (knobAngleOutPhaseFilter.val_filtered-0.5f)*EuroRack::SignalVoltRange(inputOutputDescriptors[6].CurAugment().signalType));
	}
	else if(inputOutputDescriptors[6].curAugment == 1)
	{
		knobAngleOutPhaseFilter.Process(MathExtras::WrapMax(positionKnobSpace,1.0f), sampleTime);
		//sin of angle
		SetDacVolts(2, arm_sin_f32((knobAngleOutPhaseFilter.val_filtered*M_TWOPI))*0.5f*EuroRack::SignalVoltRange(inputOutputDescriptors[6].CurAugment().signalType));
	}
	else if(inputOutputDescriptors[6].curAugment == 2)
	{
		knobAngleOutTrigger.Process(MathExtras::WrapMax(positionKnobSpace,1.0f));
		//gate pulse
		SetDacVolts(2, (1-knobAngleOutTrigger.TriggerLevel())*EuroRack::SignalVoltRange(inputOutputDescriptors[6].CurAugment().signalType));
	}

	//bpm angle
	//percentage between 2 marks
	if(inputOutputDescriptors[5].curAugment == 0)
	{
		bpmOutPhaseFilter.Process(localPositionKnobSpace*marksPerTable, sampleTime);
		// ramp
		SetDacVolts(1, (localPositionKnobSpace*marksPerTable-0.5f)*EuroRack::SignalVoltRange(inputOutputDescriptors[5].CurAugment().signalType));
	}
	else if(inputOutputDescriptors[5].curAugment == 1)
	{
		bpmOutPhaseFilter.Process(localPositionKnobSpace*marksPerTable, sampleTime);
		//sin of angle
		SetDacVolts(1, arm_sin_f32((localPositionKnobSpace*marksPerTable*M_TWOPI))*0.5f*EuroRack::SignalVoltRange(inputOutputDescriptors[5].CurAugment().signalType));
	}
	else if(inputOutputDescriptors[5].curAugment == 2)
	{
		bpmOutTrigger.Process(localPositionKnobSpace*marksPerTable);
		if(bpmOutTrigger.TriggerLevel())
			SetDacVolts(1, EuroRack::SignalVoltMin(inputOutputDescriptors[5].CurAugment().signalType));
		else
			SetDacVolts(1, EuroRack::SignalVoltMax(inputOutputDescriptors[5].CurAugment().signalType));
	}



	if(inputOutputDescriptors[2].curAugment == 1)
		recordingPlaybackTriggerL.Process(adcVolts[2]);
	else
		recordingPlaybackTriggerL.Process(inputOutputDescriptors[2].augments[1].defaultVoltVal);

	if(recordingPlaybackTriggerL.TriggeredUp())
		switchedToPlayModeFlag = true;


	double angleStart = MathExtras::WrapMax(tableTargetAngleFiltered_1  ,1.0);
	double angleEnd =   MathExtras::WrapMax(tableTargetAngleFiltered    ,1.0);
	double diff = MathExtras::WrappedLocalDifference<double>(angleEnd, angleStart, 1.0);
	const float signalInputHeadroom = 0.05f;
	int dir = MathExtras::Sign(diff);
	if(!dir) dir = 1;
	if(!recordingPlaybackTriggerL.TriggerLevel())
	{
		float recordIntensity = MathExtras::ClampInclusive(1.0f-(recordingPlaybackTriggerL.in / recordingPlaybackTriggerL.thresholdVal), 0.0f, 1.0f);
		float keepIntensity =  MathExtras::ClampInclusive((recordingPlaybackTriggerL.in / (recordingPlaybackTriggerL.thresholdVal*0.5f)), 0.0f, 1.0f);


			BLOCK_PROCESS_CHANNELS_0_2()

				wrapPerc_1 = wrapPerc;
				double tmp = angleStart + diff*blockPerc;
				wrapPerc = MathExtras::WrapOnceMaxOne<double>(tmp);
				wrapPerc = MathExtras::WrapOnceMaxOne<double>(wrapPerc);//sometimes needed to resolve 1 to 0

				float smp;
				if(ADCPluggedIn(0))
				{
					smp = signalInputHeadroom*(adc0DMA[adc0Idx] << 8)/float(INT32_MAX-1);
				}
				float smpHPF = recordHPF.Process(smp, SAMPLETIME);

				float mixed_smp = signalBuffer.Inprint(smpHPF, smp_1, wrapPerc, wrapPerc_1, dir, 1.0f, keepIntensity, 0.6f);
				smp_1 = smpHPF;



				dac0DMA[sampleIdxDMA] = -(int32_t(mixed_smp*(1.0f/signalInputHeadroom)*(INT32_MAX-1)) >> 8);
			}



		dacVolts[0] = adcVolts[0];




		//set volts mainly for lights
		SetDacVolts(0, adcVolts[0]);
	}
	else
	{

		//playback
		BLOCK_PROCESS_CHANNELS_0_2()
			wrapPerc_1 = wrapPerc;
			wrapPerc = MathExtras::WrapMax<float>(angleStart + diff*blockPerc, 1.0f);
			int32_t smp = signalBuffer.ReadRecordBufferAtPerc(wrapPerc, wrapPerc_1, dir)*(1.0f/signalInputHeadroom)*(INT32_MAX-1);
			dac0DMA[sampleIdxDMA] = -(smp >> 8);
		}
		//set volts mainly for lights
		SetDacVolts(0, DacToVolts(0, signalBuffer.ReadRecordBufferAtPerc(wrapPerc, wrapPerc_1, dir)));

	}

	SetDacVolts(3, -MotorAngleToVolts(LocalMotorAngleState.currentAngularVelFiltered2/MAX_RPM_FULL_TORQUE));
}

inline void Mode_TurnTable::KnobDSPFunction(float sampleTime)
{

	double motorAccumAngle = MotorAngleStateGetCurrentAccumulatedAngled(&LocalMotorAngleState);
	double angleError = tableTargetAngle - motorAccumAngle;

	float positionKnobSpace = -tableTargetAngle;
	float localPositionKnobSpace = MathExtras::WrapMax(positionKnobSpace, 1.0f/(marksPerTable));


	//adjust weighting for maintaining target angle vs backing target to where knob actually is
	if(MathExtras::Abs(angleError) > 0.01f)
	{
		W1 += (1.0f - W1)*sampleTime*10.0f;//increase angle maintian weight
	}
	else
	{
		W1 += (0.0f - W1)*sampleTime*100.0f;//fall down faster once weight if found.

	}
	//W1 = 0.0f;//don't angle track in favor of stable speeds
	W2 = 1.0f - W1;


	float newtableTargetSpeed;
	if(ADCPluggedIn(3) && inputOutputDescriptors[3].curAugment == 0)
	{
		newtableTargetSpeed = tableTurnDir*tempo.bps_filtered/marksPerTable;
		//TODO Add extra term to correct for beat offset, syncing incoming bpm with table bpm.
//		if(tempo.tapping && tableTurnDir != 0)
//		{
//
//			beatProximity = localPositionKnobSpace*marksPerTable;
//			if(tableTurnDir == -1)
//			{
//				beatProximity = 1 - beatProximity;
//			}
//
//			beatOffsetError = tempo.PercToNextTap() - beatProximity;
//
//			//if(int(MathExtras::Sign(beatOffsetError)) != tableTurnDir)
//			//	beatOffsetError = 0.0f;
//
//			newtableTargetSpeed += beatOffsetError*0.01f;
//		}
	}
	else if(ADCPluggedIn(3) && !adcFakePlugStates[3] && inputOutputDescriptors[3].curAugment == 1)
		newtableTargetSpeed = (tableTurnDir*adcVolts[3])/ER_CV_VOLTAGE_MAX;
	else
	{

		if(startupSampleDelayCount <= 0)
		{
			newtableTargetSpeed = LocalMotorAngleState.currentAngularVelFiltered2;
		}
		else
		{
			startupSampleDelayCount--;
			newtableTargetSpeed = tableTargetSpeed;
		}
	}

	tableTargetSpeed = newtableTargetSpeed;

	//only actual update target speed if the table is tracking speed well.
	if(  (MathExtras::Abs(newtableTargetSpeed - tableTargetSpeed) ) > 0.25f)
	{
		speedCount += sampleTime;
		if(speedCount > 0.1f){
			tableTargetSpeed = newtableTargetSpeed;
			speedCount = 0;
		}
	}
//
//
//
	if(MathExtras::Abs(newtableTargetSpeed) < 0.01f)
	{
		stillCount += sampleTime;
		if(stillCount > 0.1f)
		{
			newtableTargetSpeed = 0;
			tableTargetSpeed = 0;
			stillCount= 0;
		}
	}

	//tableTargetAngle based on dynamics
	tableTargetAngle_1 = tableTargetAngle;
	tableTargetAngleFiltered_1 = tableTargetAngleFiltered;

	double speedTrackTerm = tableTargetSpeed*sampleTime*W2;

	double angleError2 = (motorAccumAngle - tableTargetAngle);
	double angleTrackTerm = angleError2*W1*100.0f*sampleTime;

	//if(MathExtras::Abs(angleError2) < 0.2f )
	//	angleTrackTerm = 0;

	tableTargetAngle += speedTrackTerm + angleTrackTerm;


	tableTargetAngleFiltered += (tableTargetAngle - tableTargetAngleFiltered)*sampleTime*10.0f;






	//drive to maintain target angle
	float drive = (tableTargetAngle - motorAccumAngle)*80.0f;
	driveTorque = drive;
	MasterProcess_Torque(sampleTime);
}



void Mode_TurnTable::MainThreadUpdateFunction(float sampleTime)
{
	Mode::MainThreadUpdateFunction(sampleTime);

	//printf("%f, %f\r\n", beatProximity, beatOffsetError);


	if(!recordingPlaybackTriggerL.TriggerLevel())
	{
		if(recordingPlaybackTriggerL.in > recordingPlaybackTriggerL.thresholdVal*0.5f)
		{
			inputOutputDescriptors[2].augments[1].baseColor = MathExtras::Color::YELLOW;
		}
		else
		{
			inputOutputDescriptors[2].augments[1].baseColor = MathExtras::Color::ORANGE;
		}

	}
	else
	{
		inputOutputDescriptors[2].augments[1].baseColor = MathExtras::Color::BLUE;
	}


	//process tableturn direction
	tableDirTrigger.Process(adcVolts[1]);


	if(tableDirTrigger.TriggerRegion() == 2)
	{
		tableTurnDir = -1;
		inputOutputDescriptors[1].augments[0].baseColor = MathExtras::Color::GREEN;
	}
	else if(tableDirTrigger.TriggerRegion() == 1)
	{
		tableTurnDir = 0;
		inputOutputDescriptors[1].augments[0].baseColor = MathExtras::Color::RED;
	}
	else if(tableDirTrigger.TriggerRegion() == 0)
	{
		tableTurnDir = 1;
		inputOutputDescriptors[1].augments[0].baseColor = MathExtras::Color::BLUE;
	}


	//save signbuffer if needed
	if(switchedToPlayModeFlag)
	{
		SaveAudioBuffer("Scrub");
		switchedToPlayModeFlag = false;
	}





	//make save timer run routinely.
	if(!saveTimerEn)
		RestartSaveTimer();




}

void Mode_TurnTable::SaveAudioBuffer(char* fileNameStr)
{
	FRESULT res;
	if(audioFile.obj.lockid)//if already opened.
	{

	}
	else
	{
		res = f_open(&audioFile, fileNameStr, FA_CREATE_ALWAYS | FA_OPEN_EXISTING | FA_WRITE | FA_READ);


		if(res != FR_OK)
			Error_Handler();
	}



	res = f_lseek(&audioFile, 0);
	if(res != FR_OK)
		Error_Handler();
	uint32_t byteswritten;
	res = f_write(&audioFile, (char*)signalBuffer.sampleBuffer, DJ_REC_BUFF_NUM_SAMPLES*sizeof(float), (UINT*)&byteswritten);
	if(res != FR_OK)
		Error_Handler();

	res = f_sync(&audioFile);
	if(res != FR_OK)
		Error_Handler();

	if((byteswritten > 0) && (res == FR_OK))
	{
	}
	else
	{
		strcpy(errorCode, "Mode_TurnTable::signalBuffer Write Error.");
		Error_Handler();
	}

	res = f_close(&audioFile);
	audioFile.obj.lockid = 0;
	if(res != FR_OK)
		Error_Handler();

}

void Mode_TurnTable::OnSaveTimerTimeout()
{
	Mode::OnSaveTimerTimeout();


	//save targetspeed if differs from saved targetspeed by threshold.
	if(MathExtras::Abs(tableTargetSpeed - tableTargetSpeedSaved) > 0.1f)
	{
		tableTargetSpeedSaved = tableTargetSpeed;
		ee24_write_float(modeManager.CurrentModeEEPromBase()+EEPromAddressOffsets(2), tableTargetSpeed, 1000);
	}

	if((recordingPlaybackTriggerL.in > recordingPlaybackTriggerL.thresholdVal*0.5f) && (recordingPlaybackTriggerL.TriggerLevel() == 0))
	{
		SaveAudioBuffer("Scrub");
	}

}



void Mode_TurnTable::DebugPrint()
{
	Mode::DebugPrint();
}
void Mode_TurnTable::ButtonUp(int button)
{
	Mode::ButtonUp(button);
}

void  Mode_TurnTable::OnFuncCombo(int button)
{
	Mode::OnFuncCombo(button);


	if(button == 1)
	{
		FlipTurnDir();
	}
	if(button == 0)
	{
		if(recordingPlaybackTriggerL.TriggerLevel() > 0)
		{

			//get
			ee24_read_32(modeManager.CurrentModeEEPromBase()+EEPromAddressOffsets(3), &saveNumber, 1000);


			char str[32] = "SAVED_SCRUB_";
			itoa(saveNumber,str+12,10);
			SaveAudioBuffer(str);
			saveNumber++;
			ee24_write_32(modeManager.CurrentModeEEPromBase()+EEPromAddressOffsets(3), saveNumber, 1000);

		}
	}
}

void Mode_TurnTable::FlipTurnDir()
{
	float defaultV = inputOutputDescriptors[1].augments[0].defaultVoltVal;

	if(defaultV > ER_GATE_THRESH_VOLT*0.5f)
	{
		defaultV = -ER_GATE_THRESH_VOLT;
	}
	else if(defaultV > -ER_GATE_THRESH_VOLT*0.5f)
	{
		defaultV = ER_GATE_THRESH_VOLT;
	}
	else if(defaultV < -ER_GATE_THRESH_VOLT*0.5f)
	{
		defaultV = 0;
	}
	inputOutputDescriptors[1].augments[0].defaultVoltVal = defaultV;
	ee24_write_float(modeManager.CurrentModeEEPromBase()+EEPromAddressOffsets(1), inputOutputDescriptors[1].augments[0].defaultVoltVal, 1000);
}

void Mode_TurnTable::OnButtonQuickAction(int button)
{
	Mode::OnButtonQuickAction(button);

	if(button == 2 && inputOutputDescriptors[2].curAugment == 1 && !ADCPluggedIn(2))
	{
		if(recordingPlaybackTriggerL.triggerLevel == 0)
		{
			inputOutputDescriptors[2].augments[1].defaultVoltVal = recordingPlaybackTriggerL.thresholdVal + recordingPlaybackTriggerL.thresholdHalfGap*2;//go to play
		}
		else
		{
			inputOutputDescriptors[2].augments[1].defaultVoltVal = recordingPlaybackTriggerL.thresholdVal*0.75f ; //go to OVERDUB
		}


		ee24_write_float(modeManager.CurrentModeEEPromBase()+EEPromAddressOffsets(0), inputOutputDescriptors[2].augments[1].defaultVoltVal, 1000);
	}
	if(button == 1 && !ADCPluggedIn(1))
	{
		FlipTurnDir();
	}




}

void Mode_TurnTable::OnButtonClickedLongHold(int button)
{
	Mode::OnButtonClickedLongHold(button);

	if(button == 2 && inputOutputDescriptors[2].curAugment == 1 && !ADCPluggedIn(2))
	{
		inputOutputDescriptors[2].augments[1].defaultVoltVal = 0.0f;//go to REC OVER
	}

}


void Mode_TurnTable::OnPostCycleAugment(int buttonIdx, int augmentIdx)
{
	Mode::OnPostCycleAugment(buttonIdx, augmentIdx);
}



void Mode_TurnTable::OnGestureLoop()
{
	Mode::OnGestureLoop();

	ResetTableTargetAndFilters();
}

void Mode_TurnTable::OnGesturePlay()
{
	Mode::OnGesturePlay();

	ResetTableTargetAndFilters();
}


void Mode_TurnTable::ResetTableTargetAndFilters()
{
		//reset tableTargetAngle and Filters
	tableTargetAngle = MotorAngleStateGetCurrentAccumulatedAngled(&LocalMotorAngleState);
	tableTargetAngleFiltered = tableTargetAngle;
	tableTargetAngleFiltered_1 = tableTargetAngle;

}



//writes the entire state of the mode to eeprom
bool Mode_TurnTable::WriteEEPROMState(uint32_t &ee_address)
{
	bool s = Mode::WriteEEPROMState(ee_address);
	s &= ee24_write_float(ee_address, inputOutputDescriptors[2].augments[1].defaultVoltVal, 1000);
	ee_address+=sizeof(float);

	s &= ee24_write_float(ee_address, inputOutputDescriptors[1].augments[0].defaultVoltVal, 1000);
	ee_address+=sizeof(float);

	s &= ee24_write_float(ee_address, tableTargetSpeed, 1000);
	ee_address+=sizeof(float);

	s &= ee24_write_32(ee_address, saveNumber, 1000);
	ee_address+=sizeof(uint32_t);


	return s;
}



//reads the entire state of mode from eeprom immediately.
bool Mode_TurnTable::ReadEEPROMState(uint32_t &ee_address)
{
	bool s = Mode::ReadEEPROMState(ee_address);
	s &= ee24_read_float(ee_address, &inputOutputDescriptors[2].augments[1].defaultVoltVal, 1000);
	//always read back to playback because recording on startup is never good.
	inputOutputDescriptors[2].augments[1].defaultVoltVal = recordingPlaybackTriggerL.thresholdVal + recordingPlaybackTriggerL.thresholdHalfGap*2;
	ee_address+=sizeof(float);

	s &= ee24_read_float(ee_address, &inputOutputDescriptors[1].augments[0].defaultVoltVal, 1000);
	ee_address+=sizeof(float);

	s &= ee24_read_float(ee_address, &tableTargetSpeedSaved, 1000);
	tableTargetSpeed = tableTargetSpeedSaved;
	ee_address+=sizeof(float);


	s &= ee24_read_32(ee_address, &saveNumber, 1000);
	if(saveNumber == 0)//uninitialized case
		saveNumber = 1;

	ee_address+=sizeof(float);

	return s;
}



bool Mode_TurnTable::ReadSDFileData()
{
	Mode::ReadSDFileData();
	FRESULT res;
	if(audioFile.obj.lockid)//if already opened.
	{
		Error_Handler();
	}
	else
	{
		res = f_open(&audioFile, "TTable",  FA_OPEN_EXISTING | FA_WRITE | FA_READ);
		if(res != FR_OK)
		{
			if(res == FR_NO_FILE)
				return false;

			Error_Handler();
		}

		uint32_t bytesread;
		res = f_read(&audioFile, (char*)signalBuffer.sampleBuffer, DJ_REC_BUFF_NUM_SAMPLES*sizeof(float), (UINT*)&bytesread);
		if(res != FR_OK)
			Error_Handler();

		if((bytesread > 0) && (res == FR_OK))
		{

		}
		else
		{
			Error_Handler();
		}

		return true;


	}



}
