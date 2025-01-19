################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/SharedSource/DualBoardComHandler.cpp \
C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/SharedSource/LEDHandling.cpp \
C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/SharedSource/LEDPanelManager.cpp \
C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/SharedSource/MotorAngleState.cpp 

OBJS += \
./SharedSource/DualBoardComHandler.o \
./SharedSource/LEDHandling.o \
./SharedSource/LEDPanelManager.o \
./SharedSource/MotorAngleState.o 

CPP_DEPS += \
./SharedSource/DualBoardComHandler.d \
./SharedSource/LEDHandling.d \
./SharedSource/LEDPanelManager.d \
./SharedSource/MotorAngleState.d 


# Each subdirectory must supply rules for building sources it contributes
SharedSource/DualBoardComHandler.o: C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/SharedSource/DualBoardComHandler.cpp SharedSource/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DSTM32F405xx -DUSE_HAL_DRIVER -DSTM32F4xx -DARM_MATH_CM4 -DKNOB_STM -DDEBUG -c -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/DSP/Include -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/API" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/SharedSource" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/STM32Utilities/Inc" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/MathExtras_DSP" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
SharedSource/LEDHandling.o: C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/SharedSource/LEDHandling.cpp SharedSource/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DSTM32F405xx -DUSE_HAL_DRIVER -DSTM32F4xx -DARM_MATH_CM4 -DKNOB_STM -DDEBUG -c -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/DSP/Include -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/API" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/SharedSource" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/STM32Utilities/Inc" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/MathExtras_DSP" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
SharedSource/LEDPanelManager.o: C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/SharedSource/LEDPanelManager.cpp SharedSource/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DSTM32F405xx -DUSE_HAL_DRIVER -DSTM32F4xx -DARM_MATH_CM4 -DKNOB_STM -DDEBUG -c -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/DSP/Include -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/API" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/SharedSource" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/STM32Utilities/Inc" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/MathExtras_DSP" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
SharedSource/MotorAngleState.o: C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/SharedSource/MotorAngleState.cpp SharedSource/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DSTM32F405xx -DUSE_HAL_DRIVER -DSTM32F4xx -DARM_MATH_CM4 -DKNOB_STM -DDEBUG -c -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/DSP/Include -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/API" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/SharedSource" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/STM32Utilities/Inc" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/MathExtras_DSP" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-SharedSource

clean-SharedSource:
	-$(RM) ./SharedSource/DualBoardComHandler.cyclo ./SharedSource/DualBoardComHandler.d ./SharedSource/DualBoardComHandler.o ./SharedSource/DualBoardComHandler.su ./SharedSource/LEDHandling.cyclo ./SharedSource/LEDHandling.d ./SharedSource/LEDHandling.o ./SharedSource/LEDHandling.su ./SharedSource/LEDPanelManager.cyclo ./SharedSource/LEDPanelManager.d ./SharedSource/LEDPanelManager.o ./SharedSource/LEDPanelManager.su ./SharedSource/MotorAngleState.cyclo ./SharedSource/MotorAngleState.d ./SharedSource/MotorAngleState.o ./SharedSource/MotorAngleState.su

.PHONY: clean-SharedSource

