################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
E:/Repos/BeetTweek-Firmware-Public/BeetTweek-Firmware/SharedSource/DualBoardComHandler.cpp \
E:/Repos/BeetTweek-Firmware-Public/BeetTweek-Firmware/SharedSource/LEDHandling.cpp \
E:/Repos/BeetTweek-Firmware-Public/BeetTweek-Firmware/SharedSource/LEDPanelManager.cpp \
E:/Repos/BeetTweek-Firmware-Public/BeetTweek-Firmware/SharedSource/MotorAngleState.cpp 

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
SharedSource/DualBoardComHandler.o: E:/Repos/BeetTweek-Firmware-Public/BeetTweek-Firmware/SharedSource/DualBoardComHandler.cpp SharedSource/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m7 -std=gnu++14 -g3 -DDEBUG -D__FPU_PRESENT -DARM_MATH_CM7 -DUSE_HAL_DRIVER -DSTM32H743xx -c -I../Core/Inc -I../Drivers/CMSIS/DSP/Include -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/MathExtras_DSP" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/SharedSource" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/STM32Utilities/Inc" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
SharedSource/LEDHandling.o: E:/Repos/BeetTweek-Firmware-Public/BeetTweek-Firmware/SharedSource/LEDHandling.cpp SharedSource/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m7 -std=gnu++14 -g3 -DDEBUG -D__FPU_PRESENT -DARM_MATH_CM7 -DUSE_HAL_DRIVER -DSTM32H743xx -c -I../Core/Inc -I../Drivers/CMSIS/DSP/Include -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/MathExtras_DSP" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/SharedSource" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/STM32Utilities/Inc" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
SharedSource/LEDPanelManager.o: E:/Repos/BeetTweek-Firmware-Public/BeetTweek-Firmware/SharedSource/LEDPanelManager.cpp SharedSource/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m7 -std=gnu++14 -g3 -DDEBUG -D__FPU_PRESENT -DARM_MATH_CM7 -DUSE_HAL_DRIVER -DSTM32H743xx -c -I../Core/Inc -I../Drivers/CMSIS/DSP/Include -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/MathExtras_DSP" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/SharedSource" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/STM32Utilities/Inc" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
SharedSource/MotorAngleState.o: E:/Repos/BeetTweek-Firmware-Public/BeetTweek-Firmware/SharedSource/MotorAngleState.cpp SharedSource/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m7 -std=gnu++14 -g3 -DDEBUG -D__FPU_PRESENT -DARM_MATH_CM7 -DUSE_HAL_DRIVER -DSTM32H743xx -c -I../Core/Inc -I../Drivers/CMSIS/DSP/Include -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/MathExtras_DSP" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/SharedSource" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/STM32Utilities/Inc" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-SharedSource

clean-SharedSource:
	-$(RM) ./SharedSource/DualBoardComHandler.cyclo ./SharedSource/DualBoardComHandler.d ./SharedSource/DualBoardComHandler.o ./SharedSource/DualBoardComHandler.su ./SharedSource/LEDHandling.cyclo ./SharedSource/LEDHandling.d ./SharedSource/LEDHandling.o ./SharedSource/LEDHandling.su ./SharedSource/LEDPanelManager.cyclo ./SharedSource/LEDPanelManager.d ./SharedSource/LEDPanelManager.o ./SharedSource/LEDPanelManager.su ./SharedSource/MotorAngleState.cyclo ./SharedSource/MotorAngleState.d ./SharedSource/MotorAngleState.o ./SharedSource/MotorAngleState.su

.PHONY: clean-SharedSource

