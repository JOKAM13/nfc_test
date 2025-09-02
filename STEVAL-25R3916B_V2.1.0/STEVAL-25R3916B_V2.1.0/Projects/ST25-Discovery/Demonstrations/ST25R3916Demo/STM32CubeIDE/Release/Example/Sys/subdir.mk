################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
OBJS += \
./Example/Sys/syscalls.o \
./Example/Sys/sysmem.o 

C_DEPS += \
./Example/Sys/syscalls.d \
./Example/Sys/sysmem.d 


# Each subdirectory must supply rules for building sources it contributes
Example/Sys/syscalls.o: C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Projects/ST25-Discovery/Demonstrations/ST25R3916Demo/STM32CubeIDE/Application/Sys/syscalls.c Example/Sys/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32L476xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Example/Sys/sysmem.o: C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Projects/ST25-Discovery/Demonstrations/ST25R3916Demo/STM32CubeIDE/Application/Sys/sysmem.c Example/Sys/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32L476xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Example-2f-Sys

clean-Example-2f-Sys:
	-$(RM) ./Example/Sys/syscalls.cyclo ./Example/Sys/syscalls.d ./Example/Sys/syscalls.o ./Example/Sys/syscalls.su ./Example/Sys/sysmem.cyclo ./Example/Sys/sysmem.d ./Example/Sys/sysmem.o ./Example/Sys/sysmem.su

.PHONY: clean-Example-2f-Sys

