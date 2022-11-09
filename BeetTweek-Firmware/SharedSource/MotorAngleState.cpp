/*
 * MotorAngleState.c
 *
 *  Created on: Jun 2, 2020
 *      Author: casht
 */


#include "MotorAngleState.h"
#include "MathExtras.h"
#include "arm_math.h"



void MotorAngleStateInit(MotorAngleState* state)
{
	state->currentAngle = 0.0f;
	state->revolutionAccumulator = 0;
	state->revolutionAccumulatorLast = 0;

	state->currentAngularVel = 0.0f;
	state->currentAngularVelFiltered2 = 0.0f;
	state->currentAngularVelFiltered = 0.0f;

}

void MotorAngleStateDestroy(MotorAngleState* state)
{
}


void MotorAngleStateReset(MotorAngleState* state)
{
	state->currentAngle = 0.0f;
	state->revolutionAccumulator = 0;
	state->revolutionAccumulatorLast = 0;

	state->currentAngleFiltered = 0.0f;
	state->currentAngularAcc = 0.0f;
	state->currentAngularAccFiltered = 0.0f;
	state->currentAngularVel = 0.0f;
	state->currentAngularVelFiltered = 0.0f;
}

void MotorAngleStateCopyTo(MotorAngleState* state, MotorAngleState* targetState)
{
	targetState = state;
}





void MotorAngleStateResetAccumulation(MotorAngleState* state)
{
	state->revolutionAccumulator = 0;
	state->revolutionAccumulatorLast = 0;
}

void MotorAngleStateUpdateLocalf(MotorAngleState* state, float newAngle, float sampleTimeS)
{
	newAngle = MathExtras::WrapMax(newAngle, 1.0f);
	float delta = newAngle - state->currentAngle;
	state->lastAngle = state->currentAngle;
	state->currentAngle = newAngle;

	state->revolutionAccumulatorLast = state->revolutionAccumulator;
	//If abs(difference) is greater than 180 (0.5) assume the encoder has wrapped around.
	if(delta > 0.5)
	{
		state->revolutionAccumulator--;


		//update derivative accounting for wrap
		state->lastAngularVel = state->currentAngularVel;

		state->currentAngularVel = (state->currentAngle - 1.0f - state->lastAngle)/sampleTimeS;
	}
	else if(delta < -0.5)
	{
		state->revolutionAccumulator++;


		//update derivative accounting for wrap
		state->lastAngularVel = state->currentAngularVel;

		state->currentAngularVel = (state->currentAngle + 1.0f - state->lastAngle)/sampleTimeS;

	}
	else
	{
		//update derivative (un-filtered)
		state->lastAngularVel = state->currentAngularVel;

		state->currentAngularVel = (state->currentAngle - state->lastAngle)/sampleTimeS;
	}

	MotorAngleStateUpdateFiltering(state,sampleTimeS);

}
void MotorAngleStateUpdateAccumulatedd(MotorAngleState* state, double newAngle, float sampleTimeS)
{
	double currentAccumulated = MotorAngleStateGetCurrentAccumulatedAngled(state);
	double lastAccumulated = state->lastAngle + (double)state->revolutionAccumulatorLast;
	double delta = newAngle - currentAccumulated;

	float newLocalAngle = MathExtras::Fract(newAngle);
	state->revolutionAccumulatorLast = state->revolutionAccumulator;
	state->revolutionAccumulator = MathExtras::FloorToInt(newAngle);


	state->lastAngle = state->currentAngle;
	state->currentAngle = newLocalAngle;


	state->lastAngularVel = state->currentAngularVel;
	state->currentAngularVel = (currentAccumulated - lastAccumulated)/sampleTimeS;

	MotorAngleStateUpdateFiltering(state,sampleTimeS);

}

void MotorAngleStateUpdateFiltering(MotorAngleState* state, float sampleTimeS)
{
	//update vel filtering
	state->lastAngularVelFiltered = state->currentAngularVelFiltered;

	state->currentAngularVelFiltered += 100.0f*sampleTimeS*(state->currentAngularVel - state->currentAngularVelFiltered);
	state->currentAngularVelFiltered2 += 10.0f*sampleTimeS*(state->currentAngularVel - state->currentAngularVelFiltered2);
}
float MotorAngleStateGetCurrentAnglef(MotorAngleState* state)
{
	return state->currentAngle;
}

float MotorAngleStateGetCurrentAccumulatedAnglef(MotorAngleState* state)
{
	return state->currentAngle + (float)state->revolutionAccumulator;
}
double MotorAngleStateGetCurrentAccumulatedAngled(MotorAngleState* state)
{
	return state->currentAngle + (double)state->revolutionAccumulator;
}


float MotorAngleStateGetAngleDeltaf(MotorAngleState* state)
{
	//#TODO fix for best float precision.
	float curAngle = state->currentAngle + (float)state->revolutionAccumulator;
	float lastAngle = state->lastAngle + (float)state->revolutionAccumulatorLast;

	return curAngle - lastAngle;
}


