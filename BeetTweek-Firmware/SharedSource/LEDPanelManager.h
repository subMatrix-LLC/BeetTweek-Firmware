/*
 * LEDPanelManager.h
 *
 *  Created on: Oct 27, 2020
 *      Author: casht
 */

#ifndef INC_LEDPANELMANAGER_H_
#define INC_LEDPANELMANAGER_H_

#include <MathExtras_Color.h>
#include "LEDHandling.h"
#include "MathExtras.h"
#include "stdio.h"

#define LED_NUM_LEDS_PANEL 8
#define LED_NUM_LEDS_PER_RING 40
#define LED_BASE_BRIGHTNESS_255 16
#define LED_PANEL_BRIGHTNESS_255 (LED_BASE_BRIGHTNESS_255)


class LEDPanelManager {
public:
	LEDPanelManager();
	virtual ~LEDPanelManager();


	enum RINGIDENTIFIER {
		RINGIDENTIFIER_INNER = 0,
		RINGIDENTIFIER_OUTER
	};

	int ringInnerClockToLEDIdx[LED_NUM_LEDS_PER_RING];
	int ringOuterClockToLEDIdx[LED_NUM_LEDS_PER_RING];

	int panelReversed_ = 0;


	//set an led on the inner ring.  index 0 is top, going clockwise to index n-1 left of top
	void SetLEDInnerRing_Int(int clockIndex, uint8_t r, uint8_t g, uint8_t b)
	{
		WS2812_SETLED(ClockIndexToLEDIndex(RINGIDENTIFIER_INNER, clockIndex), r, g, b);
	}

	//set an led on the outer ring.  index 0 is top, going clockwise to index n-1 left of top
	void SetLEDOuterRing_Int(int clockIndex, uint8_t r, uint8_t g, uint8_t b)
	{
		WS2812_SETLED(ClockIndexToLEDIndex(RINGIDENTIFIER_OUTER, clockIndex), r, g, b);
	}



	void SetLEDInnerRing_Float(float clockAngle, uint8_t r, uint8_t g, uint8_t b)
	{
		WS2812_SETLED(ClockIndexToLEDIndex(RINGIDENTIFIER_INNER, (clockAngle)*LED_NUM_LEDS_PER_RING+0.5f), r, g, b);
	}

	void SetLEDOuterRing_Float(float clockAngle, uint8_t r, uint8_t g, uint8_t b)
	{
		WS2812_SETLED(ClockIndexToLEDIndex(RINGIDENTIFIER_OUTER, (clockAngle)*LED_NUM_LEDS_PER_RING+0.5f), r, g, b);
	}


	void GetLEDInnerRing_Int(int clockIndex, uint8_t* r_out, uint8_t* g_out, uint8_t *b_out)
	{
		WS2812_GETLED(ClockIndexToLEDIndex(RINGIDENTIFIER_INNER, clockIndex),r_out,g_out,b_out);
	}
	void GetLEDOuterRing_Int(int clockIndex, uint8_t* r_out, uint8_t* g_out, uint8_t *b_out)
	{
		WS2812_GETLED(ClockIndexToLEDIndex(RINGIDENTIFIER_OUTER, clockIndex),r_out,g_out,b_out);
	}


	void GetLEDInnerRing_Float(float clockAngle, uint8_t* r_out, uint8_t* g_out, uint8_t *b_out)
	{
		GetLEDInnerRing_Int(clockAngle*LED_NUM_LEDS_PER_RING, r_out, g_out, b_out);
		return;
		WS2812_GETLED(ClockIndexToLEDIndex(RINGIDENTIFIER_INNER,  (clockAngle)*LED_NUM_LEDS_PER_RING+0.5f),r_out,g_out,b_out);
	}
	void GetLEDOuterRing_Float(float clockAngle, uint8_t* r_out, uint8_t* g_out, uint8_t *b_out)
	{
		GetLEDOuterRing_Int(clockAngle*LED_NUM_LEDS_PER_RING, r_out, g_out, b_out);
		return;
		WS2812_GETLED(ClockIndexToLEDIndex(RINGIDENTIFIER_OUTER,  (clockAngle)*LED_NUM_LEDS_PER_RING+0.5f),r_out,g_out,b_out);
	}





	//clockwise starting at top conversion to led index.
	// indexes wrap around if "out of bounds"
	int ClockIndexToLEDIndex(RINGIDENTIFIER ring, int centerIndex)
	{
		centerIndex = MathExtras::WrapMax<int>(centerIndex, LED_NUM_LEDS_PER_RING);

		int midoffset = 3 + panelReversed_*(LED_NUM_LEDS_PER_RING/2);

		if(ring == RINGIDENTIFIER_OUTER){
			int lowIdx = LED_NUM_LEDS_PANEL;

			//int highIdx = lowIdx + LED_NUM_LEDS_PER_RING;

			int finalIdx = lowIdx + midoffset + centerIndex;

			if(finalIdx >= lowIdx + LED_NUM_LEDS_PER_RING)
			{
				int diff = finalIdx - (lowIdx + LED_NUM_LEDS_PER_RING);

				finalIdx = lowIdx + diff;
			}
			return finalIdx;
		}
		else if(ring == RINGIDENTIFIER_INNER){
			int lowIdx = LED_NUM_LEDS_PANEL + LED_NUM_LEDS_PER_RING;

			int highIdx = lowIdx + LED_NUM_LEDS_PER_RING;

			int finalIdx = highIdx - midoffset - centerIndex - 1;

			if(finalIdx < lowIdx)
			{
				int diff = finalIdx - ( lowIdx );

				finalIdx = highIdx + diff;
			}
			return finalIdx;
		}
		else
		{
			return 0;
		}

	}



	//fills color between clockIndexStart and clockIndexEnd.
	//watch out if setting very large clock-index ranges.  as the algorithm is worst case O(LED_NUM_LEDS_PER_RING*LED_NUM_LEDS_PER_RING)
	//assumes leds are currently set to 0,0,0.
	//circleStartColor is color when clock indexes are close to each other
	//circleEndColor is color when clock indexes are far from each other and overlapping.
	//circleInterpPercentage is percentage of circle the color ramp should take up. ie 1 would be the ramp ramps every cycle, 0.25 would be ramp up 1/4 cycle
	void SetLEDRingRangeLinear_Int(RINGIDENTIFIER ring,
			int clockIndexStart,
			int clockIndexEnd,
			MathExtras::Color* circleEndColor,
			MathExtras::Color* circleStartColor, float circleInterpPercentage)
	{

		//int clockIndexStartLocal = MathExtras::WrapMinMax(clockIndexStart, 0, LED_NUM_LEDS_PER_RING);
		//int clockIndexEndLocal   = MathExtras::WrapMinMax(clockIndexEnd, 0, LED_NUM_LEDS_PER_RING);


		//adjust for backwards
		int dir = 1;
		if(clockIndexStart > clockIndexEnd)
		{
			dir = -1;
		}

		//iterate over all leds in ring:
		for(int clockIdxIterator = 0; clockIdxIterator < LED_NUM_LEDS_PER_RING; clockIdxIterator++)
		{
			int ledOff = 1;
			int ledIdx = ClockIndexToLEDIndex(ring, clockIdxIterator);

			//interpolate between circle colors
			MathExtras::Color interpolatedColor;



			//compute "projected index"
			int projIdx = clockIndexEnd;

			//TODO There should be better math to step local projIdx down to clockIdxIterator
			while(MathExtras::WrapMinMax(projIdx, 0, LED_NUM_LEDS_PER_RING) != clockIdxIterator)
			{
				projIdx -= dir;
			}



			if(dir == 1)
				if(projIdx >= clockIndexStart)
					ledOff = 0;
			if(dir == -1)
				if(projIdx <= clockIndexStart)
					ledOff = 0;

			if(ledOff == 0)
			{
				int dist = projIdx - clockIndexStart;

				float t = MathExtras::ClampInclusive(float(dist), -LED_NUM_LEDS_PER_RING*circleInterpPercentage,
						LED_NUM_LEDS_PER_RING*circleInterpPercentage)/float(LED_NUM_LEDS_PER_RING*circleInterpPercentage);

				if(t < 0.0f)
					t = -t;


				interpolatedColor = *circleEndColor;
				interpolatedColor = interpolatedColor.Lerp(*circleStartColor, t);



				WS2812_SETLED(ledIdx,
						interpolatedColor.r_*LED_BASE_BRIGHTNESS_255,
						interpolatedColor.g_*LED_BASE_BRIGHTNESS_255,
						interpolatedColor.b_*LED_BASE_BRIGHTNESS_255);
			}



		}
	}


	//overload for floating point clock index [0.0 , 1.0]
	void SetLEDRingRangeLinear_Float(RINGIDENTIFIER ring,
			float clockStart,
			float clockEnd,
			MathExtras::Color* circleEndColor,
			MathExtras::Color* circleStartColor, float circleInterpPercentage)
	{

		SetLEDRingRangeLinear_Int(ring, int(((clockStart)*LED_NUM_LEDS_PER_RING)+0.5f), int(((clockEnd)*LED_NUM_LEDS_PER_RING)+0.5f), circleEndColor, circleStartColor, circleInterpPercentage);
	}

	//overload for floating point clock index [0.0 , 1.0]
	//Assumes start and end are no more than 0.5f apart.
	void SetLEDRingRangeLinear_HalfWrap_Float(RINGIDENTIFIER ring,
			float clockStart,
			float clockEnd,
			MathExtras::Color* circleEndColor,
			MathExtras::Color* circleStartColor, float circleInterpPercentage)
	{

		float diff = MathExtras::WrappedLocalDifference(clockStart, clockEnd, 1.0f);
		clockEnd = clockStart + diff;
		SetLEDRingRangeLinear_Int(ring, int(((clockStart)*LED_NUM_LEDS_PER_RING)+0.5f), int(((clockEnd)*LED_NUM_LEDS_PER_RING)+0.5f), circleEndColor, circleStartColor, circleInterpPercentage);
	}




	//set an led on the interface button panel
	void SetLEDButton(uint8_t button, uint8_t r, uint8_t g, uint8_t b)
	{


		WS2812_SETLED(button,
				r,
				g,
				b);
	}

	void GetLEDButton(uint8_t button, uint8_t* r, uint8_t* g, uint8_t* b)
	{
		WS2812_GETLED(button, r, g, b);
	}

	void SetKnobCenterLEDS(uint8_t r, uint8_t g, uint8_t b)
	{
		for(int i = 40+40+8; i < WS2812_NUMLEDS; i++)
		{
			WS2812_SETLED(i, r, g, b);
		}
	}

};

#endif /* INC_LEDPANELMANAGER_H_ */
