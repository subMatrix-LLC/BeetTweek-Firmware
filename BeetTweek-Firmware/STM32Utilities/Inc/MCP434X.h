/*
 * MCP434X.h
 *
 *  Created on: Oct 7, 2020
 *      Author: casht
 */

#ifndef INC_MCP434X_H_
#define INC_MCP434X_H_

#include "main.h"
#include "us_delay.h"


#define MCP434X_FULLSCALE 0x100

typedef struct MCP434XChip
{
	SPI_HandleTypeDef* hspi;
	GPIO_TypeDef* CSGPIOPort;
	uint16_t CSGPIOPin;

	GPIO_TypeDef* RESETGPIOPort;
	uint16_t RESETGPIOPin;

	GPIO_TypeDef* WPGPIOPort;
	uint16_t WPGPIOPin;

	//state
	uint16_t ratios[4];

	uint8_t boardDesignatorId;//EX: for U16 put 16

} MCP434XChip;




enum MCP434XChipCommandCode
{
	MCP434XChipCommandCode_Write = 0,
	MCP434XChipCommandCode_Increment = 1,
	MCP434XChipCommandCode_Decrement = 2,
	MCP434XChipCommandCode_Read = 3
};


void MCP434XChipInit(MCP434XChip* chip, SPI_HandleTypeDef* hspi,
		GPIO_TypeDef* CSGPIOPort, uint16_t CSGPIOPin,
		GPIO_TypeDef* RESETGPIOPort, uint16_t RESETGPIOPin,
		GPIO_TypeDef* WPGPIOPort, uint16_t WPGPIOPin,
		uint8_t boardDesignatorId);


void MCP434XReset(MCP434XChip* chip);

void MCP434XSetPORBORState(MCP434XChip* chip);

void MCP434XClearPORBORState(MCP434XChip* chip);


void MCP434XSendCommand(MCP434XChip* chip, MCP434XChipCommandCode commandCode, uint8_t address, uint16_t data, uint16_t* rxData = nullptr);


// Sets (Writes) a value to the given wiper index (volatile),
// ratio = 1 indicates the wiper is directly connected to the A port.
// ratio = 0 indicates wiper is connected to B port.
void MCP434XWriteWiperVolatile(MCP434XChip* chip, uint8_t wiperIndex, uint16_t ratio);

// Sets (Writes) a value to the given wiper index (non volatile),
// ratio = 1 indicates the wiper is directly connected to the A port.
// ratio = 0 indicates wiper is connected to B port.
void MCP434XWriteWiperNonVolatile(MCP434XChip* chip, uint8_t wiperIndex, uint16_t ratio);



uint16_t MCP434XReadWiperNonVolatile(MCP434XChip* chip, uint8_t wiperIndex);
uint16_t MCP434XReadWiperVolatile(MCP434XChip* chip, uint8_t wiperIndex);





#endif
