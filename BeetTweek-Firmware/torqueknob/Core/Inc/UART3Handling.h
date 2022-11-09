/*
 * UART3Handling.h
 *
 *  Created on: Jul 29, 2020
 *      Author: casht
 */

#ifndef INC_UART3HANDLING_H_
#define INC_UART3HANDLING_H_

#include "main.h"
#include "CircularQueueBuffer.h"

HAL_StatusTypeDef HandleSerialAPI(UART_HandleTypeDef* huart);
HAL_StatusTypeDef WaitReadUART3RXByte(uint8_t* data, int size, int timeout);




#endif /* INC_UART3HANDLING_H_ */
