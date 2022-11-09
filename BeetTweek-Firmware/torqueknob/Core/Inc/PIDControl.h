/*
 * PIDControl.h
 *
 *  Created on: Jan 3, 2020
 *      Author: casht
 */


#ifndef PID_CONTROL_H
#define PID_CONTROL_H


typedef struct PIDState {

	int outputClampHigh;
	int outputClampLow;
	float outputClampHighLevel;
	float outputClampLowLevel;

	float errorPadding;

	float I;
	float IGain;

	int IClampHigh;
	int IClampLow;

	float IClampHighLevel;
	float IClampLowLevel;



	float PGain;

} PIDState;

//Initialize a PIDState struct
void PIDStateInit(PIDState* state);

float PIControl(float target, float measured, PIDState* state);



#endif
