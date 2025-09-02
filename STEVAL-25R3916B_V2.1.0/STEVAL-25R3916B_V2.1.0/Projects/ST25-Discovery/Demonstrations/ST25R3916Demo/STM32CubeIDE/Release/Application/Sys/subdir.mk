################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Application/Sys/syscalls.c \
../Application/Sys/sysmem.c 

OBJS += \
./Application/Sys/syscalls.o \
./Application/Sys/sysmem.o 

C_DEPS += \
./Application/Sys/syscalls.d \
./Application/Sys/sysmem.d 


# Each subdirectory must supply rules for building sources it contributes
Application/Sys/%.o Application/Sys/%.su Application/Sys/%.cyclo: ../Application/Sys/%.c Application/Sys/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32L476xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Application-2f-Sys

clean-Application-2f-Sys:
	-$(RM) ./Application/Sys/syscalls.cyclo ./Application/Sys/syscalls.d ./Application/Sys/syscalls.o ./Application/Sys/syscalls.su ./Application/Sys/sysmem.cyclo ./Application/Sys/sysmem.d ./Application/Sys/sysmem.o ./Application/Sys/sysmem.su

.PHONY: clean-Application-2f-Sys

