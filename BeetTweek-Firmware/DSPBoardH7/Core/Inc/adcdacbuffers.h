/*
 * adcdacbuffers.h
 *
 *  Created on: Mar 26, 2021
 *      Author: casht
 */

#ifndef INC_ADCDACBUFFERS_H_
#define INC_ADCDACBUFFERS_H_

#include "main.h"



#define DACDMABUFFERSIZE (32)

#define ADCDMABUFFERSIZE (DACDMABUFFERSIZE)



extern float dacOuts[4];
extern float dacOuts_1[4];
extern float dacVolts[4];//storage for most recent compute of volts
extern float dacVolts_1[4];
extern bool dacInterpJumps[4];//flags for indicating interpolation should jump.
extern int channelInterpEnables[4];


extern volatile int32_t adc0DMA[ADCDMABUFFERSIZE];
extern volatile int32_t adc1DMA[ADCDMABUFFERSIZE];
//extern volatile uint16_t adcInsRaw[4*ADCBLOCKSIZE];
extern volatile float adcIns[4];
extern float adcVolts[4];//storage for most recent compute of volts

extern volatile int32_t dac0DMA[DACDMABUFFERSIZE];
extern volatile int32_t dac1DMA[DACDMABUFFERSIZE];



#define ADC_PLUG_COUNT_THRESH (16)
extern volatile uint16_t adcPlugCounts[4];
extern volatile int adcPlugStates_1[4];
extern volatile int adcPlugStates[4];
extern volatile int adcFakePlugStates[4];
extern int PlugStateDACSequence[128];



inline int ADCPluggedIn(int input)
{
	return adcPlugStates[input] | adcFakePlugStates[input];
}




#endif /* INC_ADCDACBUFFERS_H_ */
