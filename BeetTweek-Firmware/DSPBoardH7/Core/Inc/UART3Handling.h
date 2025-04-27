/*
 * UART3Handling.h
 *
 *  Created on: Jul 29, 2020
 *      Author: casht
 */

#ifndef INC_UART3HANDLING_H_
#define INC_UART3HANDLING_H_

#include "main.h"

class SerialDevice {
public:
	UART_HandleTypeDef* huart = nullptr;
	bool usb = false;
};

HAL_StatusTypeDef SerialDeviceTransmit(SerialDevice* device, const uint8_t* pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef WaitReadUART3RXByte(SerialDevice* device, uint8_t* data, int size, int timeout);
HAL_StatusTypeDef HandleSerialAPI(SerialDevice* device);

#endif /* INC_UART3HANDLING_H_ */
