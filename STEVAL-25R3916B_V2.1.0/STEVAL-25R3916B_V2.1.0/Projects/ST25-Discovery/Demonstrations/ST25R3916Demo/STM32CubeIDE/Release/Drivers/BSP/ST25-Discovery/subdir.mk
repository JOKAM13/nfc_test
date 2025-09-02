################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Drivers/BSP/ST25-Discovery/st25_discovery.c \
C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Drivers/BSP/ST25-Discovery/st25_discovery_dpot.c \
C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Drivers/BSP/ST25-Discovery/st25_discovery_lcd.c \
C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Drivers/BSP/ST25-Discovery/st25_discovery_st25r.c \
C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Drivers/BSP/ST25-Discovery/st25_discovery_ts.c 

OBJS += \
./Drivers/BSP/ST25-Discovery/st25_discovery.o \
./Drivers/BSP/ST25-Discovery/st25_discovery_dpot.o \
./Drivers/BSP/ST25-Discovery/st25_discovery_lcd.o \
./Drivers/BSP/ST25-Discovery/st25_discovery_st25r.o \
./Drivers/BSP/ST25-Discovery/st25_discovery_ts.o 

C_DEPS += \
./Drivers/BSP/ST25-Discovery/st25_discovery.d \
./Drivers/BSP/ST25-Discovery/st25_discovery_dpot.d \
./Drivers/BSP/ST25-Discovery/st25_discovery_lcd.d \
./Drivers/BSP/ST25-Discovery/st25_discovery_st25r.d \
./Drivers/BSP/ST25-Discovery/st25_discovery_ts.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/ST25-Discovery/st25_discovery.o: C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Drivers/BSP/ST25-Discovery/st25_discovery.c Drivers/BSP/ST25-Discovery/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32L476xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/BSP/ST25-Discovery/st25_discovery_dpot.o: C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Drivers/BSP/ST25-Discovery/st25_discovery_dpot.c Drivers/BSP/ST25-Discovery/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32L476xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/BSP/ST25-Discovery/st25_discovery_lcd.o: C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Drivers/BSP/ST25-Discovery/st25_discovery_lcd.c Drivers/BSP/ST25-Discovery/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32L476xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/BSP/ST25-Discovery/st25_discovery_st25r.o: C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Drivers/BSP/ST25-Discovery/st25_discovery_st25r.c Drivers/BSP/ST25-Discovery/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32L476xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/BSP/ST25-Discovery/st25_discovery_ts.o: C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Drivers/BSP/ST25-Discovery/st25_discovery_ts.c Drivers/BSP/ST25-Discovery/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32L476xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-BSP-2f-ST25-2d-Discovery

clean-Drivers-2f-BSP-2f-ST25-2d-Discovery:
	-$(RM) ./Drivers/BSP/ST25-Discovery/st25_discovery.cyclo ./Drivers/BSP/ST25-Discovery/st25_discovery.d ./Drivers/BSP/ST25-Discovery/st25_discovery.o ./Drivers/BSP/ST25-Discovery/st25_discovery.su ./Drivers/BSP/ST25-Discovery/st25_discovery_dpot.cyclo ./Drivers/BSP/ST25-Discovery/st25_discovery_dpot.d ./Drivers/BSP/ST25-Discovery/st25_discovery_dpot.o ./Drivers/BSP/ST25-Discovery/st25_discovery_dpot.su ./Drivers/BSP/ST25-Discovery/st25_discovery_lcd.cyclo ./Drivers/BSP/ST25-Discovery/st25_discovery_lcd.d ./Drivers/BSP/ST25-Discovery/st25_discovery_lcd.o ./Drivers/BSP/ST25-Discovery/st25_discovery_lcd.su ./Drivers/BSP/ST25-Discovery/st25_discovery_st25r.cyclo ./Drivers/BSP/ST25-Discovery/st25_discovery_st25r.d ./Drivers/BSP/ST25-Discovery/st25_discovery_st25r.o ./Drivers/BSP/ST25-Discovery/st25_discovery_st25r.su ./Drivers/BSP/ST25-Discovery/st25_discovery_ts.cyclo ./Drivers/BSP/ST25-Discovery/st25_discovery_ts.d ./Drivers/BSP/ST25-Discovery/st25_discovery_ts.o ./Drivers/BSP/ST25-Discovery/st25_discovery_ts.su

.PHONY: clean-Drivers-2f-BSP-2f-ST25-2d-Discovery

