/*
 * Mode_SequencedPluck.cpp
 *
 *  Created on: Apr 17, 2021
 *      Author: casht
 */




/*
 * Mode_Rachet.cpp
 *
 *  Created on: Jan 9, 2021
 *      Author: casht
 */




#include <clockinput.h>
#include "Modes/Mode_SequencedPluck.h"

#include "EuroRack.h"
#include "MathExtras_DSP.h"
#include "MathExtras_Curves.h"



Mode_SequencedPluck::~Mode_SequencedPluck()
{

}

void Mode_SequencedPluck::Initialize()
{
	Mode::Initialize();


	//make all variants have same scheme.
	inputOutputDescriptors[0].enabled = true;
	inputOutputDescriptors[0].augments[0].baseColor = MathExtras::Color::CYAN;
	inputOutputDescriptors[0].augments[0].signalType = EuroRack::SignalType_VPerOct;
	inputOutputDescriptors[0].funcCombo = true;


	inputOutputDescriptors[1].enabled = true;
	inputOutputDescriptors[1].augments[0].baseColor = MathExtras::Color::MAGENTA;
	inputOutputDescriptors[1].augments[0].signalType = EuroRack::SignalType_GATE;
	inputOutputDescriptors[1].augments[0].defaultVoltVal = EuroRack::SignalVoltMax(EuroRack::SignalType_GATE);
	inputOutputDescriptors[1].augments[0].gateTrigger = &muteGate;
	inputOutputDescriptors[1].funcCombo = true;


	inputOutputDescriptors[2].numAugments=2;
	inputOutputDescriptors[2].augments[1].baseColor = MathExtras::Color::GRAY;
	inputOutputDescriptors[2].augments[1].useCustomColorIntensity = false;
	inputOutputDescriptors[2].augments[1].defaultVoltVal = 0;
	inputOutputDescriptors[2].prefferedAugment = 1;


	inputOutputDescriptors[3].funcCombo = true;



	inputOutputDescriptors[4].enabled = true;
	inputOutputDescriptors[4].numAugments = 2;
	inputOutputDescriptors[4].funcCombo = true;
	inputOutputDescriptors[4].augments[0].baseColor = MathExtras::Color::RED;
	inputOutputDescriptors[4].augments[0].signalType = EuroRack::SignalType_GATE;
	inputOutputDescriptors[4].augments[1].baseColor = MathExtras::Color::YELLOW;
	inputOutputDescriptors[4].augments[1].signalType = EuroRack::SignalType_GATE;

	inputOutputDescriptors[5].enabled = true;
	inputOutputDescriptors[5].funcCombo = true;
	inputOutputDescriptors[5].augments[0].baseColor = MathExtras::Color::CYAN;
	inputOutputDescriptors[5].augments[0].signalType = EuroRack::SignalType_VPerOct;


	inputOutputDescriptors[6].enabled = true;
	inputOutputDescriptors[6].augments[0].baseColor = MathExtras::Color::BLUE;
	inputOutputDescriptors[6].funcCombo = true;

	inputOutputDescriptors[7].enabled = true;
	inputOutputDescriptors[7].augments[0].baseColor = MathExtras::Color::RED;
	inputOutputDescriptors[7].augments[0].signalType = EuroRack::SignalType_CV;


	stepTracker.rateFilter.filterCoeff = 1000.0f;



	muteGate.SetThreshold(ER_GATE_THRESH_VOLT);
	muteGate.SetThresholdHalfGap(ER_GATE_HALF_HIST_VOLT);

}

//updates the front panel
void Mode_SequencedPluck::UpdateLEDS(float sampleTime)
{
	WS2812_SETALLLED(0,0,0);

	//basic led scheme
	float angleLEDRing = 1.0-MotorAngleStateGetCurrentAccumulatedAnglef(&LocalMotorAngleState);

//	LEDManager.SetLEDRingRangeLinear_Float(LEDPanelManager::RINGIDENTIFIER_INNER, angleLEDRing,angleLEDRing,
//		(MathExtras::Color*)&MathExtras::Color::YELLOW,
//		(MathExtras::Color*)&MathExtras::Color::RED,
//		1/4.0f);

	for(int i = 0; i < LED_NUM_LEDS_PER_RING; i++)
	{
		//float rampVal = pluckTrain((float(i)/LED_NUM_LEDS_PER_RING + angleOffset ) * strumDirection);
		int pluckPointAtLED =  pluckPoint(float(i)/LED_NUM_LEDS_PER_RING + angleOffset);
		//int pluckPointAtLocalAngle = pluckPoint(1.0-LocalMotorAngleState.currentAngle) ;

		float dim = 0.2f;


		MathExtras::Color noteColor;
		noteColor.FromHSL(EuroRack::VOCT_TO_HUE(notes[MathExtras::WrapMax(pluckPointAtLED,numPluckNotes)].volts), 1.0f, 0.5f);


		if(MathExtras::WrapMax(pluckPointAtLED,numPluckNotes) == curNoteIdx)
		{

			MathExtras::Color noteColor;
			noteColor.FromHSL(EuroRack::VOCT_TO_HUE(adcVolts[0]), 1.0f, 0.5f);

			LEDManager.SetLEDInnerRing_Int(i,  noteColor.r_*LED_BASE_BRIGHTNESS_255,
					noteColor.g_*LED_BASE_BRIGHTNESS_255,
					noteColor.b_*LED_BASE_BRIGHTNESS_255);
		}


		if(MathExtras::WrapMax(pluckPointAtLED,numPluckNotes) == MathExtras::WrapMax(plkPoint,numPluckNotes) )
			dim = 1.0f;


		LEDManager.SetLEDOuterRing_Int(i,  dim*noteColor.r_*LED_BASE_BRIGHTNESS_255,
				dim*noteColor.g_*LED_BASE_BRIGHTNESS_255,
				dim*noteColor.b_*LED_BASE_BRIGHTNESS_255);








	}

	//LEDManager.SetLEDOuterRing(0,  LED_BASE_BRIGHTNESS_255,LED_BASE_BRIGHTNESS_255,LED_BASE_BRIGHTNESS_255);



	LEDManager.SetLEDInnerRing_Float(angleLEDRing,  MathExtras::Abs(driveTorque)*LED_BASE_BRIGHTNESS_255,
			(1.0f-MathExtras::Abs(driveTorque))*LED_BASE_BRIGHTNESS_255,
			0);


	StandardPanelLEDDraw();
}



//Called Very Fast.  Holds DSP Code
void Mode_SequencedPluck::AudioDSPFunction(float sampleTime, int bufferSwap)
{
	stepTracker.Process(adcVolts[0], sampleTime);
	muteGate.Process(adcVolts[1]);


	bool triggered = false;
	if(ADCPluggedIn(3))
	{
		triggered =  tempo.Tapped();

	}
	else
	{
		triggered = stepTracker.SnapShotted();
	}



	if( triggered )
	{
		//sample a new note
		SampleNewNote();

		//reset save timer
		saveTimerEn = true;
		saveTimer = 0;
	}


	dynamics = dynamicsCurveTrain(-LocalMotorAngleState.currentAngle);
	float aug = MathExtras::ClampInclusive(adcVolts[1], 0.0f, ER_CV_VOLTAGE_MAX)/ER_CV_VOLTAGE_MAX;
	dynamics = dynamics*aug;
	float volts = (dynamics)*EuroRack::SignalVoltMax(inputOutputDescriptors[6].CurAugment().signalType);
	SetDacVolts(2, volts);



	float dynamicsClamped = MathExtras::ClampInclusive(dynamics, 0.5f + ER_GATE_HALF_HIST_VOLT, 1.0f);

	if(inputOutputDescriptors[4].curAugment != 0)
		dynamicsClamped = 1.0f;

	SetDacVolts(0, (outGateVal*dynamicsClamped)*EuroRack::SignalVoltMax(inputOutputDescriptors[4].CurAugment().signalType));




	if(triggerGateSeq > 0)
	{
		triggerGateSeq--;
	}
	else if(triggerGateSeq == 0)
	{
		outGateVal = 1;
	}


	SetDacVolts(3, pluckIntensity);
	SetDacVolts(1, (pluckVoltage));
}


void Mode_SequencedPluck::SampleNewNote()
{
	if(ADCPluggedIn(0))
		notes[curNoteIdx].volts = adcVolts[0];

	curNoteIdx = MathExtras::WrapMax(curNoteIdx+1, numPluckNotes);
}




//Called Somewhat Fast.  Holds Knob Update Processing.
void Mode_SequencedPluck::KnobDSPFunction(float sampleTime)
{
	accumAngle = -MotorAngleStateGetCurrentAccumulatedAngled(&LocalMotorAngleState);


	float rampVal = pluckTrain( (accumAngle + angleOffset ) * strumDirection) ;
	float rampVal_rev = pluckTrain( (accumAngle + angleOffset ) * -strumDirection) ;
	plkPoint = pluckPoint( (accumAngle + angleOffset) );

	if(knobDspTick == 0 || rstPluck)
	{
		lastHitPluckPoint = plkPoint-strumDirection;
		lastPluckPoint = plkPoint-strumDirection;
		rstPluck = false;
	}


	if((MathExtras::Sign(lastHitPluckPoint - plkPoint) == strumDirection) || (lastHitPluckPoint == plkPoint))
	{
		if(doublePluckEnable)
			driveTorque = -strumDirection*rampVal_rev;
		else
			driveTorque = 0.0f;
	}
    else
	{
		driveTorque = strumDirection*rampVal;
	}

	if(MathExtras::Sign(lastHitPluckPoint - plkPoint) == strumDirection)
	{
		lastHitPluckPoint = plkPoint;
	}


	float angleOffsetVel = strumDirection*shiftDirMult*(tempo.bps/numPluckNotes);

	//if this is a pluck event.
	if(MathExtras::Abs(driveTorque) < MathExtras::Abs(lastDriveTorque) - 0.5f)//plucked.
	{
		lastHitPluckPoint = plkPoint;
		pluckVoltage = notes[MathExtras::WrapMax(plkPoint,numPluckNotes)].volts;
		pluckIntensity =  MathExtras::Abs(MotorAngleToVolts(1.0/(LocalMotorAngleState.currentAngularVelFiltered - angleOffsetVel)));

		if((muteGate.TriggerLevel()))
		{
			if(triggerGateSeq == 0)
			{
				if(outGateVal == 1)
				{
					//gate to 0, wait 10, then gate will go high.
					outGateVal = 0;
					triggerGateSeq = 10;
				}
				else
				{
					//wait 10, then gate will go high (for consistent timing)
					triggerGateSeq = 10;
				}
			}
		}
	}




	lastPluckPoint = plkPoint;
	lastRampV = rampVal;
	lastDriveTorque = driveTorque;

	//shifting.
	if(ADCPluggedIn(3))
		angleOffset += angleOffsetVel*sampleTime;

	if(!(muteGate.TriggerLevel()))
	{
		driveTorque = 0.0f;
		lastDriveTorque = 0.0f;
	}

	//audio haptic feed (high pass filtered)
	if(inputOutputDescriptors[2].curAugment == 1 && ADCPluggedIn(2))
	{
		float hapticSig = (adcVolts[2]/ER_CV_VOLTAGE_MAX)*0.25f;
		const float rc = 0.1;
		float a = rc/(rc + sampleTime);

		float highpassed_haptic = a*(highpassed_haptic_1 + hapticSig - hapticSig_1);
		highpassed_haptic_1 = highpassed_haptic;
		hapticSig_1 = hapticSig;

		driveTorque += MathExtras::ClampInclusive(highpassed_haptic, -0.125f, 0.125f);
	}




	MasterProcess_Torque(sampleTime);
}
float Mode_SequencedPluck::pluckTrain(double angle)
{
	return MathExtras::RampTrain1Centered(angle, 1.0/numPluckNotes, rampCycleDuty);
}

float Mode_SequencedPluck::dynamicsCurveTrain(double angle)
{

	float v = arm_sin_f32( MathExtras::WrapMax(angle+angleOffset,1.0)/M_RADTOPERC * (numPluckNotes) - M_PI_2);
	return v;
}


//return what angle a pluckIdx is at (the fall point)
//double Mode_SequencedPluck::pluckAngle(int pluckIdx)
//{
//	return double(pluckIdx)/MAX_PLUCK_NOTES + rampCycleDuty*(1.0/MAX_PLUCK_NOTES) - ((1.0/MAX_PLUCK_NOTES)*rampCycleDuty*0.5) + angleOffset;
//}

int Mode_SequencedPluck::pluckPoint(double angle)
{
	return MathExtras::RoundToInt((angle)*(numPluckNotes));
}


void Mode_SequencedPluck::UpdateShiftDirMultFromCycle()
{	const float m = 0.25;
	if(shiftDirCycle == 0)
	{
		shiftDirMult = 0*m;
	}
	else if(shiftDirCycle == 1)
	{
		shiftDirMult = 0.125*m;
	}
	else if(shiftDirCycle == 2)
	{
		shiftDirMult = 0.25*m;
	}
	else if(shiftDirCycle == 3)
	{
		shiftDirMult = 0.5*m;
	}
	else if(shiftDirCycle == 4)
	{
		shiftDirMult = 1*m;
	}
	else if(shiftDirCycle == 5)
	{
		shiftDirMult = 2*m;
	}
	else if(shiftDirCycle == 6)
	{
		shiftDirMult = 4*m;
	}
}


void Mode_SequencedPluck::DebugPrint()
{
	Mode::DebugPrint();
	//printf("%d,%f\r\n",plkPoint, accumAngle);
}

void Mode_SequencedPluck::OnButtonQuickAction(int button)
{
	Mode::OnButtonQuickAction(button);


}


void Mode_SequencedPluck::OnFuncCombo(int button)
{
	Mode::OnFuncCombo(button);
	if(button == 0)
	{
		strumDirection = -strumDirection;
		ee24_write_32(modeManager.CurrentModeEEPromBase()+EEPromAddressOffsets(1), (uint32_t)strumDirection, 1000);
	}
	else if(button == 4)
	{
		numPluckNotes = MathExtras::WrapMinMax(numPluckNotes + 1, 4, MODE_SP_MAX_PLUCKNOTES);
		ee24_write_32(modeManager.CurrentModeEEPromBase()+EEPromAddressOffsets(3), (uint32_t)numPluckNotes, 1000);
	}
	else if(button == 6)
	{
		numPluckNotes = MathExtras::WrapMinMax(numPluckNotes - 1, 4, MODE_SP_MAX_PLUCKNOTES+1);
		ee24_write_32(modeManager.CurrentModeEEPromBase()+EEPromAddressOffsets(3), (uint32_t)numPluckNotes, 1000);
	}
	else if(button == 1)
	{
		shiftDirCycle = MathExtras::WrapMax(shiftDirCycle+1,7);
		UpdateShiftDirMultFromCycle();

		int addr = modeManager.CurrentModeEEPromBase()+EEPromAddressOffsets(0);
		ee24_write_32(addr, (uint32_t)shiftDirCycle, 1000);
	}
	else if(button == 5)
	{
		doublePluckEnable = !doublePluckEnable;
		ee24_write_8(modeManager.CurrentModeEEPromBase()+EEPromAddressOffsets(2), (uint8_t)doublePluckEnable, 1000);
	}
}


bool Mode_SequencedPluck::WriteEEPROMState(uint32_t &ee_address)
{
	bool success = Mode::WriteEEPROMState(ee_address);

	ee24_write_32(ee_address, (uint32_t)shiftDirCycle, 1000);
	ee_address += sizeof(uint32_t);

	success &= ee24_write_32(ee_address, (uint32_t)strumDirection, 1000);
	ee_address += sizeof(uint32_t);

	success &= ee24_write_8(ee_address, (uint8_t)doublePluckEnable, 1000);
	ee_address += sizeof(uint8_t);

	success &= ee24_write_32(ee_address, (uint32_t)numPluckNotes, 1000);
	ee_address += sizeof(uint32_t);

	success &= WriteNoteData(ee_address);

	return success;
}


bool Mode_SequencedPluck::ReadEEPROMState(uint32_t &ee_address)
{
	bool success = Mode::ReadEEPROMState(ee_address);

	success &= ee24_read_32(ee_address, (uint32_t*)&shiftDirCycle, 1000);
	ee_address += sizeof(uint32_t);


	success &= ee24_read_32(ee_address, (uint32_t*)&strumDirection, 1000);
	if(strumDirection != -1 || strumDirection != 1)
		strumDirection = 1;
	ee_address += sizeof(uint32_t);


	uint8_t dpe;
	success &= ee24_read_8(ee_address, (uint8_t*)&dpe, 1000); doublePluckEnable = bool(dpe);
	ee_address += sizeof(uint8_t);

	success &= ee24_read_32(ee_address, (uint32_t*)&numPluckNotes, 1000);
	numPluckNotes = MathExtras::ClampInclusive(numPluckNotes, 4, MODE_SP_MAX_PLUCKNOTES);
	ee_address += sizeof(uint32_t);


	success &= ReadNoteData(ee_address);

	UpdateShiftDirMultFromCycle();


	return success;
}

bool Mode_SequencedPluck::WriteNoteData(uint32_t &ee_address)
{
	bool success = true;
	for(int i = 0; i < MODE_SP_MAX_PLUCKNOTES; i++)
	{
		success &= ee24_write_float(ee_address, notes[i].volts, 1000);
		ee_address += sizeof(float);
	}
	return success;
}


bool Mode_SequencedPluck::ReadNoteData(uint32_t &ee_address)
{
	bool success = true;
	for(int i = 0; i < MODE_SP_MAX_PLUCKNOTES; i++)
	{
		success &= ee24_read_float(ee_address, &notes[i].volts, 1000);
		ee_address += sizeof(float);
	}
	return success;
}

