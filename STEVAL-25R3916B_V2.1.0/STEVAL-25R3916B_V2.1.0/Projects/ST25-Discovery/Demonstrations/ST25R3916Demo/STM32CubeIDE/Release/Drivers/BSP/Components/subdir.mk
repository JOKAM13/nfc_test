################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Drivers/BSP/Components/ad5112/ad5112.c \
C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Drivers/BSP/Components/ili9341_cube/ili9341_cube.c \
C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Drivers/BSP/Components/stmpe811/stmpe811.c 

OBJS += \
./Drivers/BSP/Components/ad5112.o \
./Drivers/BSP/Components/ili9341_cube.o \
./Drivers/BSP/Components/stmpe811.o 

C_DEPS += \
./Drivers/BSP/Components/ad5112.d \
./Drivers/BSP/Components/ili9341_cube.d \
./Drivers/BSP/Components/stmpe811.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/Components/ad5112.o: C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Drivers/BSP/Components/ad5112/ad5112.c Drivers/BSP/Components/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32L476xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/BSP/Components/ili9341_cube.o: C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Drivers/BSP/Components/ili9341_cube/ili9341_cube.c Drivers/BSP/Components/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32L476xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/BSP/Components/stmpe811.o: C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Drivers/BSP/Components/stmpe811/stmpe811.c Drivers/BSP/Components/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32L476xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-BSP-2f-Components

clean-Drivers-2f-BSP-2f-Components:
	-$(RM) ./Drivers/BSP/Components/ad5112.cyclo ./Drivers/BSP/Components/ad5112.d ./Drivers/BSP/Components/ad5112.o ./Drivers/BSP/Components/ad5112.su ./Drivers/BSP/Components/ili9341_cube.cyclo ./Drivers/BSP/Components/ili9341_cube.d ./Drivers/BSP/Components/ili9341_cube.o ./Drivers/BSP/Components/ili9341_cube.su ./Drivers/BSP/Components/stmpe811.cyclo ./Drivers/BSP/Components/stmpe811.d ./Drivers/BSP/Components/stmpe811.o ./Drivers/BSP/Components/stmpe811.su

.PHONY: clean-Drivers-2f-BSP-2f-Components

