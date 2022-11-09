/*
 * CapSense.cpp
 *
 *  Created on: Aug 24, 2020
 *      Author: casht
 */

#include "CapSense.h"

#include "main.h"
#include "MathExtras.h"
#include "us_delay.h"







void InitializeCapSense(CapSenseDevice* capSense)
{


	//initialize Buffers
	capSense->CAP_SENSE_FILTER_COEF = new float[capSense->CAP_SENSE_FILTER_SIZE];
	capSense->CAP_SENSE_FILTER_BUFFER = new float[capSense->CAP_SENSE_FILTER_SIZE];


	//init coefficients
	for(int i = 0; i < capSense->CAP_SENSE_FILTER_SIZE; i++)
	{
		capSense->CAP_SENSE_FILTER_COEF[i] = 1.0f;
	}


	//setup the filter
	arm_fir_init_f32(&capSense->CAP_SENSE_FILTER, capSense->CAP_SENSE_FILTER_NUMTAPS, capSense->CAP_SENSE_FILTER_COEF, capSense->CAP_SENSE_FILTER_BUFFER, capSense->CAP_SENSE_FILTER_BLOCK_SIZE);

	capSense->INITIALIZED = 1;
}

int DetermineCapSense(CapSenseDevice* capSense)
{
	return capSense->CAP_SENSE_TOUCHED;
}


