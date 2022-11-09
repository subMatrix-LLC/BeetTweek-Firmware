#include "MCP434X.h"

#include "spi.h"

void MCP434XChipInit(MCP434XChip* chip, SPI_HandleTypeDef* hspi,
		GPIO_TypeDef* CSGPIOPort, uint16_t CSGPIOPin,
		GPIO_TypeDef* RESETGPIOPort, uint16_t RESETGPIOPin,
		GPIO_TypeDef* WPGPIOPort, uint16_t WPGPIOPin,
		uint8_t boardDesignatorId)
{
	chip->hspi = hspi;
	chip->CSGPIOPort = CSGPIOPort;
	chip->CSGPIOPin = CSGPIOPin;

	chip->RESETGPIOPort = RESETGPIOPort;
	chip->RESETGPIOPin = RESETGPIOPin;

	chip->WPGPIOPort = WPGPIOPort;
	chip->WPGPIOPin = WPGPIOPin;

	//default ratios to half scale
	for(int i = 0; i < 4; i++)
	{
		chip->ratios[i] = 0.5f*MCP434X_FULLSCALE;
	}

	chip->boardDesignatorId = boardDesignatorId;

}


void MCP434XReset(MCP434XChip* chip)
{
	HAL_GPIO_WritePin(chip->RESETGPIOPort, chip->RESETGPIOPin, GPIO_PIN_SET);
	DelayUS(1);
	HAL_GPIO_WritePin(chip->RESETGPIOPort, chip->RESETGPIOPin, GPIO_PIN_RESET);
}

void MCP434XSetPORBORState(MCP434XChip* chip)
{
	HAL_GPIO_WritePin(chip->RESETGPIOPort, chip->RESETGPIOPin, GPIO_PIN_RESET);
	HAL_Delay(1);
}

void MCP434XClearPORBORState(MCP434XChip* chip)
{
	HAL_GPIO_WritePin(chip->RESETGPIOPort, chip->RESETGPIOPin, GPIO_PIN_SET);
	HAL_Delay(1);
}


void MCP434XSendCommand(MCP434XChip* chip, MCP434XChipCommandCode commandCode, uint8_t address, uint16_t data, uint16_t* rxData)
{

	//set CS low.
	HAL_GPIO_WritePin(chip->CSGPIOPort, chip->CSGPIOPin, GPIO_PIN_RESET);

	  uint16_t dataFrame = (address << 12) | (commandCode << 10) | (data);

	  DelayUS(1);


	  //transmit data in 1 16 bit chunk.
	  if(rxData != nullptr)
		  HAL_SPI_TransmitReceive(chip->hspi, (uint8_t*)&dataFrame, (uint8_t*)rxData, 1, 1000);
	  else
		  HAL_SPI_Transmit(chip->hspi, (uint8_t*)&dataFrame, 1, 1000);

	  DelayUS(1);

	HAL_GPIO_WritePin(chip->CSGPIOPort, chip->CSGPIOPin, GPIO_PIN_SET);
}



// Sets (Writes) a value to the given wiper index (volatile),
// ratio = 1 indicates the wiper is directly connected to the A port.
// ratio = 0 indicates wiper is connected to B port.
void MCP434XWriteWiperVolatile(MCP434XChip* chip, uint8_t wiperIndex, uint16_t ratio)
{




	uint8_t resolvedAddress;
	switch(wiperIndex)
	{
	case 0:
		resolvedAddress = 0;//volatile wiper 0
		break;
	case 1:
		resolvedAddress = 1;//volatile wiper 1
		break;
	case 2:
		resolvedAddress = 6;//volatile wiper 2
		break;
	case 3:
		resolvedAddress = 7;//volatile wiper 3
		break;
	default:
		return;
	}

	MCP434XSendCommand(chip, MCP434XChipCommandCode_Write, resolvedAddress, ratio);
}


// Sets (Writes) a value to the given wiper index (non volatile),
// ratio = 1 indicates the wiper is directly connected to the A port.
// ratio = 0 indicates wiper is connected to B port.
void MCP434XWriteWiperNonVolatile(MCP434XChip* chip, uint8_t wiperIndex, uint16_t ratio)
{
	uint8_t resolvedAddress;
	switch(wiperIndex)
	{
	case 0:
		resolvedAddress = 0x2;//non volatile wiper 0
		break;
	case 1:
		resolvedAddress = 0x3;//non volatile wiper 1
		break;
	case 2:
		resolvedAddress = 0x8;//non volatile wiper 2
		break;
	case 3:
		resolvedAddress = 0x9;//non volatile wiper 3
		break;
	default:
		return;
	}

	MCP434XSendCommand(chip, MCP434XChipCommandCode_Write, resolvedAddress, ratio);
	//Max eeprom write cycle is 10 ms.
	HAL_Delay(10);
}

uint16_t MCP434XReadWiperNonVolatile(MCP434XChip* chip, uint8_t wiperIndex)
{
	uint8_t resolvedAddress;
	switch(wiperIndex)
	{
	case 0:
		resolvedAddress = 0x2;//non volatile wiper 0
		break;
	case 1:
		resolvedAddress = 0x3;//non volatile wiper 1
		break;
	case 2:
		resolvedAddress = 0x8;//non volatile wiper 2
		break;
	case 3:
		resolvedAddress = 0x9;//non volatile wiper 3
		break;
	default:
		return 0;
	}
	uint16_t readData;

	MCP434XSendCommand(chip, MCP434XChipCommandCode_Read, resolvedAddress, 0, &readData);
	return readData & 0x1FF;
}


uint16_t MCP434XReadWiperVolatile(MCP434XChip* chip, uint8_t wiperIndex)
{
	uint8_t resolvedAddress;
	switch(wiperIndex)
	{
	case 0:
		resolvedAddress = 0;//volatile wiper 0
		break;
	case 1:
		resolvedAddress = 1;//volatile wiper 1
		break;
	case 2:
		resolvedAddress = 6;//volatile wiper 2
		break;
	case 3:
		resolvedAddress = 7;//volatile wiper 3
		break;
	default:
		return 0;
	}
	uint16_t readData;

	MCP434XSendCommand(chip, MCP434XChipCommandCode_Read, resolvedAddress, 0, &readData);
	return readData & 0x1FF;
}

