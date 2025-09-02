################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Middlewares/ST/Reader_common/firmware/STM/STM32/Src/bootloader.c \
C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Middlewares/ST/Reader_common/firmware/STM/STM32/Src/flash_driver.c \
C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Middlewares/ST/Reader_common/firmware/STM/STM32/Src/logger.c \
C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Middlewares/ST/Reader_common/firmware/STM/STM32/Src/pac.c \
C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Middlewares/ST/Reader_common/firmware/STM/STM32/Src/spi.c \
C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Middlewares/ST/Reader_common/firmware/STM/utils/Src/stream_dispatcher.c \
C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Middlewares/ST/Reader_common/firmware/STM/STM32/Src/timer.c 

OBJS += \
./Middleware/Reader_common/bootloader.o \
./Middleware/Reader_common/flash_driver.o \
./Middleware/Reader_common/logger.o \
./Middleware/Reader_common/pac.o \
./Middleware/Reader_common/spi.o \
./Middleware/Reader_common/stream_dispatcher.o \
./Middleware/Reader_common/timer.o 

C_DEPS += \
./Middleware/Reader_common/bootloader.d \
./Middleware/Reader_common/flash_driver.d \
./Middleware/Reader_common/logger.d \
./Middleware/Reader_common/pac.d \
./Middleware/Reader_common/spi.d \
./Middleware/Reader_common/stream_dispatcher.d \
./Middleware/Reader_common/timer.d 


# Each subdirectory must supply rules for building sources it contributes
Middleware/Reader_common/bootloader.o: C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Middlewares/ST/Reader_common/firmware/STM/STM32/Src/bootloader.c Middleware/Reader_common/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32L476xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middleware/Reader_common/flash_driver.o: C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Middlewares/ST/Reader_common/firmware/STM/STM32/Src/flash_driver.c Middleware/Reader_common/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32L476xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middleware/Reader_common/logger.o: C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Middlewares/ST/Reader_common/firmware/STM/STM32/Src/logger.c Middleware/Reader_common/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32L476xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middleware/Reader_common/pac.o: C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Middlewares/ST/Reader_common/firmware/STM/STM32/Src/pac.c Middleware/Reader_common/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32L476xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middleware/Reader_common/spi.o: C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Middlewares/ST/Reader_common/firmware/STM/STM32/Src/spi.c Middleware/Reader_common/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32L476xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middleware/Reader_common/stream_dispatcher.o: C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Middlewares/ST/Reader_common/firmware/STM/utils/Src/stream_dispatcher.c Middleware/Reader_common/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32L476xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middleware/Reader_common/timer.o: C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Middlewares/ST/Reader_common/firmware/STM/STM32/Src/timer.c Middleware/Reader_common/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32L476xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middleware-2f-Reader_common

clean-Middleware-2f-Reader_common:
	-$(RM) ./Middleware/Reader_common/bootloader.cyclo ./Middleware/Reader_common/bootloader.d ./Middleware/Reader_common/bootloader.o ./Middleware/Reader_common/bootloader.su ./Middleware/Reader_common/flash_driver.cyclo ./Middleware/Reader_common/flash_driver.d ./Middleware/Reader_common/flash_driver.o ./Middleware/Reader_common/flash_driver.su ./Middleware/Reader_common/logger.cyclo ./Middleware/Reader_common/logger.d ./Middleware/Reader_common/logger.o ./Middleware/Reader_common/logger.su ./Middleware/Reader_common/pac.cyclo ./Middleware/Reader_common/pac.d ./Middleware/Reader_common/pac.o ./Middleware/Reader_common/pac.su ./Middleware/Reader_common/spi.cyclo ./Middleware/Reader_common/spi.d ./Middleware/Reader_common/spi.o ./Middleware/Reader_common/spi.su ./Middleware/Reader_common/stream_dispatcher.cyclo ./Middleware/Reader_common/stream_dispatcher.d ./Middleware/Reader_common/stream_dispatcher.o ./Middleware/Reader_common/stream_dispatcher.su ./Middleware/Reader_common/timer.cyclo ./Middleware/Reader_common/timer.d ./Middleware/Reader_common/timer.o ./Middleware/Reader_common/timer.su

.PHONY: clean-Middleware-2f-Reader_common

