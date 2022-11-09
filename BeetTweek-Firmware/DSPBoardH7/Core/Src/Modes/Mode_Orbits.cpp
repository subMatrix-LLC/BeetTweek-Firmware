/*
 * Mode_Orbits.cpp
 *
 *  Created on: Apr 11, 2022
 *      Author: casht
 */




#include "Modes/Mode_Orbits.h"
#include "MathExtras_Curves.h"

Mode_Orbits::Mode_Orbits() {

}

Mode_Orbits::~Mode_Orbits() {

}

void Mode_Orbits::Initialize() {
	Mode::Initialize();

	inputOutputDescriptors[0].enabled = true;
	inputOutputDescriptors[0].numAugments = 1;
	inputOutputDescriptors[0].quickAction = true;
	inputOutputDescriptors[0].augments[0].baseColor = MathExtras::Color::GREEN;
	inputOutputDescriptors[0].augments[0].defaultVoltVal = 1.0f;

	inputOutputDescriptors[1].enabled = true;
	inputOutputDescriptors[1].numAugments = 1;
	inputOutputDescriptors[1].augments[0].baseColor = MathExtras::Color::CYAN;
	inputOutputDescriptors[1].augments[0].defaultVoltVal = 0.0f;

	inputOutputDescriptors[4].enabled = true;
	inputOutputDescriptors[4].numAugments = 4 ;
	inputOutputDescriptors[4].augments[0].baseColor = MathExtras::Color::RED;
	inputOutputDescriptors[4].augments[1].baseColor = MathExtras::Color::YELLOW;
	inputOutputDescriptors[4].augments[2].baseColor = MathExtras::Color::BLUE;
	inputOutputDescriptors[4].augments[3].baseColor = MathExtras::Color::MAGENTA;
	for(int i = 0; i < 4; i++)
		inputOutputDescriptors[4].augments[i].signalType = EuroRack::SignalType_CV;

	inputOutputDescriptors[5].enabled = true;
	inputOutputDescriptors[5].numAugments = 4;
	inputOutputDescriptors[5].augments[0].baseColor = MathExtras::Color::RED;
	inputOutputDescriptors[5].augments[1].baseColor = MathExtras::Color::YELLOW;
	inputOutputDescriptors[5].augments[2].baseColor = MathExtras::Color::BLUE;
	inputOutputDescriptors[5].augments[3].baseColor = MathExtras::Color::MAGENTA;
	for(int i = 0; i < 4; i++)
		inputOutputDescriptors[5].augments[i].signalType = EuroRack::SignalType_CV;

	inputOutputDescriptors[6].enabled = true;
	inputOutputDescriptors[6].numAugments = 4;
	inputOutputDescriptors[6].augments[0].baseColor = MathExtras::Color::RED;
	inputOutputDescriptors[6].augments[1].baseColor = MathExtras::Color::YELLOW;
	inputOutputDescriptors[6].augments[2].baseColor = MathExtras::Color::BLUE;
	inputOutputDescriptors[6].augments[3].baseColor = MathExtras::Color::MAGENTA;
	for(int i = 0; i < 4; i++)
		inputOutputDescriptors[6].augments[i].signalType = EuroRack::SignalType_CV;

	inputOutputDescriptors[7].enabled = true;
	inputOutputDescriptors[7].numAugments = 4;
	inputOutputDescriptors[7].augments[0].baseColor = MathExtras::Color::RED;
	inputOutputDescriptors[7].augments[1].baseColor = MathExtras::Color::YELLOW;
	inputOutputDescriptors[7].augments[2].baseColor = MathExtras::Color::BLUE;
	inputOutputDescriptors[7].augments[3].baseColor = MathExtras::Color::MAGENTA;
	for(int i = 0; i < 4; i++)
		inputOutputDescriptors[7].augments[i].signalType = EuroRack::SignalType_CV;


	particles[0].angle = 0.0f;
	particles[0].mass  = 4.0f;


}

void Mode_Orbits::UpdateLEDS(float sampleTime) {
	WS2812_SETALLLED(0, 0, 0);

	MathExtras::Color c = inputOutputDescriptors[0].augments[0].baseColor;
	LEDManager.SetLEDOuterRing_Float(-LocalMotorAngleState.currentAngle,  c.r_*LED_BASE_BRIGHTNESS_255, c.g_*LED_BASE_BRIGHTNESS_255, c.b_*LED_BASE_BRIGHTNESS_255);

	for(int i = 0; i < 1; i++)
	{

			MathExtras::Color color = MathExtras::Color::CYAN;

			LEDManager.SetLEDRingRangeLinear_HalfWrap_Float(LEDPanelManager::RINGIDENTIFIER_OUTER, -particles[i].angleLED_1, -particles[i].angle, &color, &color, 0.5f);


			particles[i].angleLED_1 = particles[i].angle;
	}


	StandardPanelLEDDraw();
}

void Mode_Orbits::AudioDSPFunction(float sampleTime, int bufferSwap) {

	for(int i = 0; i < 4; i++)

		if(inputOutputDescriptors[i+4].curAugment == 0)
		{

			float dist = MathExtras::WrappedLocalDifference(LocalMotorAngleState.currentAngle, particles[0].angle, 1.0f);

			float signDist, cosDist;
			arm_sin_cos_f32(dist*360.0f, &signDist, &cosDist);

			float sig = signDist;
			SetDacVolts(i, sig*EuroRack::SignalVoltMax(inputOutputDescriptors[i].CurAugment().signalType));

		}
		else if(inputOutputDescriptors[i+4].curAugment == 1)
		{
			float sig = MathExtras::ClampInclusive(particles[0].vel*1000.0f, -1.0f, 1.0f);
			SetDacVolts(i, sig*EuroRack::SignalVoltMax(inputOutputDescriptors[i].CurAugment().signalType));
		}
		else if(inputOutputDescriptors[i+4].curAugment == 2)
		{
			float dist = MathExtras::WrappedLocalDifference(LocalMotorAngleState.currentAngle, particles[0].angle, 1.0f);
			float sig = MathExtras::ClampInclusive(dist, -1.0f, 1.0f);
			SetDacVolts(i, sig*EuroRack::SignalVoltMax(inputOutputDescriptors[i].CurAugment().signalType));
		}
		else if(inputOutputDescriptors[i+4].curAugment == 3)
		{
			float sig = MathExtras::ClampInclusive(-MathExtras::Abs(particles[0].force)*50.0f, -1.0f, 1.0f);
			sig = sig + 0.5;
			SetDacVolts(i, sig*EuroRack::SignalVoltMax(inputOutputDescriptors[i].CurAugment().signalType));
		}
}

void Mode_Orbits::KnobDSPFunction(float sampleTime) {


	float forceModifier = 1.0f;
	forceModifier = adcVolts[0];

	float freezeModifier = 0.0f;
	freezeModifier = adcVolts[1];


	float netForce = 0.0f;
	for(int i = 0; i < 1; i++)
	{
		float dist = MathExtras::WrappedLocalDifference(LocalMotorAngleState.currentAngle, particles[i].angle, 1.0f);
		float squaredDist = MathExtras::ClampMin(dist*dist, 0.01f);

		float catchment = 0.05f*particles[i].mass;
		float forceFunc = ((0.0001f*(particles[i].mass + 0.5f))/squaredDist) * (1.0f-MathExtras::CosCurveFilterF(dist, catchment))*forceModifier;
		particles[i].force = MathExtras::Sign(dist)*forceFunc - 0.1f*particles[i].vel*freezeModifier;



		netForce += particles[i].force;

		//f = ma
		//a = f/m
		float acc = particles[i].force/particles[i].mass;
		particles[i].vel += acc*sampleTime;
		particles[i].vel = MathExtras::ClampInclusive(particles[i].vel, -0.005f, 0.005f);

		particles[i].angle = MathExtras::WrapMinMax(particles[i].angle + particles[i].vel, 0.0f, 1.0f);
	}

	driveTorque = -netForce*100.0f;

	MasterProcess_Torque(sampleTime);

}

void Mode_Orbits::OnButtonQuickAction(int button) {
	Mode::OnButtonQuickAction(button);
	if(button == 0)
	{
		inputOutputDescriptors[0].augments[0].defaultVoltVal = -inputOutputDescriptors[0].augments[0].defaultVoltVal;
	}
}

void Mode_Orbits::ButtonDown(int button) {
	Mode::ButtonDown(button);
	if(button == 1)
	{
		inputOutputDescriptors[1].augments[0].defaultVoltVal = 16.0f;
	}
}

void Mode_Orbits::ButtonUp(int button) {
	Mode::ButtonUp(button);
	if(button == 1)
	{
		inputOutputDescriptors[1].augments[0].defaultVoltVal = 0.0f;
	}
}

void Mode_Orbits::OnGestureRecordStart() {
	Mode::OnGestureRecordStart();

	particle_guestSave = particles[0];
}

void Mode_Orbits::OnGesturePlay() {
	Mode::OnGesturePlay();
	particles[0] = particle_guestSave;

}

void Mode_Orbits::OnGestureLoop() {
	Mode::OnGestureLoop();
	particles[0] = particle_guestSave;

}

void Mode_Orbits::ReadExtraGuestureData(float *data) {
	particles[0].angle = data[0];
	particles[0].force = data[1];
	particles[0].vel = data[2];
}

void Mode_Orbits::WriteExtraGuestureData(float *data) {
	data[0] = particles[0].angle;
	data[1] = particles[0].force;
	data[2] = particles[0].vel;
}
