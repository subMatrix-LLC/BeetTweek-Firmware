/*
 * ProbeTools.h
 *
 *  Created on: Jan 5, 2020
 *      Author: casht
 */

#ifndef PROBETOOLS_H_
#define PROBETOOLS_H_



typedef struct FloatViewWindow
{
	float maxVal;
	float minVal;

	int constrictSamples; //number of calls to FlowView() until window is re-evaluated
	int constrictHighCounter;
	int constrictLowCounter;

	float deadBandPerc;//how close to the window edges the signal must be for the window to be maintained.

} FloatViewWindow;


void FloatViewWindowInit(FloatViewWindow* window);

// Main Method of FloatView.  return the value passed in - managed by the window.
// return value will be 0 to 1 always.
float FloatViewWindowUpdate(float val, FloatViewWindow* window);


#endif /* PROBETOOLS_H_ */
