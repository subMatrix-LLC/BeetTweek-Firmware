/*
 * PIDControl.c
 *
 *  Created on: Jan 3, 2020
 *      Author: casht
 */
#include "PIDControl.h"
#include "math.h"


void PIDStateInit(PIDState* state)
{
	state->I = 0.0f;
	state->PGain = 0.01f;
	state->IGain = 0.000001f;

	state->IClampHigh = 0;
	state->IClampLow = 0;

	state->outputClampHigh = 1;
	state->outputClampLow = 1;
	state->outputClampHighLevel = 1.0f;
	state->outputClampLowLevel = -1.0f;

	state->errorPadding = 0.0f;

}


float PIControl(float target, float measured, PIDState* state)
{

	float error = target - measured;

	//make error seem less using errorPadding.
	if(error > 0.0f)
	{
		error += -state->errorPadding;
	}
	else
	{
		error += state->errorPadding;
	}



	float P = state->PGain * error;
	float I = state->I;

	//Update PIDState
	state->I += error * state->IGain;

	//clamp I term if enabled
	if(state->IClampHigh)
	{
		if(state->I > state->IClampHighLevel)
			state->I = state->IClampHighLevel;
	}
	if(state->IClampLow)
	{
		if(state->I < state->IClampLowLevel)
			state->I = state->IClampLowLevel;
	}


	float output = (P + I);

	//clamp output if enabled.
	if(state->outputClampHigh)
	{
		if(output > state->outputClampHighLevel)
			output = state->outputClampHighLevel;

	}
	if(state->outputClampLow)
	{
		if(output < state->outputClampLowLevel)
			output = state->outputClampLowLevel;

	}

	return output;
}



