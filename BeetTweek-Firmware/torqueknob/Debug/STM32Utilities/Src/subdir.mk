################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
E:/Repos/BeetTweek-Firmware-Public/BeetTweek-Firmware/STM32Utilities/Src/ee24.c 

CPP_SRCS += \
E:/Repos/BeetTweek-Firmware-Public/BeetTweek-Firmware/STM32Utilities/Src/AS5048.cpp \
E:/Repos/BeetTweek-Firmware-Public/BeetTweek-Firmware/STM32Utilities/Src/DACX0504.cpp \
E:/Repos/BeetTweek-Firmware-Public/BeetTweek-Firmware/STM32Utilities/Src/MCP434X.cpp \
E:/Repos/BeetTweek-Firmware-Public/BeetTweek-Firmware/STM32Utilities/Src/MathExtras_Color.cpp \
E:/Repos/BeetTweek-Firmware-Public/BeetTweek-Firmware/STM32Utilities/Src/PCM3168.cpp \
E:/Repos/BeetTweek-Firmware-Public/BeetTweek-Firmware/STM32Utilities/Src/Vector2.cpp \
E:/Repos/BeetTweek-Firmware-Public/BeetTweek-Firmware/STM32Utilities/Src/Vector3.cpp \
E:/Repos/BeetTweek-Firmware-Public/BeetTweek-Firmware/STM32Utilities/Src/Vector4.cpp \
E:/Repos/BeetTweek-Firmware-Public/BeetTweek-Firmware/STM32Utilities/Src/us_delay.cpp 

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
STM32Utilities/Src/AS5048.o: E:/Repos/BeetTweek-Firmware-Public/BeetTweek-Firmware/STM32Utilities/Src/AS5048.cpp STM32Utilities/Src/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DSTM32F405xx -DUSE_HAL_DRIVER -DSTM32F4xx -DARM_MATH_CM4 -DKNOB_STM -DDEBUG -c -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/DSP/Include -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/STM32Utilities/Inc" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/SharedSource" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/MathExtras_DSP" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/API" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
STM32Utilities/Src/DACX0504.o: E:/Repos/BeetTweek-Firmware-Public/BeetTweek-Firmware/STM32Utilities/Src/DACX0504.cpp STM32Utilities/Src/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DSTM32F405xx -DUSE_HAL_DRIVER -DSTM32F4xx -DARM_MATH_CM4 -DKNOB_STM -DDEBUG -c -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/DSP/Include -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/STM32Utilities/Inc" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/SharedSource" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/MathExtras_DSP" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/API" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
STM32Utilities/Src/MCP434X.o: E:/Repos/BeetTweek-Firmware-Public/BeetTweek-Firmware/STM32Utilities/Src/MCP434X.cpp STM32Utilities/Src/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DSTM32F405xx -DUSE_HAL_DRIVER -DSTM32F4xx -DARM_MATH_CM4 -DKNOB_STM -DDEBUG -c -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/DSP/Include -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/STM32Utilities/Inc" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/SharedSource" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/MathExtras_DSP" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/API" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
STM32Utilities/Src/MathExtras_Color.o: E:/Repos/BeetTweek-Firmware-Public/BeetTweek-Firmware/STM32Utilities/Src/MathExtras_Color.cpp STM32Utilities/Src/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DSTM32F405xx -DUSE_HAL_DRIVER -DSTM32F4xx -DARM_MATH_CM4 -DKNOB_STM -DDEBUG -c -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/DSP/Include -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/STM32Utilities/Inc" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/SharedSource" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/MathExtras_DSP" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/API" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
STM32Utilities/Src/PCM3168.o: E:/Repos/BeetTweek-Firmware-Public/BeetTweek-Firmware/STM32Utilities/Src/PCM3168.cpp STM32Utilities/Src/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DSTM32F405xx -DUSE_HAL_DRIVER -DSTM32F4xx -DARM_MATH_CM4 -DKNOB_STM -DDEBUG -c -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/DSP/Include -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/STM32Utilities/Inc" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/SharedSource" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/MathExtras_DSP" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/API" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
STM32Utilities/Src/Vector2.o: E:/Repos/BeetTweek-Firmware-Public/BeetTweek-Firmware/STM32Utilities/Src/Vector2.cpp STM32Utilities/Src/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DSTM32F405xx -DUSE_HAL_DRIVER -DSTM32F4xx -DARM_MATH_CM4 -DKNOB_STM -DDEBUG -c -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/DSP/Include -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/STM32Utilities/Inc" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/SharedSource" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/MathExtras_DSP" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/API" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
STM32Utilities/Src/Vector3.o: E:/Repos/BeetTweek-Firmware-Public/BeetTweek-Firmware/STM32Utilities/Src/Vector3.cpp STM32Utilities/Src/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DSTM32F405xx -DUSE_HAL_DRIVER -DSTM32F4xx -DARM_MATH_CM4 -DKNOB_STM -DDEBUG -c -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/DSP/Include -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/STM32Utilities/Inc" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/SharedSource" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/MathExtras_DSP" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/API" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
STM32Utilities/Src/Vector4.o: E:/Repos/BeetTweek-Firmware-Public/BeetTweek-Firmware/STM32Utilities/Src/Vector4.cpp STM32Utilities/Src/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DSTM32F405xx -DUSE_HAL_DRIVER -DSTM32F4xx -DARM_MATH_CM4 -DKNOB_STM -DDEBUG -c -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/DSP/Include -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/STM32Utilities/Inc" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/SharedSource" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/MathExtras_DSP" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/API" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
STM32Utilities/Src/ee24.o: E:/Repos/BeetTweek-Firmware-Public/BeetTweek-Firmware/STM32Utilities/Src/ee24.c STM32Utilities/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32F405xx -DUSE_HAL_DRIVER -DSTM32F4xx -DARM_MATH_CM4 -DKNOB_STM -DDEBUG -c -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/DSP/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
STM32Utilities/Src/us_delay.o: E:/Repos/BeetTweek-Firmware-Public/BeetTweek-Firmware/STM32Utilities/Src/us_delay.cpp STM32Utilities/Src/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DSTM32F405xx -DUSE_HAL_DRIVER -DSTM32F4xx -DARM_MATH_CM4 -DKNOB_STM -DDEBUG -c -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/DSP/Include -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/STM32Utilities/Inc" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/BeetTweek-Firmware/SharedSource" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/MathExtras_DSP" -I"C:/Users/casht/repos/subMatrixWorkSpace/BeetTweek-Firmware/API" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-STM32Utilities-2f-Src

clean-STM32Utilities-2f-Src:
	-$(RM) ./STM32Utilities/Src/AS5048.cyclo ./STM32Utilities/Src/AS5048.d ./STM32Utilities/Src/AS5048.o ./STM32Utilities/Src/AS5048.su ./STM32Utilities/Src/DACX0504.cyclo ./STM32Utilities/Src/DACX0504.d ./STM32Utilities/Src/DACX0504.o ./STM32Utilities/Src/DACX0504.su ./STM32Utilities/Src/MCP434X.cyclo ./STM32Utilities/Src/MCP434X.d ./STM32Utilities/Src/MCP434X.o ./STM32Utilities/Src/MCP434X.su ./STM32Utilities/Src/MathExtras_Color.cyclo ./STM32Utilities/Src/MathExtras_Color.d ./STM32Utilities/Src/MathExtras_Color.o ./STM32Utilities/Src/MathExtras_Color.su ./STM32Utilities/Src/PCM3168.cyclo ./STM32Utilities/Src/PCM3168.d ./STM32Utilities/Src/PCM3168.o ./STM32Utilities/Src/PCM3168.su ./STM32Utilities/Src/Vector2.cyclo ./STM32Utilities/Src/Vector2.d ./STM32Utilities/Src/Vector2.o ./STM32Utilities/Src/Vector2.su ./STM32Utilities/Src/Vector3.cyclo ./STM32Utilities/Src/Vector3.d ./STM32Utilities/Src/Vector3.o ./STM32Utilities/Src/Vector3.su ./STM32Utilities/Src/Vector4.cyclo ./STM32Utilities/Src/Vector4.d ./STM32Utilities/Src/Vector4.o ./STM32Utilities/Src/Vector4.su ./STM32Utilities/Src/ee24.cyclo ./STM32Utilities/Src/ee24.d ./STM32Utilities/Src/ee24.o ./STM32Utilities/Src/ee24.su ./STM32Utilities/Src/us_delay.cyclo ./STM32Utilities/Src/us_delay.d ./STM32Utilities/Src/us_delay.o ./STM32Utilities/Src/us_delay.su

.PHONY: clean-STM32Utilities-2f-Src

