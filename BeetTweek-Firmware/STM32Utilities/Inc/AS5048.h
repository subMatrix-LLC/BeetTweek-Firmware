#ifndef AS5048_H
#define AS5048_H
#include "main.h"
typedef struct AS5048_Device
{
	SPI_HandleTypeDef* hspi; //spi device
	GPIO_TypeDef* chipSelGPIOPort;
	uint16_t chipSelGPIOPin;

} AS5048_Device;



void AS5048DeviceInit(AS5048_Device* device,
		SPI_HandleTypeDef* hspi,
		GPIO_TypeDef* chipSelGPIOPort,
		uint16_t chipSelGPIOPin);


/**
 * @brief  Reads the Encoder Angle From AS5048 Using SPI Blocking Functions.
 * The first reading will take twice as long in order to get the first value.
 * @retval float indicating angle from 0 to 1 (0 to 360 degrees)
 */
float AS5048_SPI_ReadAnglef(AS5048_Device* device);




#endif




