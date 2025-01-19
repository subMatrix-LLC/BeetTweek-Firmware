################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/STM32Utilities/Src/ee24.c 

CPP_SRCS += \
C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/STM32Utilities/Src/AS5048.cpp \
C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/STM32Utilities/Src/DACX0504.cpp \
C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/STM32Utilities/Src/MCP434X.cpp \
C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/STM32Utilities/Src/MathExtras_Color.cpp \
C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/STM32Utilities/Src/PCM3168.cpp \
C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/STM32Utilities/Src/Vector2.cpp \
C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/STM32Utilities/Src/Vector3.cpp \
C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/STM32Utilities/Src/Vector4.cpp \
C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/STM32Utilities/Src/us_delay.cpp 

C_DEPS += \
./STM32Utilities/Src/ee24.d 

OBJS += \
./STM32Utilities/Src/AS5048.o \
./STM32Utilities/Src/DACX0504.o \
./STM32Utilities/Src/MCP434X.o \
./STM32Utilities/Src/MathExtras_Color.o \
./STM32Utilities/Src/PCM3168.o \
./STM32Utilities/Src/Vector2.o \
./STM32Utilities/Src/Vector3.o \
./STM32Utilities/Src/Vector4.o \
./STM32Utilities/Src/ee24.o \
./STM32Utilities/Src/us_delay.o 

CPP_DEPS += \
./STM32Utilities/Src/AS5048.d \
./STM32Utilities/Src/DACX0504.d \
./STM32Utilities/Src/MCP434X.d \
./STM32Utilities/Src/MathExtras_Color.d \
./STM32Utilities/Src/PCM3168.d \
./STM32Utilities/Src/Vector2.d \
./STM32Utilities/Src/Vector3.d \
./STM32Utilities/Src/Vector4.d \
./STM32Utilities/Src/us_delay.d 


# Each subdirectory must supply rules for building sources it contributes
STM32Utilities/Src/AS5048.o: C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/STM32Utilities/Src/AS5048.cpp STM32Utilities/Src/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DSTM32F405xx -DUSE_HAL_DRIVER -DSTM32F4xx -DARM_MATH_CM4 -DKNOB_STM -DDEBUG -c -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/DSP/Include -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/API" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/SharedSource" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/STM32Utilities/Inc" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/MathExtras_DSP" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
STM32Utilities/Src/DACX0504.o: C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/STM32Utilities/Src/DACX0504.cpp STM32Utilities/Src/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DSTM32F405xx -DUSE_HAL_DRIVER -DSTM32F4xx -DARM_MATH_CM4 -DKNOB_STM -DDEBUG -c -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/DSP/Include -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/API" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/SharedSource" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/STM32Utilities/Inc" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/MathExtras_DSP" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
STM32Utilities/Src/MCP434X.o: C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/STM32Utilities/Src/MCP434X.cpp STM32Utilities/Src/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DSTM32F405xx -DUSE_HAL_DRIVER -DSTM32F4xx -DARM_MATH_CM4 -DKNOB_STM -DDEBUG -c -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/DSP/Include -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/API" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/SharedSource" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/STM32Utilities/Inc" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/MathExtras_DSP" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
STM32Utilities/Src/MathExtras_Color.o: C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/STM32Utilities/Src/MathExtras_Color.cpp STM32Utilities/Src/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DSTM32F405xx -DUSE_HAL_DRIVER -DSTM32F4xx -DARM_MATH_CM4 -DKNOB_STM -DDEBUG -c -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/DSP/Include -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/API" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/SharedSource" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/STM32Utilities/Inc" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/MathExtras_DSP" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
STM32Utilities/Src/PCM3168.o: C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/STM32Utilities/Src/PCM3168.cpp STM32Utilities/Src/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DSTM32F405xx -DUSE_HAL_DRIVER -DSTM32F4xx -DARM_MATH_CM4 -DKNOB_STM -DDEBUG -c -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/DSP/Include -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/API" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/SharedSource" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/STM32Utilities/Inc" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/MathExtras_DSP" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
STM32Utilities/Src/Vector2.o: C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/STM32Utilities/Src/Vector2.cpp STM32Utilities/Src/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DSTM32F405xx -DUSE_HAL_DRIVER -DSTM32F4xx -DARM_MATH_CM4 -DKNOB_STM -DDEBUG -c -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/DSP/Include -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/API" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/SharedSource" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/STM32Utilities/Inc" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/MathExtras_DSP" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
STM32Utilities/Src/Vector3.o: C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/STM32Utilities/Src/Vector3.cpp STM32Utilities/Src/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DSTM32F405xx -DUSE_HAL_DRIVER -DSTM32F4xx -DARM_MATH_CM4 -DKNOB_STM -DDEBUG -c -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/DSP/Include -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/API" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/SharedSource" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/STM32Utilities/Inc" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/MathExtras_DSP" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
STM32Utilities/Src/Vector4.o: C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/STM32Utilities/Src/Vector4.cpp STM32Utilities/Src/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DSTM32F405xx -DUSE_HAL_DRIVER -DSTM32F4xx -DARM_MATH_CM4 -DKNOB_STM -DDEBUG -c -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/DSP/Include -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/API" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/SharedSource" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/STM32Utilities/Inc" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/MathExtras_DSP" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
STM32Utilities/Src/ee24.o: C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/STM32Utilities/Src/ee24.c STM32Utilities/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32F405xx -DUSE_HAL_DRIVER -DSTM32F4xx -DARM_MATH_CM4 -DKNOB_STM -DDEBUG -c -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/DSP/Include -IC:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/API -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/STM32Utilities/Inc" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/SharedSource" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
STM32Utilities/Src/us_delay.o: C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/STM32Utilities/Src/us_delay.cpp STM32Utilities/Src/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DSTM32F405xx -DUSE_HAL_DRIVER -DSTM32F4xx -DARM_MATH_CM4 -DKNOB_STM -DDEBUG -c -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/DSP/Include -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/API" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/SharedSource" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/STM32Utilities/Inc" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/MathExtras_DSP" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-STM32Utilities-2f-Src

clean-STM32Utilities-2f-Src:
	-$(RM) ./STM32Utilities/Src/AS5048.cyclo ./STM32Utilities/Src/AS5048.d ./STM32Utilities/Src/AS5048.o ./STM32Utilities/Src/AS5048.su ./STM32Utilities/Src/DACX0504.cyclo ./STM32Utilities/Src/DACX0504.d ./STM32Utilities/Src/DACX0504.o ./STM32Utilities/Src/DACX0504.su ./STM32Utilities/Src/MCP434X.cyclo ./STM32Utilities/Src/MCP434X.d ./STM32Utilities/Src/MCP434X.o ./STM32Utilities/Src/MCP434X.su ./STM32Utilities/Src/MathExtras_Color.cyclo ./STM32Utilities/Src/MathExtras_Color.d ./STM32Utilities/Src/MathExtras_Color.o ./STM32Utilities/Src/MathExtras_Color.su ./STM32Utilities/Src/PCM3168.cyclo ./STM32Utilities/Src/PCM3168.d ./STM32Utilities/Src/PCM3168.o ./STM32Utilities/Src/PCM3168.su ./STM32Utilities/Src/Vector2.cyclo ./STM32Utilities/Src/Vector2.d ./STM32Utilities/Src/Vector2.o ./STM32Utilities/Src/Vector2.su ./STM32Utilities/Src/Vector3.cyclo ./STM32Utilities/Src/Vector3.d ./STM32Utilities/Src/Vector3.o ./STM32Utilities/Src/Vector3.su ./STM32Utilities/Src/Vector4.cyclo ./STM32Utilities/Src/Vector4.d ./STM32Utilities/Src/Vector4.o ./STM32Utilities/Src/Vector4.su ./STM32Utilities/Src/ee24.cyclo ./STM32Utilities/Src/ee24.d ./STM32Utilities/Src/ee24.o ./STM32Utilities/Src/ee24.su ./STM32Utilities/Src/us_delay.cyclo ./STM32Utilities/Src/us_delay.d ./STM32Utilities/Src/us_delay.o ./STM32Utilities/Src/us_delay.su

.PHONY: clean-STM32Utilities-2f-Src

