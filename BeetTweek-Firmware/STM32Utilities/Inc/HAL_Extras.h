/*
 * HAL_Extras.h
 *
 *  Created on: Feb 11, 2021
 *      Author: casht
 */

#ifndef INC_HAL_EXTRAS_HPP_
#define INC_HAL_EXTRAS_HPP_

#include "tim.h"

//returns the reset time of the given timer in seconds.  Use Post initialization.
float HAL_CalculateTimerResetTime(TIM_HandleTypeDef* tim_baseHandle)
{
	  uint32_t prescaler =  tim_baseHandle->Init.Prescaler;
	  if(prescaler == 0)
		  prescaler = 1;


	  float APBTimerClockFreq = HAL_RCC_GetPCLK1Freq()*2.0f;


	  //TODO +1?
	  return ( ((float)tim_baseHandle->Init.Period) * prescaler)/(APBTimerClockFreq);
}

//returns the time of CNT the given timer in seconds.  Use Post initialization.
float HAL_CalculateTimerTime(TIM_HandleTypeDef* tim_baseHandle)
{
	  uint32_t prescaler =  tim_baseHandle->Init.Prescaler;
	  if(prescaler == 0)
		  prescaler = 1;


	  float APBTimerClockFreq = HAL_RCC_GetPCLK1Freq()*2.0f;


	  //TODO +1?
	  return ( ((float)tim_baseHandle->Instance->CNT) * prescaler)/(APBTimerClockFreq);
}



#endif /* INC_HAL_EXTRAS_HPP_ */
