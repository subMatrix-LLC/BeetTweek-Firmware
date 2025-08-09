/*
 * Mode.cpp
 *
 *  Created on: Nov 19, 2020
 *      Author: casht
 */

#include <clockinput.h>
#include <MathExtras_Color.h>
#include "spi.h"


#include "Modes/Mode.h"

#include "DualBoardComHandler.h"

#include "EuroRack.h"

#include "tim.h"

#include "MotorController.h"
#include "sai.h"


extern ModeManager modeManager;
extern MotorController motorController;


FIL Mode::gestureFile;
DoubleBuffer_FileBacked<Mode::GESTUREDATA, KNOB_GESTURE_DOUBLE_PAGE_SIZE> Mode::gestureBuffer = DoubleBuffer_FileBacked<Mode::GESTUREDATA, KNOB_GESTURE_DOUBLE_PAGE_SIZE>();
uint32_t Mode::knobGestureSampleStretch = KNOB_GESTURE_SAMPLE_SKIP;
float Mode::knobGestureRecordBPS = 1;
bool Mode::saveKnobGestureRecordBPS = false;
MotorAngleState Mode::knobAngleGestureInitial;
double Mode::knobAngleGesturePlaybackLatest = 0.0f;
bool Mode::gestureSaveRecInfo = false;
Mode::GUESTURE_LEVEL Mode::curGestureLevel = GUESTURE_LEVEL_PLAY_RST;
uint32_t Mode::gestureRecEndIdx = 0;
/*
MathExtras::PID<float> Mode::antiCoggPID;
Mode::CalibrationMode Mode::calibrationMode = Mode::CalibrationMode_None;
int Mode::calibrationDelayCountdown = 0;
bool Mode::coggDriveLookupCW_SMPLCMPL = false;//all samples acquired
bool Mode::coggDriveLookupCCW_SMPLCMPL = false;//all samples acquired
float Mode::coggDriveLookupCW[ANTCOGGPOINTS] = {0.0f};
float Mode::coggDriveLookupCCW[ANTCOGGPOINTS] = {0.0f};
int Mode::coggDriveLookupCW_SMPLCNT[ANTCOGGPOINTS] = {0};
int Mode::coggDriveLookupCCW_SMPLCNT[ANTCOGGPOINTS] = {0};
*/
float Mode::frictionCalFactor = 0.8f;
float Mode::drivePowerFactor = DRIVEPOWERFACTOR_DEF;
float Mode::driveOffset = DRIVEOFFSET_DEF;

bool Mode::bypassGuestureInput = false;

Mode::Mode() {

	strcpy(modeName, "Standard Mode");
	strcpy(modeDescription,"Some Description");

	MotorAngleStateInit(&LocalMotorAngleState);
}

Mode::~Mode() {
	// TODO Auto-generated destructor stub
}



//do setup things when the mode is entered
void Mode::Initialize()
{
	firstLEDFrame = true;



	gestTrgtLevelTrigger.Initialize(ER_CV_VOLTAGE_MAX*2, ER_CV_VOLTAGE_MIN*2, ER_GATE_HALF_HIST_VOLT, ER_VOLTAGE_OUT_MIN, ER_VOLTAGE_OUT_MAX);

	//initialize all descriptors to struct defaults to clear state.
	for(int i = 0; i < 8; i++)
		inputOutputDescriptors[i] = InputOutputDescriptor();



	inputOutputDescriptors[0].enabled = false;
	inputOutputDescriptors[0].buttonIdx = 0;
	inputOutputDescriptors[0].channelIdx = 0;
	inputOutputDescriptors[0].numAugments = 1;
	inputOutputDescriptors[0].augments[0].defaultVoltVal = 0.0f;

	inputOutputDescriptors[1].enabled = false;
	inputOutputDescriptors[1].buttonIdx = 1;
	inputOutputDescriptors[1].channelIdx = 1;
	inputOutputDescriptors[1].numAugments = 1;
	inputOutputDescriptors[1].augments[0].defaultVoltVal = 0.0f;



	inputOutputDescriptors[2].enabled = true;
	inputOutputDescriptors[2].quickAction = true;
	inputOutputDescriptors[2].buttonIdx = 2;
	inputOutputDescriptors[2].channelIdx = 2;
	inputOutputDescriptors[2].curAugment = 0;
	inputOutputDescriptors[2].numAugments = 1;
	inputOutputDescriptors[2].augments[0].signalBehalvior = EuroRack::SignalBehalvior_NormalLinear_Inverted;
	inputOutputDescriptors[2].augments[0].signalType = EuroRack::SignalType_GATE;
	inputOutputDescriptors[2].augments[0].baseColor = MathExtras::Color::RED;
	inputOutputDescriptors[2].augments[0].useCustomColorIntensity = true;
	inputOutputDescriptors[2].augments[0].defaultVoltVal = gestTrgtLevelTrigger.RegionMid(GUESTURE_LEVEL_PLAY_RST);

	inputOutputDescriptors[3].enabled = true;
	inputOutputDescriptors[3].buttonIdx = 3;
	inputOutputDescriptors[3].channelIdx = 3;
	inputOutputDescriptors[3].numAugments = 1;
	inputOutputDescriptors[3].quickAction = true;
	inputOutputDescriptors[3].augments[0].signalBehalvior = EuroRack::SignalBehalvior_NormalLinear;
	inputOutputDescriptors[3].augments[0].signalType = EuroRack::SignalType_GATE;
	inputOutputDescriptors[3].augments[0].baseColor = MathExtras::Color::YELLOW;
	inputOutputDescriptors[3].augments[0].gateOffIntensity = 0.1f;
	inputOutputDescriptors[3].augments[0].gateTrigger = &tempo.trigger;





	inputOutputDescriptors[4].enabled = false;
	inputOutputDescriptors[4].numAugments = 1;
	inputOutputDescriptors[4].buttonIdx = 4;
	inputOutputDescriptors[4].channelIdx = 0;
	inputOutputDescriptors[4].augments[0].signalBehalvior = EuroRack::SignalBehalvior_NormalLinear;
	inputOutputDescriptors[4].augments[0].signalType = EuroRack::SignalType_CV;
	inputOutputDescriptors[4].augments[0].baseColor = MathExtras::Color::YELLOW;
	inputOutputDescriptors[4].augments[0].useCustomColorIntensity = false;


	inputOutputDescriptors[5].enabled = false;
	inputOutputDescriptors[5].numAugments = 1;
	inputOutputDescriptors[5].buttonIdx = 5;
	inputOutputDescriptors[5].channelIdx = 1;
	inputOutputDescriptors[5].augments[0].signalBehalvior = EuroRack::SignalBehalvior_NormalLinear;
	inputOutputDescriptors[5].augments[0].signalType = EuroRack::SignalType_CV;
	inputOutputDescriptors[5].augments[0].baseColor = MathExtras::Color::YELLOW;


	inputOutputDescriptors[6].enabled = false;
	inputOutputDescriptors[6].numAugments = 1;
	inputOutputDescriptors[6].buttonIdx = 6;
	inputOutputDescriptors[6].channelIdx = 2;
	inputOutputDescriptors[6].augments[0].signalBehalvior = EuroRack::SignalBehalvior_NormalLinear;
	inputOutputDescriptors[6].augments[0].signalType = EuroRack::SignalType_CV;
	inputOutputDescriptors[6].augments[0].baseColor = MathExtras::Color::YELLOW;


	inputOutputDescriptors[7].enabled = false;
	inputOutputDescriptors[7].numAugments = 1;
	inputOutputDescriptors[7].buttonIdx   = 7;
	inputOutputDescriptors[7].channelIdx  = 3;
	inputOutputDescriptors[7].augments[0].signalBehalvior = EuroRack::SignalBehalvior_NormalLinear;
	inputOutputDescriptors[7].augments[0].signalType = EuroRack::SignalType_CV;
	inputOutputDescriptors[7].augments[0].baseColor = MathExtras::Color::YELLOW;



	virtualAngleOffset = GlobalMotorAngleState.currentAngle - LocalMotorAngleState.currentAngle;

	//process the gesture trigger a few times to get rid of initial state machine transitions.
	gestTrgtLevelTrigger.Process(inputOutputDescriptors[2].augments[0].defaultVoltVal);
	gestTrgtLevelTrigger.Process(inputOutputDescriptors[2].augments[0].defaultVoltVal);


	tempo.trigger.SetThreshold(ER_GATE_THRESH_VOLT);
	tempo.trigger.SetThresholdHalfGap(ER_GATE_HALF_HIST_VOLT);



	channelInterpEnables[0] = 1;
	channelInterpEnables[1] = 1;
	channelInterpEnables[2] = 1;
	channelInterpEnables[3] = 1;

}

//un-setup thing in prep for next mode
void Mode::UnInitialize()
{

}

//updates the front panel
void Mode::UpdateLEDS(float sampleTime)
{
	WS2812_SETALLLED(0,0,0);

	//basic led scheme
	float angleLEDRing = 1.0f - LocalMotorAngleState.currentAngle;
	float targetAngleLEDRing = 1.0f - (MotorSpiOutData_Latest/65535.0f);

	LEDManager.SetLEDOuterRing_Float(angleLEDRing,  0,0,LED_BASE_BRIGHTNESS_255);
	LEDManager.SetLEDInnerRing_Float(targetAngleLEDRing,  LED_BASE_BRIGHTNESS_255,0,0);
	LEDManager.SetLEDOuterRing_Int(0,  LED_BASE_BRIGHTNESS_255,LED_BASE_BRIGHTNESS_255,LED_BASE_BRIGHTNESS_255);
}


void Mode::PostLEDUpdate(float sampleTime)
{
	//Gesture record/play indication
	LEDManager.SetKnobCenterLEDS(
			inputOutputDescriptors[2].augments[0].baseColor.r_*LED_BASE_BRIGHTNESS_255,
			inputOutputDescriptors[2].augments[0].baseColor.g_*LED_BASE_BRIGHTNESS_255,
			inputOutputDescriptors[2].augments[0].baseColor.b_*LED_BASE_BRIGHTNESS_255);


	//Indicate Func mode by shading over current colors.
	if(funcMode)
	{
		static float time = 0;
		time+=sampleTime;
		time = MathExtras::WrapMax(time, M_TWO_PI);
		float blink  = arm_sin_f32(time*M_TWO_PI)*0.24f + 0.75f;

		MathExtras::Color color = MathExtras::Color::ORANGE;

		if(modeManager.funcPressFirmwareCounter == 5)
		{
			color = MathExtras::Color::WHITE;
		}

		color.SetLightness(blink);

		LEDManager.SetLEDRingRangeLinear_Float(LEDPanelManager::RINGIDENTIFIER_OUTER, 0, 1.0f, (MathExtras::Color*)&color, (MathExtras::Color*)&color, 1.0f);
		LEDManager.SetLEDRingRangeLinear_Float(LEDPanelManager::RINGIDENTIFIER_INNER, 0, 1.0f, (MathExtras::Color*)&color, (MathExtras::Color*)&color, 1.0f);


		//blink the buttons that have a func combo to press
		for(int i = 0; i < 8; i++)
		{
			if(inputOutputDescriptors[i].funcCombo)
			{
				LEDManager.SetLEDButton(i, color.r_*LED_BASE_BRIGHTNESS_255,  color.g_*LED_BASE_BRIGHTNESS_255,  color.b_*LED_BASE_BRIGHTNESS_255);
			}
		}
	}


}

void Mode::StandardPanelLEDDraw()
{
	//inputs and outputs.
	for(int i = 0; i < 8; i++)
	{
		InputOutputDescriptor* desc = &inputOutputDescriptors[i];

		if(desc->enabled)
			DrawLEDButtonStandard(&inputOutputDescriptors[i]);
	}


}

void Mode::DrawLEDButtonStandard(InputOutputDescriptor* desc)
{

	//value [-1,1] where -1 corresponds to voltageLOW, 1 corresponds to voltageHIGH
	float intensity = desc->CurAugment().customColorIntensity;
	float volts = 0.0f;

	if(!desc->CurAugment().useCustomColorIntensity) {

		if(desc->buttonIdx < 4)
		{
			volts = adcVolts[desc->buttonIdx];
			if(!ADCPluggedIn(desc->buttonIdx))
				intensity = 0.2f;
			else
				intensity =  volts/EuroRack::SignalVoltMax(desc->augments[desc->curAugment].signalType);
		}
		else
		{
			volts = dacVolts[desc->buttonIdx-4];
			intensity =  volts/EuroRack::SignalVoltMax(desc->augments[desc->curAugment].signalType);
		}
		intensity = MathExtras::ClampInclusive(intensity, -1.0f, 1.0f);

		//if(MathExtras::Abs(intensity) < 0.3f)
		//	intensity = 0.3f;


		if(desc->augments[desc->curAugment].signalType == EuroRack::SignalType_GATE)
		{
			if(desc->augments[desc->curAugment].signalBehalvior == EuroRack::SignalBehalvior_NormalLinear_Inverted)
			{
				if(desc->augments[desc->curAugment].gateTrigger->TriggerLevel())
					intensity = desc->augments[desc->curAugment].gateOffIntensity;
				else
					intensity = 1.0f;
			}
			else
			{
				if(desc->augments[desc->curAugment].gateTrigger->TriggerLevel())
					intensity = 1.0f;
				else
					intensity = desc->augments[desc->curAugment].gateOffIntensity;
			}
		}
		else if(desc->augments[desc->curAugment].signalType == EuroRack::SignalType_VPerOct)
		{
			intensity = 1.0f;
		}
	}



	//as frequency approaches half the led update rate.  converge to showing frequency directly.
	float freqModeCrossover = MathExtras::ClampInclusive(MathExtras::Abs(desc->freq)/60.0f, 0.0f, 1.0f);

	MathExtras::Color drawColor = desc->CurAugment().baseColor;

	if(desc->augments[desc->curAugment].signalType != EuroRack::SignalType_VPerOct)
	{
		MathExtras::Vector3 colorHSV = desc->CurAugment().baseColor.ToHSV();

		colorHSV.z_ = MathExtras::Abs(intensity)*(1-freqModeCrossover) + freqModeCrossover;


		//use freqModeCrossover to smoothly switch between pos/neg color seperation. and frequency display.
		drawColor.FromHSV(MathExtras::WrapMax(colorHSV.x_
				//Magnitude term
				+ (MathExtras::Atan(intensity)*(2.0f/180.0f))*0.05f,1.0f)*(1-freqModeCrossover)

				//Freq term
				+ MathExtras::WrapMax(colorHSV.x_ + EuroRack::FREQ_TO_HUE_OFFSET(MathExtras::Abs(desc->freq)) , 1.0f)*freqModeCrossover, colorHSV.y_, colorHSV.z_);
	}
	else
	{
		drawColor.FromHSL(EuroRack::VOCT_TO_HUE(volts), 1.0f, 0.5f);
	}

	//indicate long hold with white.
	if(modeManager.buttonHoldCounts[desc->buttonIdx] > WHITE_BUTTON_HOLD_CNT)
	{
		drawColor.r_ = 1.0f;
		drawColor.g_ = 1.0f;
		drawColor.b_ = 1.0f;
	}

	LEDManager.SetLEDButton(desc->buttonIdx, drawColor.r_*LED_PANEL_BRIGHTNESS_255, drawColor.g_*LED_PANEL_BRIGHTNESS_255, drawColor.b_*LED_PANEL_BRIGHTNESS_255);
}






//Called Very Fast.  Holds DSP Code
void Mode::AudioDSPFunction(float sampleTime, int bufferSwap)
{
}


//Called Somewhat Fast.  Holds Knob Update Processing.
void Mode::KnobDSPFunction(float sampleTime)
{
	MasterProcess_Torque(sampleTime);
}

void Mode::PreAudioDSP(float sampleTime)
{

	//process clock
	if(ADCPluggedIn(3))
	{
		tempo.Process( adcVolts[3] , sampleTime);

		if(adcPlugStates[3])
		{
			recentTapBps_Counter--;
			if(recentTapBps_Counter <= 0)
			{
				recentTapBps_1 = recentTapBps;
				recentTapBps = tempo.bps_filtered;
				recentTapBps_Counter = 1000;
			}
		}
	}


	int useSync = ADCPluggedIn(3) && (inputOutputDescriptors[3].curAugment == 0);


	//process recording/playback state
	gestTrgtLevelTrigger.Process(adcVolts[2]);


	gestTrgtLevelTrigger.ClearTriggerRegionFlags();
	gestTrgtLevelTrigger.SetTriggerRegionFlags();

	//clock sync.
	int syncSig = tempo.Tapped();

	if(syncSig)
	{
		lastGestLrgIdxSync = Mode::gestureBuffer.LargeIndex();
	}

	if(tempo.Tapped())
	{
		mainThreadSyncInd++;

		tapBlinkTim = 0.1f;
		inputOutputDescriptors[3].augments[0].customColorIntensity = 1.0f;
	}
	else
	{
		tapBlinkTim = MathExtras::ClampInclusive(tapBlinkTim -= sampleTime, -1.0f, 1.0f);
		if(tapBlinkTim < 0.0f)
			inputOutputDescriptors[3].augments[0].customColorIntensity = 0.3f;
	}


	bool waitTillNextSync = true;


	if(tempo.PercToNextTap() < 0.25f)
		waitTillNextSync = false;




	if(inputOutputDescriptors[2].curAugment == 0 )
	{
		if(GuestureStateWantedTransition(GUESTURE_LEVEL_REC_RST, GUESTURE_LEVEL_REC))
		{
			if(syncSig || !useSync)
			{
				//record angle start
				knobAngleGestureInitial = LocalMotorAngleState;

				//advance
				curGestureLevel = GUESTURE_LEVEL_REC;
				OnGestureRecordStart();
			}
		}
		else if(GuestureStateWantedTransition(GUESTURE_LEVEL_REC, GUESTURE_LEVEL_REC_RST))
		{
			Mode::gestureBuffer.ResetToBeginning(WRITE_MODE);


			//advance
			curGestureLevel = GUESTURE_LEVEL_REC_RST;
		}
		else if(GuestureStateWantedTransition(GUESTURE_LEVEL_REC, GUESTURE_LEVEL_PLAY))
		{
			if(syncSig || !useSync || !waitTillNextSync)
			{
				RestartSaveTimer();
				gestureSaveRecInfo = true;

				LocalMotorAngleState = knobAngleGestureInitial;
				Mode::gestureBuffer.ResetToBeginning(READ_MODE);

				//save relevant end idx for writing.
				if(useSync && !waitTillNextSync)
					Mode::gestureBuffer.endIdx = lastGestLrgIdxSync;



				//advance
				curGestureLevel = GUESTURE_LEVEL_PLAY;
				OnGestureRecordStop();
				OnGesturePlay();

			}
		}
		else if(GuestureStateWantedTransition(GUESTURE_LEVEL_REC, GUESTURE_LEVEL_PLAY_RST))
		{
			if(syncSig || !useSync || !waitTillNextSync)
			{
				RestartSaveTimer();
				gestureSaveRecInfo = true;

				Mode::gestureBuffer.ResetToBeginning(READ_MODE);

				//save relevant end idx for writing.
				if(useSync && !waitTillNextSync)
					Mode::gestureBuffer.endIdx = lastGestLrgIdxSync;


				//advance
				curGestureLevel = GUESTURE_LEVEL_PLAY_RST;
				OnGestureRecordStop();
			}
		}
		else if(GuestureStateWantedTransition(GUESTURE_LEVEL_PLAY, GUESTURE_LEVEL_PLAY_RST))
		{

			LocalMotorAngleState = Mode::knobAngleGestureInitial;
			Mode::gestureBuffer.ResetToBeginning(READ_MODE);


			//advance
			curGestureLevel = GUESTURE_LEVEL_PLAY_RST;
			OnGestureLoop();

		}
		else if(GuestureStateWantedTransition(GUESTURE_LEVEL_PLAY, GUESTURE_LEVEL_REC))
		{
			Mode::gestureBuffer.ResetToBeginning(WRITE_MODE);


			//advance
			curGestureLevel = GUESTURE_LEVEL_REC;
			OnGestureRecordStart();
		}
		else if(GuestureStateWantedTransition(GUESTURE_LEVEL_PLAY, GUESTURE_LEVEL_REC_RST))
		{
			Mode::gestureBuffer.ResetToBeginning(WRITE_MODE);


			//advance
			curGestureLevel = GUESTURE_LEVEL_REC_RST;
		}
		else if(GuestureStateWantedTransition(GUESTURE_LEVEL_PLAY_RST, GUESTURE_LEVEL_PLAY))
		{
			if(syncSig || !useSync)
			{
				LocalMotorAngleState = knobAngleGestureInitial;


				//advance
				curGestureLevel = GUESTURE_LEVEL_PLAY;
				OnGesturePlay();
			}
		}
		else if(GuestureStateWantedTransition(GUESTURE_LEVEL_PLAY_RST, GUESTURE_LEVEL_REC_RST))
		{
			Mode::gestureBuffer.ResetToBeginning(WRITE_MODE);


			//advance
			curGestureLevel = GUESTURE_LEVEL_REC_RST;
		}
		else
		{
			//advance
			curGestureLevel = (GUESTURE_LEVEL)gestTrgtLevelTrigger.TriggerRegion();
		}
	}


	if(bypassGuestureInput)
		curGestureLevel = GUESTURE_LEVEL_PLAY_RST;;

	if(gesturePlayLoopPrimed)
	{
		if(syncSig || !useSync || !waitTillNextSync)
		{
			//update offset
			loopOffsetAngle = latestGDREAD.angle;

			LocalMotorAngleState =  Mode::knobAngleGestureInitial;
			Mode::gestureBuffer.ResetToBeginning(READ_MODE);//loop to beginning


			gesturePlayLoopPrimed = false;
			justLoopedInd = 10;


			OnGestureLoop();
		}
	}



}
void Mode::PreKnobDSP(float sampleTime)
{

}

void Mode::TorqueGuestureProcessFilter(float sampleTime)
{
	//record  guestures
	if(	curGestureLevel == GUESTURE_LEVEL_REC && !Mode::gestureBuffer.Busy())
	{
		knobGestureSampleStretch--;
		if(knobGestureSampleStretch == 0)
		{
			//record local angle, and velocity
			GESTUREDATA data;
			data.angle = LocalMotorAngleState.currentAngle;
			data.vel   = LocalMotorAngleState.currentAngularVelFiltered;
			WriteExtraGuestureData(&data.modeData[0]);

			Mode::gestureBuffer.WriteAdvanceSafe(data);
			knobGestureSampleStretch = KNOB_GESTURE_SAMPLE_SKIP;
		}
	}
	else if(curGestureLevel == GUESTURE_LEVEL_PLAY && !Mode::gestureBuffer.Busy())
	{
		float playbackSpeedFactor = knobGestureRecordBPS/tempo.bps;


		driveTorque = 0;
		knobGestureSampleStretch--;
		if(knobGestureSampleStretch <= 0)
		{
			bool s = false;

			if(!Mode::gestureBuffer.AtEndOfBuffer())
			{
				latestGDREAD = Mode::gestureBuffer.ReadAdvanceSafe(s);
			}

			//tableTargetAngle based on playback of gestures.
			MotorAngleStateUpdateLocalf(&LocalMotorAngleState, latestGDREAD.angle , sampleTime);
			ReadExtraGuestureData(&latestGDREAD.modeData[0]);

			if(Mode::gestureBuffer.AtEndOfBuffer())
			{
				gesturePlayLoopPrimed = true;
			}

			knobGestureSampleStretch = MathExtras::ClampMin(int(KNOB_GESTURE_SAMPLE_SKIP*(playbackSpeedFactor)),1);

		}
	}
	else if(Mode::curGestureLevel == GUESTURE_LEVEL_REC_RST)
	{

	}
	else if(Mode::curGestureLevel == GUESTURE_LEVEL_PLAY_RST)
	{

	}
	else
	{
		driveTorque = 0;
	}

}
void Mode::MainThreadUpdateFunction(float sampleTime)
{


	//update guestures baseColor
	if(Mode::gestureBuffer.Busy())//show busy
	{
		inputOutputDescriptors[2].augments[0].baseColor = MathExtras::Color::CYAN;
	}
	else if(Mode::curGestureLevel == GUESTURE_LEVEL_REC)
	{
		inputOutputDescriptors[2].augments[0].baseColor = MathExtras::Color::RED;
	}
	else if(Mode::curGestureLevel == GUESTURE_LEVEL_PLAY)
	{
		if(gesturePlayLoopPrimed)
		{
			inputOutputDescriptors[2].augments[0].baseColor = MathExtras::Color::WHITE;
		}
		else if(justLoopedInd >= 0){
			inputOutputDescriptors[2].augments[0].baseColor = MathExtras::Color::BLUE;
			justLoopedInd--;
		}
		else
			inputOutputDescriptors[2].augments[0].baseColor = MathExtras::Color::GREEN;
	}
	else if(Mode::curGestureLevel == GUESTURE_LEVEL_REC_RST)
	{
		inputOutputDescriptors[2].augments[0].baseColor = MathExtras::Color::YELLOW;
	}
	else if(Mode::curGestureLevel == GUESTURE_LEVEL_PLAY_RST)
	{
		inputOutputDescriptors[2].augments[0].baseColor = MathExtras::Color::MAGENTA;
	}


	//open gesture file
	if(!Mode::gestureFile.obj.lockid)
	{
		FRESULT res = f_open(&Mode::gestureFile, "GESTUREF", FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
		if(res != FR_OK)
		{
			strcpy(errorCode, "Gesture File Error");
			Error_Handler();
		}
	}



	int sf = MathExtras::ClampInclusive(mainThreadSyncInd,0,1);

	//printf("flipQueueSize: %d, serviceQueueSize: %d\r\n", Mode::gestureBuffer.flipQueue.size(), Mode::gestureBuffer.serviceQueue.size());
	//printf("%u, %d, %d, %f, %d\r\n", sf,  Mode::gestureBuffer.LargeIndex(),  Mode::gestureBuffer.endIdx, tempo.PercToNextTap() , Mode::gestureBuffer.bufferIdx);

	if(mainThreadSyncInd>0 && sf>0)
		mainThreadSyncInd=0;

	Mode::gestureBuffer.ServiceFile(&Mode::gestureFile);




	//update tempo save timer and save tempo if needed
	if(saveTimerEn)
	{
		saveTimer--;
		if(saveTimer <= 0)
		{

			OnSaveTimerTimeout();

			saveTimerEn = false;
		}
	}


}
void Mode::OnSaveTimerTimeout(){

	if(saveTapBpm)
	{
		ee24_write_float(EE_TAP_TEMPO_TNEXT_32bits, tempo.tNext, 1000);
		ee24_write_float(EE_TAP_TEMPO_BPS_32bits, tempo.bps, 1000);
		ee24_write_float(EE_TAP_TEMPO_BPSFLT_32bits, tempo.bps_filtered, 1000);
		saveTapBpm = false;
	}


	if(Mode::saveKnobGestureRecordBPS)
	{
		ee24_write_float(EE_GESTURE_TEMPO_FLOAT_32bits, knobGestureRecordBPS, 1000);
		Mode::saveKnobGestureRecordBPS = false;
	}

	//save gesture buffer endidx if needed
	if(gestureSaveRecInfo)
	{
		ee24_write_32(EE_GESTURE_ENDIDX_32bits, Mode::gestureBuffer.endIdx, 1000);

		ee24_write(EE_GESTURE_MOTORSTATE_nbits, (uint8_t*)&Mode::knobAngleGestureInitial, sizeof(MotorAngleState), 1000);


		gestureSaveRecInfo = false;
	}

	ee24_flush();
}

void Mode::PrintCoggStats(int i)
{
	/*
	  printf("%f,%f,%d,%f,%d,%f\r\n",modeManager.currentMode()->LocalMotorAngleState.currentAngle,
			  Mode::coggDriveLookupCW[i],
			  Mode::coggDriveLookupCW_SMPLCNT[i],
			  Mode::coggDriveLookupCCW[i],
			  Mode::coggDriveLookupCCW_SMPLCNT[i],
			  Mode::coggDriveLookupCCW[i]+Mode::coggDriveLookupCW[i]
			  );
			  */
}


void Mode::DoAntiCoggCalibration()
{
	/*
	Mode::calibrationMode = Mode::CalibrationMode_AntiCoggCW;
	const int countDownCount = 10000;
	Mode::calibrationDelayCountdown = countDownCount;
	do
	{
		bool cwSampleComplete=true;
		for(int i = 0; i < ANTCOGGPOINTS; i++)
		{
		  Mode::PrintCoggStats(i);

		  if(Mode::coggDriveLookupCW_SMPLCNT[i] < ANTICOGGMINSAMPLES)
		  {
			  cwSampleComplete = false;
		  }

		}
		Mode::coggDriveLookupCW_SMPLCMPL = cwSampleComplete;
	} while(!Mode::coggDriveLookupCW_SMPLCMPL);


	Mode::calibrationMode = Mode::CalibrationMode_AntiCoggCCW;
	Mode::calibrationDelayCountdown = countDownCount;
	do
	{
		bool ccwSampleComplete=true;
		for(int i = 0; i < ANTCOGGPOINTS; i++)
		{
			Mode::PrintCoggStats(i);

		  if(Mode::coggDriveLookupCCW_SMPLCNT[i] < ANTICOGGMINSAMPLES)
		  {
			  ccwSampleComplete = false;
		  }
		}
		Mode::coggDriveLookupCCW_SMPLCMPL = ccwSampleComplete;
	} while(!Mode::coggDriveLookupCCW_SMPLCMPL);


	Mode::calibrationMode = Mode::CalibrationMode_AntiCoggTESTCCW;
	int numRotations = 10;
	do
	{

		for(int i = 0; i < ANTCOGGPOINTS; i++)
		{
			Mode::PrintCoggStats(i);

		}
		numRotations--;

	} while(numRotations>0);

	Mode::calibrationMode = Mode::CalibrationMode_AntiCoggTESTCW;
	numRotations = 10;
	do
	{
		for(int i = 0; i < ANTCOGGPOINTS; i++)
		{
			Mode::PrintCoggStats(i);
		}
		numRotations--;

	} while(numRotations>0);
	Mode::calibrationMode = Mode::CalibrationMode_None;
	*/
}

void Mode::MasterProcess_Torque(float sampleTime)
{

	TorqueGuestureProcessFilter(sampleTime);

	driveTorque = MathExtras::ClampInclusive(driveTorque, -1.0f, 1.0f);


	/*
	const float calibrationTargetSpeed = 0.5f;
	int angleLookup = int(LocalMotorAngleState.currentAngle*ANTCOGGPOINTS);
	Mode::antiCoggPID.PGain = calibrationTargetSpeed*2.5f;
	Mode::antiCoggPID.IGain = 0.00f;



	if(calibrationMode == CalibrationMode_AntiCoggCW)
	{

		//driveTorque = (-calibrationTargetSpeed - LocalMotorAngleState.currentAngularVelFiltered);
		driveTorque = antiCoggPID.PIControl(-calibrationTargetSpeed, LocalMotorAngleState.currentAngularVelFiltered);

		if(Mode::calibrationDelayCountdown <= 0 && Mode::coggDriveLookupCW_SMPLCNT[angleLookup] < ANTICOGGMINSAMPLES)
		{
			Mode::coggDriveLookupCW[angleLookup] += driveTorque/ANTICOGGMINSAMPLES;
			Mode::coggDriveLookupCW_SMPLCNT[angleLookup]++;
		}

		if(Mode::calibrationDelayCountdown > 0)
			Mode::calibrationDelayCountdown--;


	}
	else if(calibrationMode == CalibrationMode_AntiCoggCCW)
	{
		//driveTorque = (calibrationTargetSpeed - LocalMotorAngleState.currentAngularVelFiltered);
		driveTorque = antiCoggPID.PIControl(calibrationTargetSpeed, LocalMotorAngleState.currentAngularVelFiltered);

		if(Mode::calibrationDelayCountdown <= 0 && Mode::coggDriveLookupCCW_SMPLCNT[angleLookup] < ANTICOGGMINSAMPLES)
		{
			Mode::coggDriveLookupCCW[angleLookup] += driveTorque/ANTICOGGMINSAMPLES;
			Mode::coggDriveLookupCCW_SMPLCNT[angleLookup]++;
		}

		if(Mode::calibrationDelayCountdown > 0)
			Mode::calibrationDelayCountdown--;
	}
	else
	{
		if(calibrationMode == CalibrationMode_AntiCoggTESTCCW)
			driveTorque = antiCoggPID.PIControl(calibrationTargetSpeed, LocalMotorAngleState.currentAngularVelFiltered);


		else if(calibrationMode == CalibrationMode_AntiCoggTESTCW)
			driveTorque = antiCoggPID.PIControl(-calibrationTargetSpeed, LocalMotorAngleState.currentAngularVelFiltered);


		//apply anticogg
		driveTorque -= (Mode::coggDriveLookupCW[angleLookup] + Mode::coggDriveLookupCCW[angleLookup]);
	}
    */


	driveTorque += LocalMotorAngleStatePure.currentAngularVelFiltered*(frictionCalFactor);


	driveTorque = MathExtras::ClampInclusive(driveTorque, -1.5f, 1.5f);

#if !defined(COMBINEDBOARD)
	ComHandler.Master_Process((driveTorque+1.5)*(1.0f/3.0f)*0xFFFF          ,      0,       0,       0,
							  &MotorAngleData_Latest,nullptr, nullptr, nullptr);
#elif defined(COMBINEDBOARD)
	//send torque
	motorController.torqueCommand = (-driveTorque*0.5f)*drivePowerFactor + driveOffset;
	motorController.UpdateMotorControl(sampleTime);


	//get angle
	MotorAngleData_Latest = motorController.motorAngle*0xFFFF;

#endif


	float True_KnobAngle_Latest = ((float)MotorAngleData_Latest/UINT16_MAX);


	//update the true motor angle metrics
	MotorAngleStateUpdateLocalf(&GlobalMotorAngleState, True_KnobAngle_Latest, sampleTime);


	//update the mode's "virtualized" motor angle metrics
	float virtualWrappedAngle = MathExtras::WrapMax(-virtualAngleOffset + True_KnobAngle_Latest, 1.0f);
	MotorAngleStateUpdateLocalf(&LocalMotorAngleStatePure,  virtualWrappedAngle, sampleTime);




	if(curGestureLevel == GUESTURE_LEVEL_REC ||
			curGestureLevel == GUESTURE_LEVEL_REC_RST ||
			curGestureLevel == GUESTURE_LEVEL_PLAY_RST)
		MotorAngleStateUpdateLocalf(&LocalMotorAngleState,  virtualWrappedAngle + loopOffsetAngle, sampleTime);


}





//call to tell the mode to handle the button down.
void Mode::ButtonDown(int button)
{

}

//call to tell the mode to handle the button up.
void Mode::ButtonUp(int button)
{

}

void Mode::OnFuncCombo(int button)
{

}
void Mode::OnButtonQuickAction(int button)
{
	if(button == 2 && inputOutputDescriptors[button].curAugment == 0)
	{
		//cycle gesture play/pause default value for no cable plugs
		if(curGestureLevel == GUESTURE_LEVEL_PLAY)
		{
			inputOutputDescriptors[2].augments[0].defaultVoltVal = gestTrgtLevelTrigger.RegionMid(GUESTURE_LEVEL_PLAY_RST);
		}
		else if(curGestureLevel == GUESTURE_LEVEL_REC)
			inputOutputDescriptors[2].augments[0].defaultVoltVal = gestTrgtLevelTrigger.RegionMid(GUESTURE_LEVEL_PLAY_RST);
		else
			inputOutputDescriptors[2].augments[0].defaultVoltVal = gestTrgtLevelTrigger.RegionMid(GUESTURE_LEVEL_PLAY);


		SaveSafeGestureState(inputOutputDescriptors[2].augments[0].defaultVoltVal);



	}
	else if(button == 3 && inputOutputDescriptors[button].curAugment == 0 && !adcPlugStates[3])
	{
		bool firstTap = (adcFakePlugStates[3] == 0);//if the tap is the first tap to bring it to a fake plug state. (ie the tap was released)

		adcFakePlugStates[3] = 1;


		//tap tempo
		inputOutputDescriptors[3].augments[0].useCustomColorIntensity = true;
		inputOutputDescriptors[3].augments[0].customColorIntensity = 0.3f;

		ee24_write_8(EE_FAKE_PLUG_STATES_4_8bits + 3, adcFakePlugStates[3], 1000);

		//restart timer and save tempo to eeprom
		RestartSaveTimer();
		saveTapBpm = true;


		if(firstTap)
		{
			tempo.bps = 1.0f;
			tempo.bps_filtered = 1.0f;
			tempo.tNext = 1 / tempo.bps;
		}
		else
			tempo.TapOnNextProcess();
	}
}

void Mode::OnButtonClickedLongHold(int button)
{
	//way to get out of tapped tempo
	if(button == 3 /* && inputOutputDescriptors[button].curAugment == 0*/)
	{
		adcFakePlugStates[3] = 0;

		tempo.ClearTapCount();
		inputOutputDescriptors[3].augments[0].useCustomColorIntensity = false;

		ee24_write_8(EE_FAKE_PLUG_STATES_4_8bits + 3, (uint8_t)adcFakePlugStates[3], 1000);
	}


	else if(button == 2 && inputOutputDescriptors[button].curAugment == 0)
	{
		//set gesture rec default value for no cable plugs
		inputOutputDescriptors[2].augments[0].defaultVoltVal = gestTrgtLevelTrigger.RegionMid(GUESTURE_LEVEL_REC);
	}
}

void Mode::OnButtonLongHold(int button)
{
	if(button == 2 && inputOutputDescriptors[button].curAugment == 0)
	{
		//set gesture rec default value for no cable plugs
		inputOutputDescriptors[2].augments[0].defaultVoltVal = gestTrgtLevelTrigger.RegionMid(GUESTURE_LEVEL_REC_RST);
	}
}

void Mode::OnADCPlugChange(int adc)
{
	if(adc == 3)
	{
		if(adcPlugStates[3])
		{
			//plugged in
			adcFakePlugStates[3] = 0;
			inputOutputDescriptors[3].augments[0].useCustomColorIntensity = false;
		}
		else
		{
			//unplugged - setup the fakeplug with tempo tapping.. from the bpm of recent plug
			adcFakePlugStates[3] = 1;

			tempo.bps = recentTapBps_1;
			tempo.bps_filtered = recentTapBps_1;
			tempo.tNext = 1/recentTapBps_1;
			tempo.nextTNext = tempo.tNext ;

			RestartSaveTimer();
			saveTapBpm = true;


			inputOutputDescriptors[3].augments[0].useCustomColorIntensity = true;
			inputOutputDescriptors[3].augments[0].customColorIntensity = 0.3f;

		}
	}
}

void Mode::OnPostCycleAugment(int buttonIdx, int augmentIdx)
{

}





void Mode::RecordButtonLevelCycle()
{

}

//Prints Debug Information
void Mode::DebugPrint()
{
	//printf("%f\r\n",LocalMotorAngleState.currentAngle);

}


float Mode::KnobAngleDeadZoneRemap(float knobAngle)
{
	if(knobAngle <= KNOB_DEAD_ZONE_ANGLE_HALF && knobAngle >= -KNOB_DEAD_ZONE_ANGLE_HALF)
	{
		return 0.0f;
	}
	else
	{
		if(knobAngle > 0)
		{
			return knobAngle - KNOB_DEAD_ZONE_ANGLE_HALF;
		}
		else
		{
			return knobAngle + KNOB_DEAD_ZONE_ANGLE_HALF;
		}
	}

}

bool Mode::AngleInDeadZone(float knobAngle)
{
	if(knobAngle <= KNOB_DEAD_ZONE_ANGLE_HALF && knobAngle >= -KNOB_DEAD_ZONE_ANGLE_HALF)
	{
		return true;
	}
	else
	{
		return false;
	}
}

////return 0 when angle just outside of deadzone, 1 when in center of deadzone
//float Mode::KnobAngleDeadZonePerc(float knobAngle)
//{
//
//}

