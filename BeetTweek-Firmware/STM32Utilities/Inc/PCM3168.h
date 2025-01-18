/*
 * PCM3168.h
 *
 *  Created on: Jul 19, 2021
 *      Author: casht
 */

#ifndef INC_PCM3168_H_
#define INC_PCM3168_H_

#include "main.h"
#include "spi.h"

uint8_t PCM3168Read(uint8_t addr);

void PCM3168Write(uint8_t addr, uint8_t data);

void PCM3168_Toggle48DeEmph();

#endif /* INC_PCM3168_H_ */
