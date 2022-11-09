/*
 * profiling.h
 *
 *  Created on: Apr 5, 2021
 *      Author: casht
 */

#ifndef INC_PROFILING_H_
#define INC_PROFILING_H_
#include "main.h"


extern volatile uint32_t PROCESSING_OVERTIME_MISSES;
extern volatile uint32_t FastUpdate_TimerHeadRoom;
extern volatile uint32_t FastUpdate_KnobProcessTime;
extern volatile uint32_t FastUpdate_ADCFilterAndConversion;
extern volatile uint32_t FastUpdate_AudioDSP;
extern volatile uint32_t FastUpdate_DAC;

#endif /* INC_PROFILING_H_ */
