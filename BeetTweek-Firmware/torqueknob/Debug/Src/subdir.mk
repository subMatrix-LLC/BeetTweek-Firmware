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
./Src/ee24.d 

OBJS += \
./Src/AS5048.o \
./Src/DACX0504.o \
./Src/MCP434X.o \
./Src/MathExtras_Color.o \
./Src/PCM3168.o \
./Src/Vector2.o \
./Src/Vector3.o \
./Src/Vector4.o \
./Src/ee24.o \
./Src/us_delay.o 

CPP_DEPS += \
./Src/AS5048.d \
./Src/DACX0504.d \
./Src/MCP434X.d \
./Src/MathExtras_Color.d \
./Src/PCM3168.d \
./Src/Vector2.d \
./Src/Vector3.d \
./Src/Vector4.d \
./Src/us_delay.d 


# Each subdirectory must supply rules for building sources it contributes
Src/AS5048.o: E:/Repos/BeetTweek-Firmware-Public/BeetTweek-Firmware/STM32Utilities/Src/AS5048.cpp Src/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DSTM32F405xx -DUSE_HAL_DRIVER -DSTM32F4xx -DARM_MATH_CM4 -DKNOB_STM -DDEBUG -c -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/DSP/Include -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Src/DACX0504.o: E:/Repos/BeetTweek-Firmware-Public/BeetTweek-Firmware/STM32Utilities/Src/DACX0504.cpp Src/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DSTM32F405xx -DUSE_HAL_DRIVER -DSTM32F4xx -DARM_MATH_CM4 -DKNOB_STM -DDEBUG -c -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/DSP/Include -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Src/MCP434X.o: E:/Repos/BeetTweek-Firmware-Public/BeetTweek-Firmware/STM32Utilities/Src/MCP434X.cpp Src/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DSTM32F405xx -DUSE_HAL_DRIVER -DSTM32F4xx -DARM_MATH_CM4 -DKNOB_STM -DDEBUG -c -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/DSP/Include -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Src/MathExtras_Color.o: E:/Repos/BeetTweek-Firmware-Public/BeetTweek-Firmware/STM32Utilities/Src/MathExtras_Color.cpp Src/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DSTM32F405xx -DUSE_HAL_DRIVER -DSTM32F4xx -DARM_MATH_CM4 -DKNOB_STM -DDEBUG -c -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/DSP/Include -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Src/PCM3168.o: E:/Repos/BeetTweek-Firmware-Public/BeetTweek-Firmware/STM32Utilities/Src/PCM3168.cpp Src/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DSTM32F405xx -DUSE_HAL_DRIVER -DSTM32F4xx -DARM_MATH_CM4 -DKNOB_STM -DDEBUG -c -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/DSP/Include -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Src/Vector2.o: E:/Repos/BeetTweek-Firmware-Public/BeetTweek-Firmware/STM32Utilities/Src/Vector2.cpp Src/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DSTM32F405xx -DUSE_HAL_DRIVER -DSTM32F4xx -DARM_MATH_CM4 -DKNOB_STM -DDEBUG -c -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/DSP/Include -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Src/Vector3.o: E:/Repos/BeetTweek-Firmware-Public/BeetTweek-Firmware/STM32Utilities/Src/Vector3.cpp Src/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DSTM32F405xx -DUSE_HAL_DRIVER -DSTM32F4xx -DARM_MATH_CM4 -DKNOB_STM -DDEBUG -c -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/DSP/Include -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Src/Vector4.o: E:/Repos/BeetTweek-Firmware-Public/BeetTweek-Firmware/STM32Utilities/Src/Vector4.cpp Src/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DSTM32F405xx -DUSE_HAL_DRIVER -DSTM32F4xx -DARM_MATH_CM4 -DKNOB_STM -DDEBUG -c -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/DSP/Include -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Src/ee24.o: E:/Repos/BeetTweek-Firmware-Public/BeetTweek-Firmware/STM32Utilities/Src/ee24.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32F405xx -DUSE_HAL_DRIVER -DSTM32F4xx -DARM_MATH_CM4 -DKNOB_STM -DDEBUG -c -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/DSP/Include -IC:/Users/casht/Desktop/TESTFOLDER/API -I"C:/Users/casht/Desktop/TESTFOLDER/BeetTweek-Firmware/STM32Utilities/Inc" -I"C:/Users/casht/Desktop/TESTFOLDER/BeetTweek-Firmware/SharedSource" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Src/us_delay.o: E:/Repos/BeetTweek-Firmware-Public/BeetTweek-Firmware/STM32Utilities/Src/us_delay.cpp Src/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DSTM32F405xx -DUSE_HAL_DRIVER -DSTM32F4xx -DARM_MATH_CM4 -DKNOB_STM -DDEBUG -c -I../Drivers/CMSIS/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/DSP/Include -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/AS5048.cyclo ./Src/AS5048.d ./Src/AS5048.o ./Src/AS5048.su ./Src/DACX0504.cyclo ./Src/DACX0504.d ./Src/DACX0504.o ./Src/DACX0504.su ./Src/MCP434X.cyclo ./Src/MCP434X.d ./Src/MCP434X.o ./Src/MCP434X.su ./Src/MathExtras_Color.cyclo ./Src/MathExtras_Color.d ./Src/MathExtras_Color.o ./Src/MathExtras_Color.su ./Src/PCM3168.cyclo ./Src/PCM3168.d ./Src/PCM3168.o ./Src/PCM3168.su ./Src/Vector2.cyclo ./Src/Vector2.d ./Src/Vector2.o ./Src/Vector2.su ./Src/Vector3.cyclo ./Src/Vector3.d ./Src/Vector3.o ./Src/Vector3.su ./Src/Vector4.cyclo ./Src/Vector4.d ./Src/Vector4.o ./Src/Vector4.su ./Src/ee24.cyclo ./Src/ee24.d ./Src/ee24.o ./Src/ee24.su ./Src/us_delay.cyclo ./Src/us_delay.d ./Src/us_delay.o ./Src/us_delay.su

.PHONY: clean-Src

