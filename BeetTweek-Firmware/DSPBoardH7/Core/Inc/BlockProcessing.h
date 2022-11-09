/*
 * BlockProcessing.h
 *
 *  Created on: May 11, 2021
 *      Author: casht
 */

#ifndef INC_BLOCKPROCESSING_H_
#define INC_BLOCKPROCESSING_H_

#include "adcdacbuffers.h"


#define SAMPLERATE (48000.0)
#define SAMPLETIME (1.0/SAMPLERATE)
#define BLOCKPROCESSINGTIME_S ((DACDMABUFFERSIZE/2) * (1.0f/SAMPLERATE))


//Iterate through all DMA Samples
#define BLOCK_PROCESS(stride, offset) \
	int bufferSwapShift = bufferSwap*DACDMABUFFERSIZE/2; \
	int blockSize = DACDMABUFFERSIZE/2; \
	int nextBlockSampleIdx = DACDMABUFFERSIZE/2; \
	if(bufferSwapShift) nextBlockSampleIdx = 0; \
	float blockPerc = 0.0f; \
	int adc0Idx, adc1Idx; \
	for(int sampleIdxDMA = offset + DACDMABUFFERSIZE/2 - bufferSwapShift; sampleIdxDMA < DACDMABUFFERSIZE - bufferSwapShift; sampleIdxDMA+=stride){ \
		blockPerc += 1.0f/(DACDMABUFFERSIZE/2); \
		adc0Idx = sampleIdxDMA - (sampleIdxDMA%2); \
		adc1Idx = sampleIdxDMA - (sampleIdxDMA%2); \




#define BLOCK_PROCESS_ALL() BLOCK_PROCESS(1, 0)


#define BLOCK_PROCESS_CHANNELS_0_2() BLOCK_PROCESS(2,0)
#define BLOCK_PROCESS_CHANNELS_1_3() BLOCK_PROCESS(2,1)


#define ADC_TO_FLOAT(adcDMA) (float(adcDMA << 8)/(INT32_MAX))


extern "C" {
void ProcessHalfBlock(int bufferSwap);
}


#endif /* INC_BLOCKPROCESSING_H_ */
