/*
 * LEDHandling.h
 *
 *  Created on: Oct 21, 2020
 */

#ifndef INC_LEDHANDLING_H_
#define INC_LEDHANDLING_H_



// Peripheral usage
#include "main.h"
#define HTIM4_PERIOD (350)


#define WS2812_NUMLEDS (40+40+8+8)
#define WS2812_NUMCOLORCHANNELS (3)
#define WS2812_DMA_BYTES_PER_CHAN (8)
#define WS2812_DMA_RESET_BYTES (300)
#define WS2812_TOTAL_BYTES (WS2812_NUMLEDS*WS2812_NUMCOLORCHANNELS)

#define WS2812_DMA_BUFF_SIZE (WS2812_TOTAL_BYTES*WS2812_DMA_BYTES_PER_CHAN+WS2812_DMA_RESET_BYTES)





#define WS2812_LOWVAL (90)
#define WS2812_HIGHVAL (240)





extern DMA_HandleTypeDef hdma_tim4_ch1;
extern DMA_HandleTypeDef hdma_memtomem_dma2_stream1;
//extern volatile uint8_t WS2812_LEDSTATE[WS2812_TOTAL_BYTES];
extern volatile uint16_t WS2812_DMA_BUFF1[WS2812_DMA_BUFF_SIZE];
extern volatile uint16_t WS2812_DMA_BUFF2[WS2812_DMA_BUFF_SIZE];

void WS2812_INIT();
void WS2812_DEINIT();
//void WS2812_SETLED_BLIT(uint8_t index, uint8_t r, uint8_t g, uint8_t b);
void WS2812_SETLED(uint8_t index, uint8_t r, uint8_t g, uint8_t b);
void WS2812_SETALLLED(uint8_t r, uint8_t g, uint8_t b);
void WS2812_GETLED(uint8_t index, uint8_t* r_out, uint8_t* g_out, uint8_t* b_out);

void WS2812_SWAP_BUFFERS();

#endif /* INC_LEDHANDLING_H_ */
