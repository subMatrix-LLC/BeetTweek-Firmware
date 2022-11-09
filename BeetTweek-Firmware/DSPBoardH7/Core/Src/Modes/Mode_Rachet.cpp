/*
 * Mode_Rachet.cpp
 *
 *  Created on: Jan 9, 2021
 *      Author: casht
 */




#include "Modes/Mode_Rachet.h"

#include "EuroRack.h"


Mode_Rachet::Mode_Rachet() : Mode()
{
	SetBothNames("Mode_Rachet");
}
Mode_Rachet::~Mode_Rachet()
{

}

void Mode_Rachet::Initialize()
{
	Mode::Initialize();
	ratchetAngle = MotorAngleStateGetCurrentAccumulatedAnglef(&LocalMotorAngleState);


	//make all variants have same scheme.
	inputOutputDescriptors[0].enabled = true;
	inputOutputDescriptors[0].augments[0].baseColor = MathExtras::Color::YELLOW;
	inputOutputDescriptors[0].augments[0].defaultVoltVal = -1.0f;

	inputOutputDescriptors[1].enabled = true;
	inputOutputDescriptors[1].augments[0].baseColor = MathExtras::Color::MAGENTA;
	inputOutputDescriptors[1].augments[0].signalType = EuroRack::SignalType_GATE;
	inputOutputDescriptors[1].augments[0].gateTrigger = &directionGate;
	inputOutputDescriptors[1].funcCombo = true;

//		inputOutputDescriptors[3].enabled = true;
//		inputOutputDescriptors[3].signalType = EuroRack::SignalType_GATE5;
//		inputOutputDescriptors[3].signalBehalvior = EuroRack::SignalBehalvior_NormalLinear_Inverted;
//		inputOutputDescriptors[3].ledColor = MathExtras::Color::YELLOW;
//


	inputOutputDescriptors[4].enabled = true;
	inputOutputDescriptors[4].augments[0].baseColor = MathExtras::Color::RED;

	inputOutputDescriptors[5].enabled = true;
	inputOutputDescriptors[5].augments[0].baseColor = MathExtras::Color::CYAN;
	inputOutputDescriptors[5].augments[0].signalBehalvior = EuroRack::SignalBehalvior_NormalLinear;
	inputOutputDescriptors[5].augments[0].signalType = EuroRack::SignalType_CV;


	inputOutputDescriptors[6].enabled = true;
	inputOutputDescriptors[6].augments[0].baseColor = MathExtras::Color::BLUE;

	inputOutputDescriptors[7].enabled = true;
	inputOutputDescriptors[7].augments[0].baseColor = MathExtras::Color::MAGENTA;





	directionGate.SetThreshold(ER_GATE_THRESH_VOLT);
	directionGate.SetThresholdHalfGap(ER_GATE_HALF_HIST_VOLT);



}

//updates the front panel
void Mode_Rachet::UpdateLEDS(float sampleTime)
{

	WS2812_SETALLLED(0,0,0);

	//basic led scheme
	float angleLEDRing = 1.0f - MotorAngleStateGetCurrentAccumulatedAnglef(&LocalMotorAngleState);
	float targetAngleLEDRing = 1.0f - (ratchetAngle);


	LEDManager.SetLEDRingRangeLinear_Float(LEDPanelManager::RINGIDENTIFIER_INNER, targetAngleLEDRing,angleLEDRing,
		(MathExtras::Color*)&MathExtras::Color::YELLOW,
		(MathExtras::Color*)&MathExtras::Color::RED,
		1/4.0f);

	//LEDManager.SetLEDOuterRing(0,  LED_BASE_BRIGHTNESS_255,LED_BASE_BRIGHTNESS_255,LED_BASE_BRIGHTNESS_255);

	LEDManager.SetLEDOuterRing_Float(angleLEDRing,  0,0,LED_BASE_BRIGHTNESS_255);


	StandardPanelLEDDraw();
}

//Called Very Fast.  Holds DSP Code
void Mode_Rachet::AudioDSPFunction(float sampleTime, int bufferSwap)
{
	float accumAngle = MotorAngleStateGetCurrentAccumulatedAnglef(&LocalMotorAngleState);

	//put distance on first output
	float distance = (ratchetAngle - accumAngle);
	SetDacVolts(0, MotorAngleToVolts(distance*2.0f));

	//Non-linear distance on 2nd output
	SetDacVolts(1, (MathExtras::Ln(distance + 0.5f)+0.693147f)*2.5f);


	//Sin of angle difference
	float curOutputSin = arm_sin_f32(distance*M_PI_2*16.0f);
	SetDacVolts(2, curOutputSin*ER_CV_VOLTAGE_MAX);

	SetDacVolts(3, LocalMotorAngleState.currentAngularVelFiltered*1.0f);

}


//Called Somewhat Fast.  Holds Knob Update Processing.
void Mode_Rachet::KnobDSPFunction(float sampleTime)
{
	float accumAngle = MotorAngleStateGetCurrentAccumulatedAnglef(&LocalMotorAngleState);

	float inputTerm = MathExtras::ClampInclusive(adcVolts[0], 0.0f, EuroRack::SignalVoltMax(inputOutputDescriptors[0].CurAugment().signalType));


	directionGate.Process(adcVolts[1]);
	if(ADCPluggedIn(1))
		direction = int(!directionGate.TriggerLevel());

	if(direction == 1)
	{
		if(accumAngle > ratchetAngle)
		{
			ratchetAngle = accumAngle;

			driveTorque = 0.0f;

			//apply friction model from 2nd input
			//driveTorque = -motorAngleState.currentAngularVelFiltered*0.5f*0.25;//(1.0f-adcIns[1]/float(0xFFFF));
		}
		else
		{
			//the knob is in torque..

			//slowly retract at speed based off of input

			//ratchetAngle -= (ratchetAngle - accumAngle)*0.1*sampleTime*driveTorque;
			const float deadZone = 0.02f;
			if(MathExtras::Abs(ratchetAngle - accumAngle) > deadZone)
			{

				ratchetAngle -= MathExtras::Sign(ratchetAngle - accumAngle)*0.5*inputTerm*sampleTime;
			}

			if(accumAngle <= ratchetAngle - (deadZone-0.01f))
			{
				driveTorque = (ratchetAngle - (deadZone-0.01f) - accumAngle)*200.0f;
			}
			else
			{
				driveTorque = 0.0f;
			}
		}

	}
	else
	{
		if(accumAngle < ratchetAngle)
		{
			ratchetAngle = accumAngle;

			driveTorque = 0.0f;

		}
		else
		{
			//the knob is in torque..

			//slowly retract at speed based off of input

			//ratchetAngle -= (ratchetAngle - accumAngle)*0.1*sampleTime*driveTorque;
			const float deadZone = 0.02f;
			if(MathExtras::Abs(ratchetAngle - accumAngle) > deadZone)
			{
				ratchetAngle -= MathExtras::Sign(ratchetAngle - accumAngle)*0.5*inputTerm*sampleTime;
			}

			if(accumAngle >= ratchetAngle + (deadZone-0.01f))
			{
				driveTorque = (ratchetAngle + (deadZone-0.01f) - accumAngle)*200.0f;
			}
			else
			{
				driveTorque = 0.0f;
			}
		}

	}
	if(isnan(ratchetAngle))
	{
		strcpy(errorCode, "RachetMode NAN Angle");
		Error_Handler();
	}


	MasterProcess_Torque(sampleTime);

}


void Mode_Rachet::ButtonUp(int button)
{

}

void Mode_Rachet::OnButtonQuickAction(int button)
{
	Mode::OnButtonQuickAction(button);
}
void Mode_Rachet::OnFuncCombo(int button)
{
	Mode::OnFuncCombo(button);
	if(button == 1)
	{
		direction = -direction;
		ee24_write_32(modeManager.CurrentModeEEPromBase()+EEPromAddressOffsets(0), (uint32_t)direction, 1000);
	}
}


bool Mode_Rachet::WriteEEPROMState(uint32_t &ee_address)
{
	bool success = Mode::WriteEEPROMState(ee_address);

	ee24_write_32(ee_address, (uint32_t)direction, 1000);
	ee_address += sizeof(int);

	return success;
}

bool Mode_Rachet::ReadEEPROMState(uint32_t &ee_address)
{
	bool success = Mode::ReadEEPROMState(ee_address);

	success &= ee24_read_32(ee_address, (uint32_t*)&direction, 1000);
	if(direction != 1 && direction != -1)
		direction = 1;
	ee_address += sizeof(int);



	return success;
}
