/*
 * CapSense..h
 *
 *  Created on: Aug 24, 2020
 *      Author: casht
 */

#ifndef INC_CAPSENSE_H_
#define INC_CAPSENSE_H_



#include "arm_math.h"

//defines pins for a single motor device (DRV831*, etc)
typedef struct CapSenseDevice
{
	int CAP_SENSE_FILTER_SIZE = 32;
	int CAP_SENSE_FILTER_BLOCK_SIZE = 1;
	int CAP_SENSE_FILTER_NUMTAPS = CAP_SENSE_FILTER_SIZE;
	int CAP_SENSE_FILTER_THRES = 2700;


    float32_t CAP_SENSE_RISE_CNT_FILTERED = CAP_SENSE_FILTER_THRES;
    int CAP_SENSE_TOUCHED;
    int CAP_SENSE_LATEST_RISE_CNT;

    float* CAP_SENSE_FILTER_COEF = nullptr;
    float* CAP_SENSE_FILTER_BUFFER =  nullptr;

    arm_fir_instance_f32 CAP_SENSE_FILTER;

    int INITIALIZED = 0;
} CapSenseDevice;



void InitializeCapSense(CapSenseDevice* capSense);

int DetermineCapSense(CapSenseDevice* capSense);



#endif /* INC_CAPSENSE_H_ */
