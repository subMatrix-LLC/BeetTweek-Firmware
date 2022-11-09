/*
 * SPI3Handling.h
 *
 *  Created on: Aug 26, 2020
 *      Author: casht
 */

#ifndef INC_SPIHANDLING_H_
#define INC_SPIHANDLING_H_

#include "spi.h"
#include "MotorControl.h"
#include "MotorController.h"
#include "DualBoardComHandler.h"

extern MotorController motorController;
extern DualBoardComHandler ComHandler;

extern "C" {

void SPI_CUSTOM_HANDLER(SPI_HandleTypeDef *hspi);
}
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi);

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi);

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi);



#endif /* INC_SPIHANDLING_H_ */
