#ifndef US_DELAY_H
#define US_DELAY_H

#include "main.h"


#ifdef STM32F7
#include "stm32f7xx_hal.h"
#define US_DELAY_TUNE 0.92f
#endif

#ifdef STM32H7
#include "stm32h7xx_hal.h"
#define  US_DELAY_TUNE .63f
#endif


#ifdef STM32F4
#include "stm32f4xx_hal.h"
#define  US_DELAY_TUNE 1.0f
#endif

extern float usPerTick;




void InitUSDelay();



/**
 * @brief  Delays for amount of micro seconds
 * @param  micros: Number of microseconds for delay
 * @retval None
 */
__STATIC_INLINE void DelayUS(volatile uint32_t micros) {

    /* Go to clock cycles */
	//

    micros /= usPerTick;

    /* Wait till done */
    while (micros--);

}





#endif
