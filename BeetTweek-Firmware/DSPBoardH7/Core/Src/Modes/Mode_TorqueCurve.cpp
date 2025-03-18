/*
 * Mode_TorqueCurve.cpp
 *
 *  Created on: Nov 16, 2021
 *      Author: casht
 */


#include "Modes/Mode_TorqueCurve.h"
#include "arm_math.h"

//do setup things when the mode is entered
void Mode_TorqueCurve::Initialize()
{
	Mode::Initialize();



	inputOutputDescriptors[0].enabled = true;
	inputOutputDescriptors[0].numAugments = 1;
	inputOutputDescriptors[0].augments[0].baseColor = MathExtras::Color::ORANGE;
	inputOutputDescriptors[0].augments[0].defaultVoltVal = 0.0f;


	inputOutputDescriptors[1].enabled = true;
	inputOutputDescriptors[1].numAugments = 1;
	inputOutputDescriptors[1].prefferedAugment = 0;
	inputOutputDescriptors[1].augments[0].baseColor = MathExtras::Color::ORANGE;
	inputOutputDescriptors[1].augments[0].defaultVoltVal = 0.0f;



	inputOutputDescriptors[4].enabled = true;
	inputOutputDescriptors[4].augments[0].baseColor = MathExtras::Color::YELLOW;

	inputOutputDescriptors[5].enabled = true;
	inputOutputDescriptors[5].augments[0].baseColor = MathExtras::Color::RED;

	inputOutputDescriptors[6].enabled = true;
	inputOutputDescriptors[6].augments[0].baseColor = MathExtras::Color::BLUE;

	inputOutputDescriptors[7].enabled = false;
	inputOutputDescriptors[7].augments[0].baseColor = MathExtras::Color::MAGENTA;





	zeroSensor.SetThreshold(0.0f);
	zeroSensor.SetThresholdHalfGap(0.5f);

	//buffer1.looping = false;.
	driveIndicatorLP.filterCoeff = 10.0f;

	buffer1.Reset(0);


}

//un-setup thing in prep for next mode
void Mode_TorqueCurve::UnInitialize()
{
	Mode::UnInitialize();
}

//updates the front panel
void Mode_TorqueCurve::UpdateLEDS(float sampleTime)
{
	WS2812_SETALLLED(0,0,0);


		for(float clkangle = 0.0f; clkangle < 1.0f; clkangle += 1.0f/LED_NUM_LEDS_PER_RING){

			//int bufferIdx = clkangle*MODE_TORQUECURVE_BUFFERSIZE;
			float bufferVal = buffer1.GetInterpolated(clkangle);
			float torquePos = MathExtras::ClampInclusive(bufferVal, 0.0f, 1.0f);
			float torqueNeg = MathExtras::ClampInclusive(bufferVal, -1.0f, 0.0f)*-1.0f;
			LEDManager.SetLEDOuterRing_Float(-clkangle,  torqueNeg*LED_BASE_BRIGHTNESS_255,torquePos*LED_BASE_BRIGHTNESS_255,LED_BASE_BRIGHTNESS_255);
		}




//		for(int i = 0; i < MODE_TORQUECURVE_BUFFERSIZE; i++)
//		{
//			buffer2[i] = (buffer1.Get(i));
//			//buffer2[i] += 10.0f*sampleTime*(buffer1.Get(i)-buffer2[i]);
//
//	//		if(MathExtras::Abs(buffer1.Get(i) - buffer2[i]) > 0.5f)
//	//		{
//	//			buffer2[i] = (buffer1.Get(i));
//	//		}
//		}
		//doCopyBuffer=false;


	LEDManager.SetLEDInnerRing_Float(-LocalMotorAngleState.currentAngle,  0, LED_BASE_BRIGHTNESS_255, 0);

	StandardPanelLEDDraw();
}

//Called Very Fast.  Holds DSP Code
void Mode_TorqueCurve::AudioDSPFunction(float sampleTime, int bufferSwap)
{
	Mode::AudioDSPFunction(sampleTime,bufferSwap);

	sigHP.filterCoeff = tempo.bps_filtered*0.01f;

	sigHP.Process(adcVolts[0]+adcVolts[1], sampleTime);


	//0.99967025f correction factor...
	//samplesInPeriod = tempo.SamplesInPeriod(sampleTime);
	//float stride = 0.99967025f*MODE_TORQUECURVE_BUFFERSIZE/samplesInPeriod;
	//bidx = MathExtras::WrapMax(bidx+stride, float(MODE_TORQUECURVE_BUFFERSIZE));
	float torque = 2.0f*sigHP.val_filtered/(sigMax-sigMin);

	float curTorque = buffer1.GetInterpolated(tempo.PercToNextTap());

	float newValue = curTorque + 0.1f*(torque - curTorque);

	bidx_1 = buffer1.SetForwardInterp(MathExtras::ClampInclusive(newValue, -1.0f, 1.0f), tempo.PercToNextTap(), bidx_1);


	//slowly bring max and min down to ER_CV levels if allowed
	sigMax += sampleTime*(ER_SS_VOLTAGE_MAX - sigMax);
	sigMin += sampleTime*(ER_SS_VOLTAGE_MIN - sigMin);


	if(sigHP.val_filtered > sigMax)
		sigMax = sigHP.val_filtered;
	else if(sigHP.val_filtered < sigMin)
		sigMin = sigHP.val_filtered;



	//count zero crossings
	zeroSensor.Process(torque);
	if(zeroSensor.Triggered())
		zeroCrossings++;

	if(tempo.Tapped() )//reset on sync
	{
		zeroCrossings = 0;
		bEndingIdx = bidx;
	}


	SetDacVolts(0, driveIndicatorLP.val_filtered*ER_CV_VOLTAGE_MAX);
	SetDacVolts(1, MotorAngleToVolts(LocalMotorAngleState.currentAngle));


	float theta = LocalMotorAngleState.currentAngle*M_TWO_PI;
	float curOutputSin = arm_sin_f32(theta);
	SetDacVolts(2, MotorAngleToVolts(curOutputSin));


	SetDacVolts(3,tempo.PercToNextTap()*ER_GATE_THRESH_VOLT*2);
}

//Called Somewhat Fast.  Holds Knob Update Processing.
void Mode_TorqueCurve::KnobDSPFunction(float sampleTime)
{
	Mode::KnobDSPFunction(sampleTime);

	//driveTorque = buffer1.Get(int(LocalMotorAngleState.currentAngle*(MODE_TORQUECURVE_BUFFERSIZE-1)));
	//float drive = buffer2[int(LocalMotorAngleState.currentAngle*(MODE_TORQUECURVE_BUFFERSIZE-1))];
	float drive = buffer1.GetInterpolated(LocalMotorAngleState.currentAngle);


	driveIndicatorLP.Process(drive, sampleTime);
	driveTorque = drive;

	//count zero Crossings in the buffer, use this as metric for "signal busy-ness"
	float busyFactor = MathExtras::ClampInclusive(float(zeroCrossings)/MODE_TORQUECURVE_DENSEZEROCROSSING_CNT, 0.0f, 1.0f);




	//add some friction based on busyFactor
	float frictionTerm = 0.2f*busyFactor;
	driveTorque = driveTorque*(1.0f - frictionTerm) - (LocalMotorAngleState.currentAngularVelFiltered/MAX_RPM_FULL_TORQUE)*frictionTerm*20.0f;


}


//call to tell the mode to handle the button down.
void Mode_TorqueCurve::ButtonDown(int button)
{
	Mode::ButtonDown(button);
}

//call to tell the mode to handle the button up.
void Mode_TorqueCurve::ButtonUp(int button)
{
	Mode::ButtonUp(button);
}

void Mode_TorqueCurve::OnFuncCombo(int button)
{
	Mode::OnFuncCombo(button);
}

//Prints Debug Information
void Mode_TorqueCurve::DebugPrint()
{
	Mode::DebugPrint();

	// printf("%d,%f\r\n",int(bEndingIdx), tempo.tNext);
}

void Mode_TorqueCurve::OnPostCycleAugment(int buttonIdx, int augmentIdx)
{
	Mode::OnPostCycleAugment(buttonIdx,augmentIdx);
}

bool Mode_TorqueCurve::WriteEEPROMState(uint32_t &ee_address)
{
	return Mode::WriteEEPROMState(ee_address);
}

bool Mode_TorqueCurve::ReadEEPROMState(uint32_t &ee_address)
{
	return Mode::ReadEEPROMState(ee_address);
}

