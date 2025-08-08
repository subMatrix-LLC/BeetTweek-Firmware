#include "main.h"
#include "spi.h"
#include "tim.h"
#include "MotorControl.h"
#include "MotorAngleState.h"
#include "AS5048.h"

extern "C" {

static MotorDevice motorDevA;
static AS5048_Device motorEncoderDev;

void BoardVerificationInit()
{
    MotorDeviceInit(&motorDevA, DRV8313_EN1_GPIO_Port, DRV8313_EN1_Pin,
                    DRV8313_EN2_GPIO_Port, DRV8313_EN2_Pin,
                    DRV8313_EN3_GPIO_Port, DRV8313_EN3_Pin,
                    
					DRV8313_IN1_GPIO_Port, DRV8313_IN1_Pin,
					DRV8313_IN2_GPIO_Port, DRV8313_IN2_Pin,
					DRV8313_IN3_GPIO_Port, DRV8313_IN3_Pin,

                    &htim3,
                    TIM_CHANNEL_1,
                    TIM_CHANNEL_2,
                    TIM_CHANNEL_4,
                    
                    DRV8313_A_nRESET_GPIO_Port, DRV8313_A_nRESET_Pin,
                    0, 0,
                    100,
                    DRV8313_A_nFAULT_GPIO_Port, DRV8313_A_nFAULT_Pin,
                    1,
                    22, 0.8f);

    MotorAngleStateInit(&motorDevA.angleState);

    AS5048DeviceInit(&motorEncoderDev, &hspi5, AS5048_CS_GPIO_Port, AS5048_CS_Pin);

    ResetMotorChip(&motorDevA);

    SetMotorEnable(&motorDevA, 1, GPIO_PIN_SET);
    SetMotorEnable(&motorDevA, 2, GPIO_PIN_SET);
    SetMotorEnable(&motorDevA, 3, GPIO_PIN_SET);
}

void BoardVerificationLoopUpdate()
{
    HAL_Delay(1000);
    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
}

} 
