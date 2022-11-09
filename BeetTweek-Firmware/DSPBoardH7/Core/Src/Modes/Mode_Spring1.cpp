#include "Modes/Mode_Spring1.h"
#include "EuroRack.h"

#include "Modes/Mode.h"

#include "arm_math.h"



Mode_Spring1::~Mode_Spring1()
{

}


//do setup things when the mode is entered
void Mode_Spring1::Initialize()
{
	Mode::Initialize();


	//make all variants have same scheme.

	inputOutputDescriptors[0].enabled = true;
	inputOutputDescriptors[0].numAugments = 2;
	inputOutputDescriptors[0].augments[0].baseColor = MathExtras::Color::GREEN;
	inputOutputDescriptors[0].augments[0].defaultVoltVal = 0.0f;
	inputOutputDescriptors[0].augments[0].signalType = EuroRack::SignalType_CV;
	inputOutputDescriptors[0].augments[1].baseColor = MathExtras::Color::GREEN;
	inputOutputDescriptors[0].augments[1].defaultVoltVal = 0.0f;
	inputOutputDescriptors[0].augments[1].signalType = EuroRack::SignalType_LRGSIGNAL;




	inputOutputDescriptors[0].augments[0].baseColor = MathExtras::Color::GREEN;
	inputOutputDescriptors[0].augments[0].defaultVoltVal = 0.0f;


	inputOutputDescriptors[1].enabled = true;
	inputOutputDescriptors[1].numAugments = 3;
	inputOutputDescriptors[1].augments[0].baseColor = MathExtras::Color::MAGENTA;
	inputOutputDescriptors[1].augments[0].signalType = EuroRack::SignalType_GATE;
	inputOutputDescriptors[1].augments[0].defaultVoltVal = 0.0f;
	inputOutputDescriptors[1].augments[0].signalBehalvior = EuroRack::SignalBehalvior_NormalLinear;
	inputOutputDescriptors[1].augments[0].gateTrigger = &Gate;

	inputOutputDescriptors[1].augments[1].baseColor = MathExtras::Color::YELLOW;
	inputOutputDescriptors[1].augments[1].signalType = EuroRack::SignalType_LRGSIGNAL;
	inputOutputDescriptors[1].augments[1].signalBehalvior = EuroRack::SignalBehalvior_NormalLinear;

	inputOutputDescriptors[1].augments[2].baseColor = MathExtras::Color::YELLOW;
	inputOutputDescriptors[1].augments[2].signalType = EuroRack::SignalType_SMSIGNAL_1;
	inputOutputDescriptors[1].augments[2].signalBehalvior = EuroRack::SignalBehalvior_NormalLinear;


	inputOutputDescriptors[4].enabled = true;
	inputOutputDescriptors[4].augments[0].baseColor = MathExtras::Color::BLUE;


	inputOutputDescriptors[5].enabled = true;
	inputOutputDescriptors[5].funcCombo = true;
	inputOutputDescriptors[5].augments[0].baseColor = MathExtras::Color::RED;
	inputOutputDescriptors[5].augments[1].baseColor = MathExtras::Color::RED;
	inputOutputDescriptors[5].augments[2].baseColor = MathExtras::Color::RED;
	inputOutputDescriptors[5].numAugments = 3;


	inputOutputDescriptors[6].enabled = true;
	inputOutputDescriptors[6].augments[0].baseColor = MathExtras::Color::RED;




	Gate.SetThreshold(ER_GATE_THRESH_VOLT);
	Gate.SetThresholdHalfGap(ER_GATE_HALF_HIST_VOLT);

}

//un-setup thing in prep for next mode
void Mode_Spring1::UnInitialize()
{

}

void Mode_Spring1::UpdateLEDS(float sampleTime)
{
	WS2812_SETALLLED(0,0,0);


	float targetAngleLEDRing = -targetAngle;

	//cap led rotation rate at some multiple of actual freq, maxed to 1/8  update rate.
	float capUpdateRateFactor = 0.125f;

	if(inputOutputDescriptors[5].curAugment == 0)
	{

		DrawErrorRing(targetAngleLEDRing, -snapFilteredMotorAngle);

//		if(MathExtras::Abs(driveTorque) > 0.5f)
//		{
//			LEDManager.SetLEDRingRangeLinear_Float(LEDPanelManager::RINGIDENTIFIER_OUTER, angleTotal-driveTorque*(1.0f/8.0f), angleTotal,
//					(MathExtras::Color*)&MathExtras::Color::YELLOW,
//					(MathExtras::Color*)&MathExtras::Color::YELLOW,
//					1/8.0f);
//		}



		StandardPanelLEDDraw();
//

	}
	else if(inputOutputDescriptors[5].curAugment == 1)
	{

		curLFO1OutputFreqLEDCapped = MathExtras::ClampInclusive(curLFO1OutputFreq, -capUpdateRateFactor/sampleTime, capUpdateRateFactor/sampleTime);
		float Percent1SLOW = (MathExtras::Abs(capUpdateRateFactor/sampleTime) - MathExtras::Abs(curLFO1OutputFreqLEDCapped))/MathExtras::Abs(capUpdateRateFactor/sampleTime);


		float theta1LEDCapped = timeAccum1LEDCapped*M_TWO_PI;
		curOsc1OutputSinLEDCapped = arm_sin_f32(theta1LEDCapped);
		curOsc1OutputCosLEDCapped = arm_sin_f32(M_PI_2 + theta1LEDCapped);

		float angle1LEDRad = MathExtras::WrapMinMax(theta1LEDCapped, 0.0f, M_TWO_PI);

		//advance led capped time - if can actually keep up with the real signal, add in proportional term to value error.
		//float error = MathExtras::AngleLocalDifferenceRad(angle2Rad,angle2LEDRad);
		float error1 = MathExtras::AngleLocalDifferenceRad(MathExtras::WrapMinMax(theta1, 0.0f, M_TWO_PI), MathExtras::WrapMinMax(theta1LEDCapped, 0.0f, M_TWO_PI));



		//simple P controller to correct phase offset.
		const float K1 = 2.0f;
		float speedCorrectionTerm1 = K1*sampleTime*(error1);
		timeAccum1LEDCapped += sampleTime*curLFO1OutputFreqLEDCapped + speedCorrectionTerm1*Percent1SLOW;
		timeAccum1LEDCapped = MathExtras::WrapMinMax(timeAccum1LEDCapped, 0.0f, 1.0f);


		float clock1AngleLEDCapped = (angle1LEDRad - M_PI_2)*M_RADTOPERC;


		/////////////////////////////////////////////



		//compute a clock "span" to visualize motion blur
		float clock1Span = (sampleTime*curLFO1OutputFreq)/(1.0f);

		clock1Span = MathExtras::ClampInclusive(clock1Span, -1.0f, 1.0f);


		MathExtras::Color color1 = inputOutputDescriptors[5].CurAugment().baseColor;
		color1.FromHSL(  MathExtras::WrapMax(color1.ToHSL().x_ + EuroRack::FREQ_TO_HUE_OFFSET(MathExtras::Abs(curLFO1OutputFreq)) , 1.0f), 1.0f, 0.5f);




		LEDManager.SetLEDRingRangeLinear_Float(LEDPanelManager::RINGIDENTIFIER_INNER, clock1AngleLEDCapped, clock1AngleLEDCapped + clock1Span,
						&color1,
						&color1,
						1.0f);



		//draw current torque, and green target
		DrawErrorRing(targetAngleLEDRing, -snapFilteredMotorAngle);

		LEDManager.SetLEDOuterRing_Float(targetAngleLEDRing,  0, LED_BASE_BRIGHTNESS_255, 0);


		StandardPanelLEDDraw();
	}
	else if(inputOutputDescriptors[5].curAugment == 2)
	{
		curLFO2OutputFreqLEDCapped = MathExtras::ClampInclusive(curLFO2OutputFreq, -capUpdateRateFactor/sampleTime, capUpdateRateFactor/sampleTime);
		float Percent2SLOW = (MathExtras::Abs(capUpdateRateFactor/sampleTime) - MathExtras::Abs(curLFO2OutputFreqLEDCapped))/MathExtras::Abs(capUpdateRateFactor/sampleTime);

		float theta2LEDCapped = timeAccum2LEDCapped*M_TWO_PI;
		curOsc2OutputSinLEDCapped = arm_sin_f32(theta2LEDCapped);
		curOsc2OutputCosLEDCapped = arm_sin_f32(M_PI_2 + theta2LEDCapped);


		float angle2LEDRad = MathExtras::WrapMinMax(theta2LEDCapped, 0.0f, M_TWO_PI);



		//advance led capped time - if can actually keep up with the real signal, add in proportional term to value error.
		//float error = MathExtras::AngleLocalDifferenceRad(angle2Rad,angle2LEDRad);
		float error2 = MathExtras::AngleLocalDifferenceRad(MathExtras::WrapMinMax(theta2, 0.0f, M_TWO_PI), MathExtras::WrapMinMax(theta2LEDCapped, 0.0f, M_TWO_PI));



		//simple P controller to correct phase offset of capped frequency LEDS.
		const float K2 = 2.0f;
		float speedCorrectionTerm2 = K2*sampleTime*(error2);
		timeAccum2LEDCapped += sampleTime*curLFO2OutputFreqLEDCapped + speedCorrectionTerm2*Percent2SLOW;
		timeAccum2LEDCapped = MathExtras::WrapMinMax(timeAccum2LEDCapped, 0.0f, 1.0f);



		float clock2AngleLEDCapped = (angle2LEDRad - M_PI_2)*M_RADTOPERC;

		float clock2Span = (sampleTime*curLFO2OutputFreq)/(1.0f);
		clock2Span = MathExtras::ClampInclusive(clock2Span, -1.0f, 1.0f);

		MathExtras::Color color2 = inputOutputDescriptors[5].CurAugment().baseColor;
		color2.FromHSL(  MathExtras::WrapMax(color2.ToHSL().x_ + EuroRack::FREQ_TO_HUE_OFFSET(MathExtras::Abs(curLFO2OutputFreq)) , 1.0f), 1.0f, 0.5f);

		MathExtras::Color color2Dark = color2;
		color2Dark.FromHSL( color2Dark.ToHSL().x_, 1.0f, 0.1f);

		LEDManager.SetLEDRingRangeLinear_Float(LEDPanelManager::RINGIDENTIFIER_INNER, clock2AngleLEDCapped, clock2AngleLEDCapped + clock2Span,
						&color2,
						&color2Dark,
						1.0f);



		{
		DrawErrorRing(targetAngleLEDRing, -snapFilteredMotorAngle);

		LEDManager.SetLEDOuterRing_Float(targetAngleLEDRing,  0, LED_BASE_BRIGHTNESS_255, 0);



		StandardPanelLEDDraw();
		}
	}


	LEDManager.SetLEDOuterRing_Float(-snapFilteredMotorAngle,        inputOutputDescriptors[4].CurAugment().baseColor.r_*LED_BASE_BRIGHTNESS_255,
			inputOutputDescriptors[4].CurAugment().baseColor.g_*LED_BASE_BRIGHTNESS_255,
			inputOutputDescriptors[4].CurAugment().baseColor.b_*LED_BASE_BRIGHTNESS_255);
	LEDManager.SetLEDInnerRing_Float(targetAngleLEDRing,  0, LED_BASE_BRIGHTNESS_255, 0);


	//show extra signal
	if(curGestureLevel != GUESTURE_LEVEL_PLAY && (inputOutputDescriptors[1].curAugment > 0))//dont show yellow torque indicator when playing.
	{
		LEDManager.SetLEDRingRangeLinear_Float(LEDPanelManager::RINGIDENTIFIER_INNER, -snapFilteredMotorAngle-driveTrqMinInd*(1.0f/8.0f), -snapFilteredMotorAngle-driveTrqMaxInd*(1.0f/8.0f),
			(MathExtras::Color*)&MathExtras::Color::YELLOW,
			(MathExtras::Color*)&MathExtras::Color::YELLOW,
			1/4.0f);
	}


}
void Mode_Spring1::DrawErrorRing(float targetAngleLEDRing, float angleTotalLED)
{
	//draw current torque, and green target
	if(!Gate.TriggerLevel())
	{
		LEDManager.SetLEDRingRangeLinear_Float(LEDPanelManager::RINGIDENTIFIER_OUTER, targetAngleLEDRing, angleTotalLED,
			(MathExtras::Color*)&MathExtras::Color::YELLOW,
			(MathExtras::Color*)&MathExtras::Color::RED,
			0.25f);
	}
}

void Mode_Spring1::AudioDSPFunction(float sampleTime, int bufferSwap)
{
	if(inputOutputDescriptors[1].curAugment == 0)
		Gate.Process(adcVolts[1]);
	else
		Gate.Process(inputOutputDescriptors[1].augments[0].defaultVoltVal);


	//angle on first always
	float motorAngle = MotorAngleStateGetCurrentAccumulatedAnglef(&LocalMotorAngleState);

	if(MathExtras::Abs(motorAngle - targetAngFiltered)  < 0.04f)
		snapFilteredMotorAngle += 10*sampleTime*(targetAngFiltered - snapFilteredMotorAngle);
	else
		snapFilteredMotorAngle += 100*sampleTime*(motorAngle - snapFilteredMotorAngle);

	snapFilteredMotorAngleError = targetAngFiltered - snapFilteredMotorAngle;

	if(Gate.TriggerLevel())
	{
		snapFilteredMotorAngleError = 0.0f;
		snapFilteredMotorAngle = motorAngle;
	}


	SetDacVolts(0, MotorAngleToVolts(snapFilteredMotorAngle));

	if(inputOutputDescriptors[5].curAugment == 0)
	{

		//angle error
		SetDacVolts(1, MotorAngleToVolts(snapFilteredMotorAngleError));
		SetDacVolts(2, 0);


		inputOutputDescriptors[5].freq = 0;
		inputOutputDescriptors[6].freq = 0;

	}
	else if(inputOutputDescriptors[5].curAugment == 1)
	{
		const float freqPerKnobRotation = 60.0f;

		//put Sinusoid with frequency proportional to torque On First Output
		theta1 = timeAccum*M_TWO_PI*freqPerKnobRotation;
		curOsc1OutputSin = arm_sin_f32(theta1);
		curOsc1OutputCos = arm_sin_f32(M_PI_2 + theta1);



		SetDacVolts(1, curOsc1OutputCos*ER_CV_VOLTAGE_MAX);
		SetDacVolts(2, MotorAngleToVolts(MathExtras::WrapMinMax(theta1,- float(M_PI_2), float(M_PI_2))/float(M_PI_2)));



		timeAccum += sampleTime*(snapFilteredMotorAngleError*(1.0f+adcVolts[1]/ER_VOLTAGE_OUT_MAX));// + speedCorrectionTerm*(1.0f-deadZoneRemap);


		//keep floating precision
		timeAccum = MathExtras::WrapMinMax(timeAccum, 0.0f, 1.0f);


		//update current frequency
		curLFO1OutputFreq = freqPerKnobRotation*snapFilteredMotorAngleError*(1.0f+adcVolts[1]/ER_VOLTAGE_OUT_MAX);
		inputOutputDescriptors[5].freq = curLFO1OutputFreq;
		inputOutputDescriptors[6].freq = curLFO1OutputFreq;



	}
	else if(inputOutputDescriptors[5].curAugment == 2)
	{
		//put Sinusoid with frequency adjusted by torque

		curLFO2OutputFreq += snapFilteredMotorAngleError*sampleTime*10.0*(1.0f+adcVolts[1]/ER_VOLTAGE_OUT_MAX);
		inputOutputDescriptors[5].freq = curLFO2OutputFreq;
		inputOutputDescriptors[6].freq = curLFO2OutputFreq;



		theta2 = timeAccum2*M_TWO_PI;

		curOsc2OutputSin = arm_sin_f32(theta2 );
		curOsc2OutputCos = arm_sin_f32(M_PI_2 + theta2);

		SetDacVolts(1, MotorAngleToVolts(curOsc2OutputCos));
		SetDacVolts(2, MotorAngleToVolts(MathExtras::WrapMinMax(theta2,- float(M_PI_2), float(M_PI_2))/float(M_PI_2)));

		//Advance time proportional to frequency...
		timeAccum2 += sampleTime*curLFO2OutputFreq;

		//keep floating precision
		timeAccum2 = MathExtras::WrapMinMax(timeAccum2, 0.0f, 1.0f);



	}
}


void Mode_Spring1::KnobDSPFunction(float sampleTime)
{
	targetAngle = VoltsToMotorAngle(adcVolts[0], inputOutputDescriptors[0].CurAugment().signalType);
	targetAngFiltered += ANTIALIAS_FIRSTORDER_FILT_COEFF*sampleTime*(targetAngle - targetAngFiltered);

	motor_trgfiltered_angleError = targetAngFiltered-MotorAngleStateGetCurrentAccumulatedAnglef(&LocalMotorAngleState);
	if(!Gate.TriggerLevel())
	{
		//P(I,D) control
		const float Kp = 10.0f;
		const float Ki = 0.0000f;

		if(abs(motor_trgfiltered_angleError) < 0.1f)
		{
			integralAccumulator += motor_trgfiltered_angleError;
		}
		else
		{
			integralAccumulator = 0;
		}

		driveTorque = motor_trgfiltered_angleError*Kp + integralAccumulator*Ki;


	}
	else
		driveTorque = 0.0f;




	if(inputOutputDescriptors[1].curAugment >= 1)
	{
		//subtract off Y
		float subtracterSig = VoltsToMotorAngle(adcVolts[1], inputOutputDescriptors[1].CurAugment().signalType);
		driveTorque = MathExtras::ClampInclusive(driveTorque, -1.0f, 1.0f);//preclamp..
		float subtracted = -MathExtras::Abs(subtracterSig)*driveTorque;
		subtracted = MathExtras::ClampInclusive(subtracted, -1.0f, 1.0f);
		driveTorque += subtracted;

		//update min/max torque indicator for minimizing led aliasing
		driveTrqMaxInd += 20.0f*sampleTime*(MathExtras::Max(0.0f,subtracted) - driveTrqMaxInd);
		driveTrqMinInd += 20.0f*sampleTime*(MathExtras::Min(0.0f,subtracted) - driveTrqMinInd);
	}



	if(inputOutputDescriptors[5].curAugment == 0)
	{

	}
	else if(inputOutputDescriptors[5].curAugment == 1)
	{
		//add in oscillations from the oscillators
		float activationCurve1 = MathExtras::LinearRamp_1<float>(MathExtras::Abs(curLFO1OutputFreq)-10.0f, 10.0f);


		//curve for activating only when base drive torque is near max (-1 or 1)
		float activationCurve3 = MathExtras::LinearRamp_1<float>(MathExtras::Abs(driveTorque)-0.8f, 0.2f);


		driveTorque = MathExtras::ClampInclusive(driveTorque, -1.0f, 1.0f);
		float sign = MathExtras::Sign(driveTorque);
		float osc1Signal = (curOsc1OutputSin+1.0f)*0.5f;


		driveTorque = driveTorque - sign*osc1Signal*0.5f*activationCurve1*activationCurve3;
	}
	else if(inputOutputDescriptors[5].curAugment == 2)
	{
		float activationCurve2 = MathExtras::LinearRamp_1<float>(MathExtras::Abs(curLFO2OutputFreq)-10.0f, 10.0f);

		//curve for activating only when base drive torque is near max (-1 or 1)
		float activationCurve3 = MathExtras::LinearRamp_1<float>(MathExtras::Abs(driveTorque)-0.8f, 0.2f);


		driveTorque = MathExtras::ClampInclusive(driveTorque, -1.0f, 1.0f);
		float sign = MathExtras::Sign(driveTorque);
		float osc2Signal = (curOsc2OutputSin+1.0f)*0.5f;

		driveTorque = driveTorque - sign*osc2Signal*0.5f*activationCurve2*activationCurve3;
	}


	MasterProcess_Torque(sampleTime);

}

void Mode_Spring1::OnPostCycleAugment(int buttonIdx, int augmentIdx)
{
	inputOutputDescriptors[buttonIdx].freq = 0.0;

}


void Mode_Spring1::ButtonDown(int button)
{

}


void Mode_Spring1::ButtonUp(int button)
{
}

void Mode_Spring1::OnFuncCombo(int button)
{
	Mode::OnFuncCombo(button);


	if(button == 5)
	{
		curLFO2OutputFreq = 0.0f;//reset frequency.
	}
}


bool Mode_Spring1::WriteEEPROMState(uint32_t &ee_address)
{
	bool success = Mode::WriteEEPROMState(ee_address);

	success &= ee24_write_float(ee_address, timeAccum, 1000);
	ee_address += sizeof(float);

	success &= ee24_write_float(ee_address, timeAccum2, 1000);
	ee_address += sizeof(float);

	success &= ee24_write_float(ee_address, curLFO2OutputFreq, 1000);
	ee_address += sizeof(float);

	return success;
}


bool Mode_Spring1::ReadEEPROMState(uint32_t &ee_address)
{
	bool success = Mode::ReadEEPROMState(ee_address);

	success &= ee24_read_float(ee_address, &timeAccum, 1000);
	ee_address += sizeof(float);


	success &= ee24_read_float(ee_address, &timeAccum2, 1000);
	ee_address += sizeof(float);

	success &= ee24_read_float(ee_address, &curLFO2OutputFreq, 1000);
	ee_address += sizeof(float);

	return success;
}



void Mode_Spring1::DebugPrint()
{
	Mode::DebugPrint();
	//printf("%f\r\n", MotorAngleStateGetCurrentAccumulatedAnglef(&LocalMotorAngleState));
	//printf("%f,%f,%f,%f\r\n", motor_trgfiltered_angleError,targetAngle,MotorAngleStateGetCurrentAccumulatedAnglef(&LocalMotorAngleState), adcVolts[0]);
	//printf("%u\r\n",dacOuts[0]);

	//printf("%d,%d\r\n", MotorSpiOutData_Latest,MotorSpiInData_Latest);
}
