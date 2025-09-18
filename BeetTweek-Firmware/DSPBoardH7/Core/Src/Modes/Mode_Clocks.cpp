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

	clockBTracker.trigger.SetThreshold(0.5f);
	clockBTracker.trigger.SetThresholdHalfGap(0.001f);

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

	//CLOCK B INPUT
	inputOutputDescriptors[1].enabled = true;
	inputOutputDescriptors[1].augments[0].gateTrigger = &clockBTracker.trigger;
	inputOutputDescriptors[1].augments[0].signalType = EuroRack::SignalType_GATE;
	inputOutputDescriptors[1].augments[0].baseColor = MathExtras::Color::YELLOW;
	//inputOutputDescriptors[1].augments[0].useCustomColorIntensity = true;

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
	float angleLEDRingB = MathExtras::WrapMax(timeAccumCurB, 1.0);



	//LEDManager.SetLEDOuterRing_Float(angleLEDRing,  0,0,LED_BASE_BRIGHTNESS_255);



	//if Y has input clockB and no tap/ext clock - show error indicator for manual speed control.
    if(ADCPluggedIn(1) && !ADCPluggedIn(3)){
		//clock vs clockB error indicator
		float clkclkspeedErrorRing = (timeDelta-timeDeltaB)*40.0f;
		clkclkspeedErrorRing = MathExtras::ClampInclusive(clkclkspeedErrorRing, -0.25f, 0.25f);
		//float clkclkPhaseErrorRing = (timeAccumCur - timeAccumCurB)*20.0f;
		float clkclkPhaseErrorRing = phaseErrorAB*10.0f;
		clkclkPhaseErrorRing = MathExtras::ClampInclusive(clkclkPhaseErrorRing, -0.25f, 0.25f);

		float singleMetric = clkclkspeedErrorRing + MathExtras::Abs(clkclkPhaseErrorRing)*MathExtras::Sign(clkclkspeedErrorRing);

		clkclkPhaseErrorRing = MathExtras::Abs(clkclkPhaseErrorRing);
		clkclkPhaseErrorRing *=  MathExtras::Sign(clkclkspeedErrorRing);

		LEDManager.SetLEDRingRangeLinear_Float(LEDPanelManager::RINGIDENTIFIER_OUTER, 0.0f, clkclkspeedErrorRing, (MathExtras::Color*)&MathExtras::Color::RED,  (MathExtras::Color*)&MathExtras::Color::RED, 0.5f);
		LEDManager.SetLEDRingRangeLinear_Float(LEDPanelManager::RINGIDENTIFIER_OUTER, 0.0f, clkclkPhaseErrorRing, (MathExtras::Color*)&MathExtras::Color::RED,  (MathExtras::Color*)&MathExtras::Color::RED, 0.5f);


		//LEDManager.SetLEDRingRangeLinear_Float(LEDPanelManager::RINGIDENTIFIER_OUTER, 0.0f, singleMetric, (MathExtras::Color*)&MathExtras::Color::RED,  (MathExtras::Color*)&MathExtras::Color::RED, 0.5f);

    }






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
    if(ADCPluggedIn(1)){
		//Rotating Grid For Clock B
		i = 0;
		for(int tick = angleLEDRingB*LED_NUM_LEDS_PER_RING; tick <=  angleLEDRingB*LED_NUM_LEDS_PER_RING + LED_NUM_LEDS_PER_RING; tick += inc)
		{
			if(i == 0)
			{
				LEDManager.SetLEDInnerRing_Int(tick,  LED_BASE_BRIGHTNESS_255, LED_BASE_BRIGHTNESS_255, 0);
				if((tick+1)%LED_NUM_LEDS_PER_RING == 0)
				{

				}
			}
			else
			{
				LEDManager.SetLEDInnerRing_Int(tick,  LED_BASE_BRIGHTNESS_255, LED_BASE_BRIGHTNESS_255, 0);
			}
			i++;
		}
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





    if(!ADCPluggedIn(1)){
	if(highSpeedRingIndicatorFlag)
	{
		LEDManager.SetLEDInnerRing_Float(MathExtras::WrapMax(timeAccumCur*marksPerTable, 1.0),
				LED_BASE_BRIGHTNESS_255,
				LED_BASE_BRIGHTNESS_255/2,
				0);
	}
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
    syncErrorB = MathExtras::WrappedLocalDifference( double(MathExtras::WrapMax(clockBTracker.PercToNextTap() + clockBTracker.bps_filtered*delayCompensation, 1.0f)), MathExtras::WrapMax(timeAccumCurB*marksPerTable,1.0), 1.0);

    if(ADCPluggedIn(3))
    {
    	phaseErrorAB = MathExtras::WrappedLocalDifference(tempo.PercToNextTap(),clockBTracker.PercToNextTap(),1.0f);
    }
    else
    {
    	phaseErrorAB = MathExtras::WrappedLocalDifference(float(MathExtras::WrapMax(timeAccumCur*marksPerTable,1.0)),clockBTracker.PercToNextTap(),1.0f);
    }

    SetDacVolts(3, noDeadAngle*EuroRack::SignalVoltRange(inputOutputDescriptors[7].CurAugment().signalType));

    if(ADCPluggedIn(1)){
    	clockBTracker.Process(adcVolts[1], sampleTime);
    }

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

		float speedAdjWeight = 0.0f;
		float phaseAdjWeight = 0.0f;

		float slightSpeedAdj = 0.0f;
		float slightPhaseAdj = 0.0f;

		//syncing case
		if(ADCPluggedIn(1))
		{
			float spdError = (timeDelta-timeDeltaB)*100.0f;


			speedAdjWeight = MathExtras::ClampMax(MathExtras::Abs(spdError), 1.0f);
			phaseAdjWeight = 1.0f - speedAdjWeight;

			if(AngleInDeadZone(rawAngle) && MathExtras::Abs(spdError) < 0.01f)
				slightSpeedAdj = -0.00001f*MathExtras::Sign(spdError)*(1.0f-MathExtras::Abs(phaseErrorAB));

			if(AngleInDeadZone(rawAngle) && MathExtras::Abs(spdError) < 0.1f)
				slightPhaseAdj = -0.001f*phaseErrorAB;
		}
		else
		{
			speedAdjWeight = 1.0f;
			phaseAdjWeight = 0.0f;
		}

		targetSpeed += 0.0001f*speedAdjWeight*noDeadAngle + slightSpeedAdj ;//speed adjust
		timeAccumCur += 0.0001f*phaseAdjWeight*noDeadAngle + slightPhaseAdj;//phase shift




		targetSpeed = MathExtras::ClampMin<float>(targetSpeed, 0.0);
    }

    float a = 0.0f;
    if(AngleInDeadZone(rawAngle))
    {
    	a = sampleTime*(syncError)*100000.0f;//strong snap back to sync.
    }

    timeDelta = targetSpeed + speedAlteration + a;
    float b = 0.0f;

    b = sampleTime*(syncErrorB)*100000.0f;//strong snap to sync with indicator.
    timeDeltaB = clockBTracker.bps_filtered*(1.0/marksPerTable);//+ b;

	double delta = sampleTime*(timeDelta);
	delta = MathExtras::ClampMin(delta, 0.0);


	//if angle <= left hand quarter - reset clock.(if tap tempo/external)
	if(noDeadAngle < -0.25 && ADCPluggedIn(3))
	{
		timeAccumCur = 0.0f;
	}


	double deltaB = sampleTime*(timeDeltaB);
	deltaB = MathExtras::ClampMin(deltaB, 0.0);


	curSpeed = delta;
	timeAccumCur += delta;

	timeAccumCurB += deltaB;


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
	timeAccumCurB = MathExtras::WrapOnceMax(timeAccumCurB, 1.0);


}




void Mode_Clocks::KnobDSPFunction(float sampleTime)
{




	driveTorque = -10.0f*MotorAngleStateGetCurrentAccumulatedAngled(&LocalMotorAngleState);

	driveTorque = MathExtras::ClampInclusive(driveTorque, -1.0f, 1.0f);



	float feedBack = 0.0f;
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



