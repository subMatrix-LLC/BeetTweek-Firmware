/*
 * LEDHandling.cpp
 *
 *  Created on: Oct 21, 2020
 *      Author: casht
 */

#include "LEDHandling.h"
#include "tim.h"
#include "us_delay.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

volatile uint8_t WS2812_LEDSTATE[WS2812_TOTAL_BYTES];
volatile uint16_t WS2812_DMA_BUFF1[WS2812_DMA_BUFF_SIZE];
volatile uint16_t WS2812_DMA_BUFF2[WS2812_DMA_BUFF_SIZE];
volatile int WS2812_DMA_CURRENT_DRAW_BUFF = 1;

void WS2812_INIT()
{
	for(uint16_t i = 0; i < WS2812_TOTAL_BYTES; i++)
		WS2812_LEDSTATE[i] = 0;


	for(uint16_t i = 0; i < WS2812_TOTAL_BYTES*WS2812_DMA_BYTES_PER_CHAN; i++)
		WS2812_DMA_BUFF1[i] = (uint16_t)WS2812_LOWVAL;

	for(uint16_t i = WS2812_TOTAL_BYTES*WS2812_DMA_BYTES_PER_CHAN; i < WS2812_DMA_BUFF_SIZE; i++)
		WS2812_DMA_BUFF1[i] = 0;



	for(uint16_t i = 0; i < WS2812_TOTAL_BYTES*WS2812_DMA_BYTES_PER_CHAN; i++)
		WS2812_DMA_BUFF2[i] = (uint16_t)WS2812_LOWVAL;

	for(uint16_t i = WS2812_TOTAL_BYTES*WS2812_DMA_BYTES_PER_CHAN; i < WS2812_DMA_BUFF_SIZE; i++)
		WS2812_DMA_BUFF2[i] = 0;


	/*HAL_StatusTypeDef status = */HAL_TIM_PWM_Start_DMA(&htim4, TIM_CHANNEL_1, (uint32_t*)WS2812_DMA_BUFF2,
			WS2812_DMA_BUFF_SIZE);



}
void WS2812_DEINIT()
{
	HAL_TIM_PWM_Stop_DMA(&htim4, TIM_CHANNEL_1);
}

//call after setting led states..
void WS2812_SWAP_BUFFERS()
{

	//Could Maybe use DMA for transfer.  But its usually busy doing other things..
	/*HAL_StatusTypeDef status = */HAL_DMA_Start(&hdma_memtomem_dma2_stream1, (uint32_t)WS2812_DMA_BUFF1, (uint32_t)WS2812_DMA_BUFF2, WS2812_DMA_BUFF_SIZE);

	HAL_DMA_PollForTransfer(&hdma_memtomem_dma2_stream1,HAL_DMA_FULL_TRANSFER, 10000);

}
//void WS2812_SETLED_BLIT(uint8_t index, uint8_t r, uint8_t g, uint8_t b)
//{
//	uint16_t stateLookup = index*WS2812_NUMCOLORCHANNELS;
//
//	//determine buffer to write to
//	volatile uint16_t* drawBuffer = nullptr;
//	if(WS2812_DMA_CURRENT_DRAW_BUFF == 1)
//	{
//		drawBuffer = WS2812_DMA_BUFF1;
//	}
//	else
//	{
//		drawBuffer = WS2812_DMA_BUFF2;
//	}
//
//	bool update[3] = {false};
//
//	if(WS2812_LEDSTATE[stateLookup] != g)
//	{
//		WS2812_LEDSTATE[stateLookup] = g;
//		update[0] = true;
//	}
//
//	if(WS2812_LEDSTATE[stateLookup + 1] != r)
//	{
//		WS2812_LEDSTATE[stateLookup + 1] = r;
//		update[1] = true;
//	}
//
//	if(WS2812_LEDSTATE[stateLookup + 2] != b)
//	{
//		WS2812_LEDSTATE[stateLookup + 2] = b;
//		update[2] = true;
//	}
//
//
//	if(!update[0] && !update[1] && !update[2])
//		return;
//
//
//
//	for(uint16_t i = 0; i < 8; i++)
//	{
//		uint8_t mask = (1 << i);
//
//		if(update[0]) drawBuffer[index*WS2812_NUMCOLORCHANNELS*WS2812_DMA_BYTES_PER_CHAN                               + 7-i] = ((g & mask) >> i)*WS2812_HIGHVAL + (!((g & mask) >> i))*WS2812_LOWVAL;
//		if(update[1]) drawBuffer[index*WS2812_NUMCOLORCHANNELS*WS2812_DMA_BYTES_PER_CHAN + WS2812_DMA_BYTES_PER_CHAN   + 7-i] = ((r & mask) >> i)*WS2812_HIGHVAL + (!((r & mask) >> i))*WS2812_LOWVAL;
//		if(update[2]) drawBuffer[index*WS2812_NUMCOLORCHANNELS*WS2812_DMA_BYTES_PER_CHAN + WS2812_DMA_BYTES_PER_CHAN*2 + 7-i] = ((b & mask) >> i)*WS2812_HIGHVAL + (!((b & mask) >> i))*WS2812_LOWVAL;
//	}
//
//}
void WS2812_SETLED(uint8_t index, uint8_t r, uint8_t g, uint8_t b)
{
	uint16_t stateLookup = index*WS2812_NUMCOLORCHANNELS;

	WS2812_LEDSTATE[stateLookup] = g;
	WS2812_LEDSTATE[stateLookup+1] = r;
	WS2812_LEDSTATE[stateLookup+2] = b;

	//determine buffer to write to
	volatile uint16_t* drawBuffer = nullptr;
	if(WS2812_DMA_CURRENT_DRAW_BUFF == 1)
	{
		drawBuffer = WS2812_DMA_BUFF1;
	}
	else
	{
		drawBuffer = WS2812_DMA_BUFF2;
	}



	for(uint16_t i = 0; i < 8; i++)
	{
		uint8_t mask = (1 << i);
		drawBuffer[index*WS2812_NUMCOLORCHANNELS*WS2812_DMA_BYTES_PER_CHAN                               + 7-i] = ((g & mask) >> i)*WS2812_HIGHVAL + (!((g & mask) >> i))*WS2812_LOWVAL;
		drawBuffer[index*WS2812_NUMCOLORCHANNELS*WS2812_DMA_BYTES_PER_CHAN + WS2812_DMA_BYTES_PER_CHAN   + 7-i] = ((r & mask) >> i)*WS2812_HIGHVAL + (!((r & mask) >> i))*WS2812_LOWVAL;
		drawBuffer[index*WS2812_NUMCOLORCHANNELS*WS2812_DMA_BYTES_PER_CHAN + WS2812_DMA_BYTES_PER_CHAN*2 + 7-i] = ((b & mask) >> i)*WS2812_HIGHVAL + (!((b & mask) >> i))*WS2812_LOWVAL;
	}

}





void WS2812_GETLED(uint8_t index, uint8_t* r_out, uint8_t* g_out, uint8_t* b_out)
{
	uint16_t stateLookup = index*WS2812_NUMCOLORCHANNELS;
	*g_out = WS2812_LEDSTATE[stateLookup];
	*r_out = WS2812_LEDSTATE[stateLookup+1];
	*b_out = WS2812_LEDSTATE[stateLookup+2];
}


void WS2812_SETALLLED(uint8_t r, uint8_t g, uint8_t b)
{
	for(uint8_t i = 0; i < WS2812_NUMLEDS; i++)
	{
		WS2812_SETLED(i, r,g,b);
	}
}
