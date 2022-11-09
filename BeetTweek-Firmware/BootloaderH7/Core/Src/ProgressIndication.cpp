/*
 * ProgressIndication.cpp
 *
 *  Created on: Jan 11, 2022
 *      Author: casht
 */
#include "LEDPanelManager.h"
#include "MathExtras.h"



LEDPanelManager LEDManager;


extern "C" {

	void InitProgress()
	{
		WS2812_INIT();



	}
	void DeInitProgess()
	{

		WS2812_DEINIT();
	}


	void UpdateProgressPercent(int percent, float r, float g, float b)
	{
		WS2812_SETALLLED(0, 0, 0);
		MathExtras::Color color;
		color.r_ = r;
		color.g_ = g;
		color.b_ = b;
		LEDManager.SetLEDRingRangeLinear_Float(LEDPanelManager::RINGIDENTIFIER_OUTER, 0, 0.01f*percent, (MathExtras::Color*)&color, (MathExtras::Color*)&color, 1.0f);

		WS2812_SWAP_BUFFERS();
	}

}
