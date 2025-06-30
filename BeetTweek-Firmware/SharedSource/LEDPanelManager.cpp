/*
 * LEDPanelManager.cpp
 *
 *  Created on: Oct 27, 2020
 *      Author: casht
 */

#include "LEDPanelManager.h"

LEDPanelManager::LEDPanelManager() {


	//fill the lookup arrays.
	for(int i = 0; i < LED_NUM_LEDS_PER_RING; i++)
	{
		ringInnerClockToLEDIdx[i] = ClockIndexToLEDIndex(RINGIDENTIFIER_INNER, i);
		ringOuterClockToLEDIdx[i] = ClockIndexToLEDIndex(RINGIDENTIFIER_OUTER, i);
	}
}

LEDPanelManager::~LEDPanelManager() {
	// TODO Auto-generated destructor stub
}

