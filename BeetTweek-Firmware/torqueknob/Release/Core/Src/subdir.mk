################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/adc.c \
../Core/Src/gpio.c \
../Core/Src/i2c.c \
../Core/Src/main.c \
../Core/Src/spi.c \
../Core/Src/stm32f4xx_hal_msp.c \
../Core/Src/stm32f4xx_it.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f4xx.c \
../Core/Src/tim.c \
../Core/Src/usart.c 

CPP_SRCS += \
../Core/Src/CapSense.cpp \
../Core/Src/CapSenseInterrupts.cpp \
../Core/Src/MotorControl.cpp \
../Core/Src/MotorController.cpp \
../Core/Src/PIDControl.cpp \
../Core/Src/ProbeTools.cpp \
../Core/Src/Program.cpp \
../Core/Src/RollingFloatBuffer.cpp \
../Core/Src/SPIHandling.cpp \
../Core/Src/TimerInterupts.cpp \
../Core/Src/UART3Handling.cpp 

C_DEPS += \
./Core/Src/adc.d \
./Core/Src/gpio.d \
./Core/Src/i2c.d \
./Core/Src/main.d \
./Core/Src/spi.d \
./Core/Src/stm32f4xx_hal_msp.d \
./Core/Src/stm32f4xx_it.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f4xx.d \
./Core/Src/tim.d \
./Core/Src/usart.d 

OBJS += \
./Core/Src/CapSense.o \
./Core/Src/CapSenseInterrupts.o \
./Core/Src/MotorControl.o \
./Core/Src/MotorController.o \
./Core/Src/PIDControl.o \
./Core/Src/ProbeTools.o \
./Core/Src/Program.o \
./Core/Src/RollingFloatBuffer.o \
./Core/Src/SPIHandling.o \
./Core/Src/TimerInterupts.o \
./Core/Src/UART3Handling.o \
./Core/Src/adc.o \
./Core/Src/gpio.o \
./Core/Src/i2c.o \
./Core/Src/main.o \
./Core/Src/spi.o \
./Core/Src/stm32f4xx_hal_msp.o \
./Core/Src/stm32f4xx_it.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f4xx.o \
./Core/Src/tim.o \
./Core/Src/usart.o 

CPP_DEPS += \
./Core/Src/CapSense.d \
./Core/Src/CapSenseInterrupts.d \
./Core/Src/MotorControl.d \
./Core/Src/MotorController.d \
./Core/Src/PIDControl.d \
./Core/Src/ProbeTools.d \
./Core/Src/Program.d \
./Core/Src/RollingFloatBuffer.d \
./Core/Src/SPIHandling.d \
./Core/Src/TimerInterupts.d \
./Core/Src/UART3Handling.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.cpp Core/Src/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -DSTM32F405xx -DUSE_HAL_DRIVER -DSTM32F4xx -DARM_MATH_CM4 -DKNOB_STM -c -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/DSP/Include -I../Drivers/CMSIS/DSP/Include -I../Drivers/CMSIS/NN/Include -I"C:/Users/casht/repos/torqueknob/stcubeideworkspace/SharedSource" -Os -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DSTM32F405xx -DUSE_HAL_DRIVER -DSTM32F4xx -DARM_MATH_CM4 -DKNOB_STM -c -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/DSP/Include -I"C:/Users/casht/repos/torqueknob/stcubeideworkspace/SharedSource" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/CapSense.cyclo ./Core/Src/CapSense.d ./Core/Src/CapSense.o ./Core/Src/CapSense.su ./Core/Src/CapSenseInterrupts.cyclo ./Core/Src/CapSenseInterrupts.d ./Core/Src/CapSenseInterrupts.o ./Core/Src/CapSenseInterrupts.su ./Core/Src/MotorControl.cyclo ./Core/Src/MotorControl.d ./Core/Src/MotorControl.o ./Core/Src/MotorControl.su ./Core/Src/MotorController.cyclo ./Core/Src/MotorController.d ./Core/Src/MotorController.o ./Core/Src/MotorController.su ./Core/Src/PIDControl.cyclo ./Core/Src/PIDControl.d ./Core/Src/PIDControl.o ./Core/Src/PIDControl.su ./Core/Src/ProbeTools.cyclo ./Core/Src/ProbeTools.d ./Core/Src/ProbeTools.o ./Core/Src/ProbeTools.su ./Core/Src/Program.cyclo ./Core/Src/Program.d ./Core/Src/Program.o ./Core/Src/Program.su ./Core/Src/RollingFloatBuffer.cyclo ./Core/Src/RollingFloatBuffer.d ./Core/Src/RollingFloatBuffer.o ./Core/Src/RollingFloatBuffer.su ./Core/Src/SPIHandling.cyclo ./Core/Src/SPIHandling.d ./Core/Src/SPIHandling.o ./Core/Src/SPIHandling.su ./Core/Src/TimerInterupts.cyclo ./Core/Src/TimerInterupts.d ./Core/Src/TimerInterupts.o ./Core/Src/TimerInterupts.su ./Core/Src/UART3Handling.cyclo ./Core/Src/UART3Handling.d ./Core/Src/UART3Handling.o ./Core/Src/UART3Handling.su ./Core/Src/adc.cyclo ./Core/Src/adc.d ./Core/Src/adc.o ./Core/Src/adc.su ./Core/Src/gpio.cyclo ./Core/Src/gpio.d ./Core/Src/gpio.o ./Core/Src/gpio.su ./Core/Src/i2c.cyclo ./Core/Src/i2c.d ./Core/Src/i2c.o ./Core/Src/i2c.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/spi.cyclo ./Core/Src/spi.d ./Core/Src/spi.o ./Core/Src/spi.su ./Core/Src/stm32f4xx_hal_msp.cyclo ./Core/Src/stm32f4xx_hal_msp.d ./Core/Src/stm32f4xx_hal_msp.o ./Core/Src/stm32f4xx_hal_msp.su ./Core/Src/stm32f4xx_it.cyclo ./Core/Src/stm32f4xx_it.d ./Core/Src/stm32f4xx_it.o ./Core/Src/stm32f4xx_it.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f4xx.cyclo ./Core/Src/system_stm32f4xx.d ./Core/Src/system_stm32f4xx.o ./Core/Src/system_stm32f4xx.su ./Core/Src/tim.cyclo ./Core/Src/tim.d ./Core/Src/tim.o ./Core/Src/tim.su ./Core/Src/usart.cyclo ./Core/Src/usart.d ./Core/Src/usart.o ./Core/Src/usart.su

.PHONY: clean-Core-2f-Src

