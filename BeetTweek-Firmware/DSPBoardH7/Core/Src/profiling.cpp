/*
 * profiling.cpp
 *
 *  Created on: Apr 5, 2021
 *      Author: casht
 */

#include "profiling.h"


volatile uint32_t PROCESSING_OVERTIME_MISSES = 0;
volatile uint32_t FastUpdate_TimerHeadRoom = 0;
volatile uint32_t FastUpdate_KnobProcessTime = 0;
volatile uint32_t FastUpdate_ADCFilterAndConversion = 0;
volatile uint32_t FastUpdate_AudioDSP = 0;
volatile uint32_t FastUpdate_DAC = 0;
