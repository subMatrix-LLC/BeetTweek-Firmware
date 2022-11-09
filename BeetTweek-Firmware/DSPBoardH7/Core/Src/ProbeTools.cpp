/*
 * ProbeTools.cpp
 *
 *  Created on: Aug 7, 2020
 *      Author: casht
 */

#include "ProbeTools.h"

void FloatViewWindowInit(FloatViewWindow* window)
{
	window->maxVal = 1.0;
	window->minVal = 0.0;

	window->deadBandPerc = 0.25f;

	window->constrictSamples = 10000;
	window->constrictHighCounter = window->constrictSamples;
	window->constrictLowCounter = window->constrictSamples;
}


// Main Method of FloatView.  return the value passed in - managed by the window.
// return value will be 0 to 1 always.
float FloatViewWindowUpdate(float val, FloatViewWindow* window)
{
	//if the counter reaches zero.  shrink the window towards the val.
	if(window->constrictHighCounter == 0)
	{
		window->maxVal = (window->maxVal + val)*0.5f;

		window->constrictHighCounter = window->constrictSamples;
	}
	if(window->constrictLowCounter == 0)
	{
		window->minVal = (window->minVal + val)*0.5f;
		window->constrictLowCounter = window->constrictSamples;
	}


	//if val is within range - reset the timer.
	if((val - window->minVal) > (window->maxVal - window->minVal)*(0.5f + window->deadBandPerc))
		window->constrictHighCounter = window->constrictSamples;
	else if((val - window->minVal) < (window->maxVal - window->minVal)*(0.5f - window->deadBandPerc))
		window->constrictLowCounter = window->constrictSamples;


	if(val > window->maxVal)
		window->maxVal = val;

	if(val < window->minVal)
		window->minVal = val;


	window->constrictHighCounter--;
	window->constrictLowCounter--;

	//return current val mapped within the window. (0 to 1)
	return (val - window->minVal)/(window->maxVal - window->minVal);
}
