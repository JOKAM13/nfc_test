################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Middlewares/ST/p2p/lib_llcp.c \
C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Middlewares/ST/p2p/lib_snep.c 

OBJS += \
./Middleware/p2p/lib_llcp.o \
./Middleware/p2p/lib_snep.o 

C_DEPS += \
./Middleware/p2p/lib_llcp.d \
./Middleware/p2p/lib_snep.d 


# Each subdirectory must supply rules for building sources it contributes
Middleware/p2p/lib_llcp.o: C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Middlewares/ST/p2p/lib_llcp.c Middleware/p2p/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32L476xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middleware/p2p/lib_snep.o: C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Middlewares/ST/p2p/lib_snep.c Middleware/p2p/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32L476xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middleware-2f-p2p

clean-Middleware-2f-p2p:
	-$(RM) ./Middleware/p2p/lib_llcp.cyclo ./Middleware/p2p/lib_llcp.d ./Middleware/p2p/lib_llcp.o ./Middleware/p2p/lib_llcp.su ./Middleware/p2p/lib_snep.cyclo ./Middleware/p2p/lib_snep.d ./Middleware/p2p/lib_snep.o ./Middleware/p2p/lib_snep.su

.PHONY: clean-Middleware-2f-p2p

