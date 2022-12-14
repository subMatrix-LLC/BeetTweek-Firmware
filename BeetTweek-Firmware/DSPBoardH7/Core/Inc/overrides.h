/*
 * overrides.h
 *
 *  Created on: Sep 25, 2020
 *      Author: casht
 */

#ifndef INC_OVERRIDES_H_
#define INC_OVERRIDES_H_


int _write(int32_t file, uint8_t *ptr, int32_t len)
{
/* Implement your write code here, this is used by puts and printf for example */
//int i=0;
//for(i=0 ; i<len ; i++)
//ITM_SendChar((*ptr++));

#if !defined(COMBINEDBOARD)
HAL_StatusTypeDef status = HAL_UART_Transmit(&huart3, ptr, len, 1000);
#else
#if defined(VIRTUALCOMUSB)
CDC_Transmit_FS(ptr, len);
#endif
#endif

return len;

}


#endif /* INC_OVERRIDES_H_ */
