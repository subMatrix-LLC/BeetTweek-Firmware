/*
 * adcdacbuffers.cpp
 *
 *  Created on: Mar 26, 2021
 *      Author: casht
 */



#include "adcdacbuffers.h"

float dacOuts[4] = {0};
float dacOuts_1[4] = {0};
float dacVolts[4] = {0.0f};//storage for most recent compute of volts
float dacVolts_1[4] = {0.0f};
bool dacInterpJumps[4] = {false};
int channelInterpEnables[4] = {1,1,1,1};

//volatile float GlobalAttenuation = 1.0f;

volatile int32_t adc0DMA[ADCDMABUFFERSIZE] = {0};
volatile int32_t adc1DMA[ADCDMABUFFERSIZE] = {0};
//volatile uint16_t adcInsRaw[4*ADCBLOCKSIZE] = {0};
volatile float adcIns[4] = {0};
float adcVolts[4] = {0.0f};//storage for most recent compute of volts

volatile int32_t dac0DMA[DACDMABUFFERSIZE] = {0};
volatile int32_t dac1DMA[DACDMABUFFERSIZE] = {0};

volatile int adcPlugStates_1[4] = {0};
volatile int adcPlugStates[4] = {0};
volatile uint8_t adcFakePlugStates[4] = {0};
volatile uint16_t adcPlugCounts[4] = {ADC_PLUG_COUNT_THRESH*2};




int PlugStateDACSequence[128] = {
		1,
		1,
		0,
		0,
		1,
		0,
		0,
		1,
		1,
		0,
		1,
		1,
		0,
		0,
		0,
		1,
		0,
		1,
		1,
		1,
		0,
		0,
		0,
		1,
		0,
		1,
		0,
		0,
		1,
		0,
		1,
		1,
		0,
		0,
		0,
		0,
		0,
		1,
		1,
		0,
		1,
		0,
		1,
		0,
		0,
		1,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		1,
		1,
		0,
		1,
		0,
		0,
		1,
		0,
		1,
		1,
		1,
		1,
		0,
		1,
		0,
		0,
		1,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		1,
		1,
		0,
		0,
		1,
		1,
		0,
		0,
		0,
		1,
		0,
		1,
		1,
		0,
		1,
		0,
		1,
		0,
		0,
		0,
		1,
		1,
		0,
		1,
		1,
		0,
		0,
		1,
		1,
		0,
		1,
		1,
		0,
		1,
		1,
		1,
		1,
		1,
		1,
		0,
		0,
		0,
		0,
		1,
		1,
		1,
		0,
		1,
		1,
		1
};
















