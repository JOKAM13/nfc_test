################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Middlewares/ST/menu_demo/Menu_Plot.c \
C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Middlewares/ST/menu_demo/Menu_core.c \
C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Middlewares/ST/menu_demo/Menu_cursor.c \
C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Middlewares/ST/menu_demo/Menu_tab.c 

OBJS += \
./Middleware/menu_demo/Menu_Plot.o \
./Middleware/menu_demo/Menu_core.o \
./Middleware/menu_demo/Menu_cursor.o \
./Middleware/menu_demo/Menu_tab.o 

C_DEPS += \
./Middleware/menu_demo/Menu_Plot.d \
./Middleware/menu_demo/Menu_core.d \
./Middleware/menu_demo/Menu_cursor.d \
./Middleware/menu_demo/Menu_tab.d 


# Each subdirectory must supply rules for building sources it contributes
Middleware/menu_demo/Menu_Plot.o: C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Middlewares/ST/menu_demo/Menu_Plot.c Middleware/menu_demo/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32L476xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middleware/menu_demo/Menu_core.o: C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Middlewares/ST/menu_demo/Menu_core.c Middleware/menu_demo/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32L476xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middleware/menu_demo/Menu_cursor.o: C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Middlewares/ST/menu_demo/Menu_cursor.c Middleware/menu_demo/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32L476xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middleware/menu_demo/Menu_tab.o: C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Middlewares/ST/menu_demo/Menu_tab.c Middleware/menu_demo/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32L476xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middleware-2f-menu_demo

clean-Middleware-2f-menu_demo:
	-$(RM) ./Middleware/menu_demo/Menu_Plot.cyclo ./Middleware/menu_demo/Menu_Plot.d ./Middleware/menu_demo/Menu_Plot.o ./Middleware/menu_demo/Menu_Plot.su ./Middleware/menu_demo/Menu_core.cyclo ./Middleware/menu_demo/Menu_core.d ./Middleware/menu_demo/Menu_core.o ./Middleware/menu_demo/Menu_core.su ./Middleware/menu_demo/Menu_cursor.cyclo ./Middleware/menu_demo/Menu_cursor.d ./Middleware/menu_demo/Menu_cursor.o ./Middleware/menu_demo/Menu_cursor.su ./Middleware/menu_demo/Menu_tab.cyclo ./Middleware/menu_demo/Menu_tab.d ./Middleware/menu_demo/Menu_tab.o ./Middleware/menu_demo/Menu_tab.su

.PHONY: clean-Middleware-2f-menu_demo

