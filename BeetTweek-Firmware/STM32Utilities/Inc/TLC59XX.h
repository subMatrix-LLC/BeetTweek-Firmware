#ifndef TLC59XX_H
#define TLC59XX_H


#include "main.h"

typedef struct TLC59XXChipLEDState
{
	uint16_t red;
	uint16_t green;
	uint16_t blue;

} TLC59XXChipLEDState;



typedef struct TLC59XXChip
{
	SPI_HandleTypeDef* hspi;
	GPIO_TypeDef* blankGPIOPort; uint16_t blankGPIOPin;
	GPIO_TypeDef* xlatGPIOPort; uint16_t xlatGPIOPin;
	int numRGBLeds;
	float brightnessFactor;
	
	TLC59XXChipLEDState* LEDStates;
	
	
} TLC59XXChip;





void TLC59XXChipInit(TLC59XXChip* chip, SPI_HandleTypeDef* hspi,
		GPIO_TypeDef* blankGPIOPort, uint16_t blankGPIOPin,
		GPIO_TypeDef* xlatGPIOPort, uint16_t xlatGPIOPin,
		int numRGBLeds, float brightnessFactor)
{
	
	chip->hspi = hspi;
	chip->blankGPIOPort = blankGPIOPort;
	chip->blankGPIOPin = blankGPIOPin;
	
	chip->xlatGPIOPort = xlatGPIOPort;
	chip->xlatGPIOPin = xlatGPIOPin;
	chip->numRGBLeds = numRGBLeds;
	chip->brightnessFactor = brightnessFactor;
	
	
	chip->LEDStates = new TLC59XXChipLEDState[numRGBLeds];
	
}

void TLC59XXChipSetLEDColor(TLC59XXChip* chip, int LEDIdx, float red, float green, float blue)
{
	chip->LEDStates[LEDIdx].red = red*4095.0*chip->brightnessFactor;
	chip->LEDStates[LEDIdx].green = green*4095.0*chip->brightnessFactor;
	chip->LEDStates[LEDIdx].blue = blue*4095.0*chip->brightnessFactor;
}

void TLC59XXChipUpdate(TLC59XXChip* chip)
{
	for(int i = 0; i < chip->numRGBLeds; i++)
	{
		HAL_SPI_Transmit(chip->hspi, (uint8_t*)(&   chip->LEDStates[i].blue  ), 1, 1000);  //b
		HAL_SPI_Transmit(chip->hspi, (uint8_t*)(&   chip->LEDStates[i].green ), 1, 1000);  //g
		HAL_SPI_Transmit(chip->hspi, (uint8_t*)(&   chip->LEDStates[i].red   ), 1, 1000);  //r
	}

	//pulse xlat to commit the current register state on the chip and trasfer to effective register
	HAL_GPIO_WritePin(chip->xlatGPIOPort, chip->xlatGPIOPin, GPIO_PIN_SET);
	DelayUS(1);
	HAL_GPIO_WritePin(chip->xlatGPIOPort, chip->xlatGPIOPin, GPIO_PIN_RESET);
	
	
	
	HAL_GPIO_WritePin(chip->blankGPIOPort, chip->blankGPIOPin, GPIO_PIN_RESET);
}



#endif
