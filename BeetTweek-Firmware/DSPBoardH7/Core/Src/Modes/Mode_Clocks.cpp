/*
 * Mode_Clocks.cpp
 *
 */




#include "Modes/Mode_Clocks.h"

#include "clockinput.h"

extern ModeManager modeManager;

Mode_Clocks::Mode_Clocks() : Mode()
{
	SetBothNames("Mode_Clocks");
}


Mode_Clocks::~Mode_Clocks()
{

}

void Mode_Clocks::Initialize() {
	Mode::Initialize();

	for(int i = 0; i < 4; i++)
	{

		triggers[i].SetThreshold(0.5f);
		triggers[i].SetThresholdHalfGap(0.001f);

		outputTimeGates[i].SetTrigger(&triggers[i]);
		outputTimeGates[i].SetSampleCount(SAMPLERATE*EuroRack::ClockGateTime);
	}

	for(int i = 0; i < 3; i++)
	{
		inputOutputDescriptors[4+i].enabled = true;
		inputOutputDescriptors[4+i].numAugments = 2;

		inputOutputDescriptors[4+i].augments[0].signalType = EuroRack::SignalType_GATE;
		inputOutputDescriptors[4+i].augments[0].baseColor = MathExtras::Color::ORANGE;
		inputOutputDescriptors[4+i].augments[0].useCustomColorIntensity = true;


		inputOutputDescriptors[4+i].augments[1].signalType = EuroRack::SignalType_CV;
		inputOutputDescriptors[4+i].augments[1].baseColor = MathExtras::Color::RED;
	}
	inputOutputDescriptors[7].enabled = true;
	inputOutputDescriptors[7].augments[0].signalType = EuroRack::SignalType_CV;
	inputOutputDescriptors[7].augments[0].baseColor = MathExtras::Color::BLUE;




	inputOutputDescriptors[0].enabled = true;

	//SPEED INPUT
	inputOutputDescriptors[0].augments[0].signalType = EuroRack::SignalType_CV;
	inputOutputDescriptors[0].augments[0].baseColor = MathExtras::Color::BLUE;




}



//call to tell the mode to handle the button down.
void Mode_Clocks::ButtonDown(int button)
{


}

//call to tell the mode to handle the button up.
void Mode_Clocks::ButtonUp(int button)
{
}



void Mode_Clocks::UpdateLEDS(float sampleTime)
{

	WS2812_SETALLLED(0,0,0);

	//basic led scheme
	float angleLEDRing = MathExtras::WrapMax(timeAccumCur, 1.0);



	//LEDManager.SetLEDOuterRing_Float(angleLEDRing,  0,0,LED_BASE_BRIGHTNESS_255);

	//Rotating Grid With Center led blue for reference
	const int inc = LED_NUM_LEDS_PER_RING/(float(marksPerTable-1));
	int i = 0;
	for(int tick = angleLEDRing*LED_NUM_LEDS_PER_RING; tick <=  angleLEDRing*LED_NUM_LEDS_PER_RING + LED_NUM_LEDS_PER_RING; tick += inc)
	{
		if(i == 0)
		{
			LEDManager.SetLEDOuterRing_Int(tick,  LED_BASE_BRIGHTNESS_255, LED_BASE_BRIGHTNESS_255/2, 0);
			if((tick+1)%LED_NUM_LEDS_PER_RING == 0)
			{

			}
		}
		else
		{
			LEDManager.SetLEDOuterRing_Int(tick,  LED_BASE_BRIGHTNESS_255, LED_BASE_BRIGHTNESS_255/2, 0);



		}
		i++;
	}



	if(timeDelta <= 0.0f)
	{

		LEDManager.SetLEDRingRangeLinear_Float(LEDPanelManager::RINGIDENTIFIER_INNER, 0.0f, noDeadAngle, (MathExtras::Color*)&MathExtras::Color::RED,  (MathExtras::Color*)&MathExtras::Color::RED, 0.5f);

		inputOutputDescriptors[7].augments[0].baseColor = MathExtras::Color::RED;
	}
	else
	{
		if(noDeadAngle > 0.0f)
		{
			LEDManager.SetLEDRingRangeLinear_Float(LEDPanelManager::RINGIDENTIFIER_INNER, 0.0f, noDeadAngle, (MathExtras::Color*)&MathExtras::Color::GREEN,  (MathExtras::Color*)&MathExtras::Color::GREEN, 0.5f);
			inputOutputDescriptors[7].augments[0].baseColor = MathExtras::Color::GREEN;



		}
		else if(AngleInDeadZone(rawAngle))
		{
			LEDManager.SetLEDRingRangeLinear_Float(LEDPanelManager::RINGIDENTIFIER_INNER, 0.0f, noDeadAngle, (MathExtras::Color*)&MathExtras::Color::BLUE,  (MathExtras::Color*)&MathExtras::Color::BLUE, 0.5f);

			inputOutputDescriptors[7].augments[0].baseColor = MathExtras::Color::BLUE;

		}
		else
		{
			LEDManager.SetLEDRingRangeLinear_Float(LEDPanelManager::RINGIDENTIFIER_INNER, 0.0f, noDeadAngle, (MathExtras::Color*)&MathExtras::Color::ORANGE,  (MathExtras::Color*)&MathExtras::Color::ORANGE, 0.5f);

			inputOutputDescriptors[7].augments[0].baseColor = MathExtras::Color::ORANGE;

		}
	}

	//quarter markers
	for(int i = 1; i <= 3; i++)
	{
		if(rawAngle >= 0.25f*i - 0.125f)
		{
			LEDManager.SetLEDOuterRing_Int((LED_NUM_LEDS_PER_RING/4)*i, LED_BASE_BRIGHTNESS_255, LED_BASE_BRIGHTNESS_255, LED_BASE_BRIGHTNESS_255);
		}
		if(rawAngle <= -0.25f*i + 0.125f && i <= 2)
		{
			LEDManager.SetLEDOuterRing_Int(-(LED_NUM_LEDS_PER_RING/4)*i, LED_BASE_BRIGHTNESS_255, LED_BASE_BRIGHTNESS_255, LED_BASE_BRIGHTNESS_255);
		}
	}



	LEDManager.SetLEDOuterRing_Int(0,
			LED_BASE_BRIGHTNESS_255,
			LED_BASE_BRIGHTNESS_255,
			LED_BASE_BRIGHTNESS_255);


	if(highSpeedRingIndicatorFlag)
	{
		LEDManager.SetLEDInnerRing_Float(MathExtras::WrapMax(timeAccumCur*marksPerTable, 1.0),
				LED_BASE_BRIGHTNESS_255,
				LED_BASE_BRIGHTNESS_255/2,
				0);
	}

	StandardPanelLEDDraw();
}

//Called Very Fast.  Holds DSP Code
void Mode_Clocks::AudioDSPFunction(float sampleTime, int bufferSwap)
{
	rawAngle = -MotorAngleStateGetCurrentAccumulatedAngled(&LocalMotorAngleState);

	rawAngle += MathExtras::ClampInclusive((2.0f*adcVolts[0])/EuroRack::SignalVoltRange(inputOutputDescriptors[0].CurAugment().signalType), -1.0f, 1.0f);




	noDeadAngle =   1.0f*KnobAngleDeadZoneRemap(rawAngle);


	float delayCompensation = BLOCKPROCESSINGTIME_S*marksPerTable*1.0f;
    syncError = MathExtras::WrappedLocalDifference( double(MathExtras::WrapMax(tempo.PercToNextTap() + tempo.bps_filtered*delayCompensation, 1.0f)), MathExtras::WrapMax(timeAccumCur*marksPerTable,1.0), 1.0);


    SetDacVolts(3, noDeadAngle*EuroRack::SignalVoltRange(inputOutputDescriptors[7].CurAugment().signalType));


	if(curSpeed*SAMPLERATE < highSpeedRingSpeedThresh)
	{
		highSpeedRingIndicatorFlag = true;
	}

	if(rawAngle > -KNOB_DEAD_ZONE_ANGLE_HALF)
	{
		if((curSpeed*SAMPLERATE >= highSpeedRingSpeedThresh) )
		{
			highSpeedRingIndicatorFlag = false;
		}
	}





    float tempoSpeed =  tempo.bps_filtered*(1.0/marksPerTable);




    float speedAlteration = 0.0f;

    if(ADCPluggedIn(3))
    {
    	//linked to tempo
    	targetSpeed = tempoSpeed;
    	speedAlteration = noDeadAngle;
    }
    else
    {
    	//freehand
		syncError = 0.0f;
		targetSpeed += 0.0001f*noDeadAngle;
		targetSpeed = MathExtras::ClampMin<float>(targetSpeed, 0.0);
    }

    float a = 0.0f;
    if(AngleInDeadZone(rawAngle))
    {
    	a = sampleTime*(syncError)*100000.0f;//strong snap back to sync.
    }

    timeDelta = targetSpeed + speedAlteration + a;


	double delta = sampleTime*(timeDelta);
	delta = MathExtras::ClampMin(delta, 0.0);


	//if angle <= left hand quater - reset clock.
	if(noDeadAngle < -0.25)
	{
		timeAccumCur = 0.0f;
	}



//    if(ADCPluggedIn(3))
//    {
//		float totalError = syncError ;
//		errorAccum += totalError;
//		delta =  MathExtras::ClampMin(sampleTime*tempoSpeed + sampleTime*(totalError)*100.0f, 0.0f);
//    }

	//float curBeatFloor = MathExtras::Floor(timeAccumCur*marksPerTable);

//	if(AngleInDeadZone(rawAngle))
//	{
//
//		float totalError = syncError ;
//
//		errorAccum += totalError;
//		delta =  MathExtras::ClampMin(sampleTime*tempoSpeed + sampleTime*(totalError)*100.0f, 0.0f);
//
//
//
//		inDirectPlay = false;
//		directPlayLastAngle = 0.0f;
//	}
//	else
//	{
//		errorAccum = 0.0f;
//
//
//		inDirectPlay = false;
//		directPlayLastAngle = 0.0f;
//	}
	curSpeed = delta;
	timeAccumCur += delta;


	for(int i = 0; i < 3; i++)
	{
		float ang = MathExtras::WrapMax<double>(timeAccumCur*marksPerTable*(i+1), 1.0);
		float pulseLevel;
		if(ang < 0.5f)
			pulseLevel = outputTimeGates[i].Process(1.0f);
		else
			pulseLevel = outputTimeGates[i].Process(0.0f);

		if(inputOutputDescriptors[4+i].curAugment == 0)
		{
			SetDacVolts(i, pulseLevel*EuroRack::SignalVoltRange(inputOutputDescriptors[4+i].CurAugment().signalType));
		}
		else if(inputOutputDescriptors[4+i].curAugment == 1)
		{
			SetDacVolts(i, (ang-0.5f)*EuroRack::SignalVoltRange(inputOutputDescriptors[4+i].CurAugment().signalType));
		}





	}


	timeAccumCur = MathExtras::WrapOnceMax(timeAccumCur, 1.0);
}




void Mode_Clocks::KnobDSPFunction(float sampleTime)
{




	driveTorque = -10.0f*MotorAngleStateGetCurrentAccumulatedAngled(&LocalMotorAngleState);

	driveTorque = MathExtras::ClampInclusive(driveTorque, -1.0f, 1.0f);



	float feedBack = 0.0f;//1.0f - tempo.PercToNextTap();

	if(driveTorque < 0.0)
	{
		feedBack = 1.0f-tempo.PercToNextTap();
	}

	feedBack += float(outputTimeGates[0].val);

	feedBack = MathExtras::ClampMax(feedBack, 1.0f);

	driveTorque *= (1.0 - feedBack*0.25);



	MasterProcess_Torque(sampleTime);

}


bool Mode_Clocks::WriteEEPROMState(uint32_t &ee_address)
{
	bool success = Mode::WriteEEPROMState(ee_address);

	success &= ee24_write_float(ee_address, targetSpeed, 1000);
	ee_address += sizeof(float);


	return success;
}


bool Mode_Clocks::ReadEEPROMState(uint32_t &ee_address)
{
	bool success = Mode::ReadEEPROMState(ee_address);

	success &= ee24_read_float(ee_address, &targetSpeed, 1000, 0.0f);
	ee_address += sizeof(float);

	return success;
}

void Mode_Clocks::OnSaveTimerTimeout()
{
	Mode::OnSaveTimerTimeout();



	ee24_write_float(modeManager.CurrentModeEEPromBase()+EEPromAddressOffsets(0), targetSpeed, 1000);



}

void Mode_Clocks::MainThreadUpdateFunction(float sampleTime)
{

	Mode::MainThreadUpdateFunction(sampleTime);



	inputOutputDescriptors[4].augments[0].customColorIntensity = outputTimeGates[0].val;
	inputOutputDescriptors[5].augments[0].customColorIntensity = outputTimeGates[1].val;
	inputOutputDescriptors[6].augments[0].customColorIntensity = outputTimeGates[2].val;
	inputOutputDescriptors[7].augments[0].customColorIntensity = outputTimeGates[3].val;


	//printf("%f, %f, %f\n", syncError, tempo.PercToNextTap(),  MathExtras::WrapMax(timeAccumCur*marksPerTable,1.0));


	//make save timer run routinely.
	if(!saveTimerEn)
		RestartSaveTimer();

}



