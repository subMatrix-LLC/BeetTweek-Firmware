/*
 * MotorInterupts.cpp
 *
 *  Created on: Aug 7, 2020
 *      Author: casht
 */
#include "TimerInterupts.h"
#include "main.h"
#include "tim.h"
#include "MotorController.h"

extern MotorController motorController;

extern "C"
{

void TIM2_Interupt()
{
	motorController.UpdateMotorControl(TIM2_RESET_TIME);
}


}
