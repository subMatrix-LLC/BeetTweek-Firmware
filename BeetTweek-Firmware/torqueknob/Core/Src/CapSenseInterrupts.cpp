/*
 * CapSenseInterrupts.cpp
 *
 *  Created on: Sep 28, 2020
 *      Author: casht
 */

#include "CapSense.h"


extern CapSenseDevice knobCapSense;

extern "C" {


void HandleCapSenseTrigInterupt()
{
	if(knobCapSense.INITIALIZED == 0)
		return;

	//rising edge
	knobCapSense.CAP_SENSE_LATEST_RISE_CNT = TIM8->CNT;

	float filterIN = float(knobCapSense.CAP_SENSE_LATEST_RISE_CNT);

	//add to filter
	arm_fir_f32(&knobCapSense.CAP_SENSE_FILTER, &filterIN, &knobCapSense.CAP_SENSE_RISE_CNT_FILTERED, knobCapSense.CAP_SENSE_FILTER_BLOCK_SIZE);


	if(knobCapSense.CAP_SENSE_RISE_CNT_FILTERED < knobCapSense.CAP_SENSE_FILTER_THRES)
	{
		knobCapSense.CAP_SENSE_TOUCHED = 0;
	}
	else
	{
		knobCapSense.CAP_SENSE_TOUCHED = 1;
	}

}

}
