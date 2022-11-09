/*
 * us_delay.c
 *
 *  Created on: Dec 20, 2019
 *      Author: casht
 */

#include "math.h"
#include "us_delay.h"
#include "stdio.h"



float usPerTick = 0;
void InitUSDelay()
{

	//start the counter.

	//enable counter debug registers
#if defined(STM32H7) || defined (STM32F4)
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
#if defined(STM32H7)
	DWT->LAR = 0xC5ACCE55;// <-- added unlock access to DWT (ITM, etc.)registers
#endif
	DWT->CYCCNT = 0;
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;


#endif

	DWT->CTRL |= 1;



	DWT->CYCCNT = 0;


	uint32_t ticks = 10000;
	uint32_t ticker = ticks;

	uint32_t clkStart = DWT->CYCCNT;

	/* Wait till done */
	while (ticker--);

	uint32_t clkEnd = DWT->CYCCNT;


	uint32_t clks = clkEnd - clkStart;

	//check the clock has not revolved
	if(clkEnd < clkStart)
	{
		clks = (0xFFFFFFFF - clkStart) + clkEnd;
	}



	float Time_us = (clks / (float)SystemCoreClock) * 1000000;

	usPerTick = Time_us/float(ticks);


	//final tune value
	usPerTick *= US_DELAY_TUNE;

}


