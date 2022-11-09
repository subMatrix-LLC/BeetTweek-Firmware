/*
 * SPI2Handling.cpp
 *
 *  Created on: Nov 23, 2020
 *      Author: casht
 */

#include <SPIHandling.h>



#include "Modes/Mode.h"
#include "spi.h"

#include "tim.h"
#include "stdio.h"


extern ModeManager modeManager;

extern "C" {

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{


}
void HAL_SPI_TxRXCpltCallback(SPI_HandleTypeDef *hspi)
{


}
void HAL_SPI_RXCpltCallback(SPI_HandleTypeDef *hspi)
{

}

void CUSTOMSPI3_Handler(SPI_HandleTypeDef *hspi)
{


}




}


