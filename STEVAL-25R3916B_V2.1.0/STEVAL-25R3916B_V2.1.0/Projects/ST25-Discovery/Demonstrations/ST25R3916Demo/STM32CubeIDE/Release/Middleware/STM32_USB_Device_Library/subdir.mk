################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.c \
C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c \
C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Src/usbd_customhid.c \
C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Middlewares/ST/STM32_USB_Device_Library/Class/HID/Src/usbd_hid.c \
C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c 

OBJS += \
./Middleware/STM32_USB_Device_Library/usbd_core.o \
./Middleware/STM32_USB_Device_Library/usbd_ctlreq.o \
./Middleware/STM32_USB_Device_Library/usbd_customhid.o \
./Middleware/STM32_USB_Device_Library/usbd_hid.o \
./Middleware/STM32_USB_Device_Library/usbd_ioreq.o 

C_DEPS += \
./Middleware/STM32_USB_Device_Library/usbd_core.d \
./Middleware/STM32_USB_Device_Library/usbd_ctlreq.d \
./Middleware/STM32_USB_Device_Library/usbd_customhid.d \
./Middleware/STM32_USB_Device_Library/usbd_hid.d \
./Middleware/STM32_USB_Device_Library/usbd_ioreq.d 


# Each subdirectory must supply rules for building sources it contributes
Middleware/STM32_USB_Device_Library/usbd_core.o: C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.c Middleware/STM32_USB_Device_Library/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32L476xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middleware/STM32_USB_Device_Library/usbd_ctlreq.o: C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c Middleware/STM32_USB_Device_Library/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32L476xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middleware/STM32_USB_Device_Library/usbd_customhid.o: C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Src/usbd_customhid.c Middleware/STM32_USB_Device_Library/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32L476xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middleware/STM32_USB_Device_Library/usbd_hid.o: C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Middlewares/ST/STM32_USB_Device_Library/Class/HID/Src/usbd_hid.c Middleware/STM32_USB_Device_Library/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32L476xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middleware/STM32_USB_Device_Library/usbd_ioreq.o: C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c Middleware/STM32_USB_Device_Library/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32L476xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middleware-2f-STM32_USB_Device_Library

clean-Middleware-2f-STM32_USB_Device_Library:
	-$(RM) ./Middleware/STM32_USB_Device_Library/usbd_core.cyclo ./Middleware/STM32_USB_Device_Library/usbd_core.d ./Middleware/STM32_USB_Device_Library/usbd_core.o ./Middleware/STM32_USB_Device_Library/usbd_core.su ./Middleware/STM32_USB_Device_Library/usbd_ctlreq.cyclo ./Middleware/STM32_USB_Device_Library/usbd_ctlreq.d ./Middleware/STM32_USB_Device_Library/usbd_ctlreq.o ./Middleware/STM32_USB_Device_Library/usbd_ctlreq.su ./Middleware/STM32_USB_Device_Library/usbd_customhid.cyclo ./Middleware/STM32_USB_Device_Library/usbd_customhid.d ./Middleware/STM32_USB_Device_Library/usbd_customhid.o ./Middleware/STM32_USB_Device_Library/usbd_customhid.su ./Middleware/STM32_USB_Device_Library/usbd_hid.cyclo ./Middleware/STM32_USB_Device_Library/usbd_hid.d ./Middleware/STM32_USB_Device_Library/usbd_hid.o ./Middleware/STM32_USB_Device_Library/usbd_hid.su ./Middleware/STM32_USB_Device_Library/usbd_ioreq.cyclo ./Middleware/STM32_USB_Device_Library/usbd_ioreq.d ./Middleware/STM32_USB_Device_Library/usbd_ioreq.o ./Middleware/STM32_USB_Device_Library/usbd_ioreq.su

.PHONY: clean-Middleware-2f-STM32_USB_Device_Library

