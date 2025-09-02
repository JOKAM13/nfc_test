################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Application/Sys/syscalls.c \
../Application/Sys/sysmem.c 

OBJS += \
./Example/Sys/syscalls.o \
./Example/Sys/sysmem.o 

C_DEPS += \
./Example/Sys/syscalls.d \
./Example/Sys/sysmem.d 


# Each subdirectory must supply rules for building sources it contributes
Example/Sys/syscalls.o: C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Projects/ST25-Discovery/Demonstrations/ST25R3916Demo/STM32CubeIDE/Application/Sys/syscalls.c Example/Sys/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu99 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DUSE_LCD -DMENU_DEMO_CENTER_ICONS=1 -DUSE_LOGGER=1 -DUSE_MB1749_A=1 -DST25R3916B -DUSE_M25R16B -DUSE_JOYSTICKONLY '-DANALOG_CONFIG_OFFSET=(0x000E1000U)' -c -I../../Inc -I../../Picture -I../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../Drivers/CMSIS/Include -I../../../../../../Drivers/BSP/ST25-Discovery -I../../../../../../Drivers/BSP/Components/Common -I../../../../../../Drivers/BSP/Components/stmpe811 -I../../../../../../Drivers/BSP/Components/ad5112 -I../../../../../../Drivers/BSP/Components/ili9341_cube -I../../../../../../Middlewares/Third_Party/LibJPEG/include -I../../../../../../Middlewares/ST/menu_demo -I../../../../../../Middlewares/ST/NDEF/include/message -I../../../../../../Middlewares/ST/NDEF/include/poller -I../../../../../../Middlewares/ST/RFAL/include -I../../../../../../Middlewares/ST/RFAL/source -I../../../../../../Middlewares/ST/RFAL/source/st25r3916 -I../../../../../../Middlewares/ST/Reader_common/firmware/STM/utils/Inc -I../../../../../../Middlewares/ST/Reader_common/firmware/STM/STM32/Inc -I../../../../../../Middlewares/ST/fw_3916/DISCO-STM32L4x6/Inc -I../../../../../../Middlewares/ST/p2p -I../../../../../../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../../../../../../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../../../../../../Middlewares/ST/STM32_USB_Device_Library/Class/HID/Inc -I../../../../../../Middlewares/Third_Party/LibJPEG/include -I../../../../../../Utilities/Fonts -O1 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Example/Sys/sysmem.o: C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Projects/ST25-Discovery/Demonstrations/ST25R3916Demo/STM32CubeIDE/Application/Sys/sysmem.c Example/Sys/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu99 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DUSE_LCD -DMENU_DEMO_CENTER_ICONS=1 -DUSE_LOGGER=1 -DUSE_MB1749_A=1 -DST25R3916B -DUSE_M25R16B -DUSE_JOYSTICKONLY '-DANALOG_CONFIG_OFFSET=(0x000E1000U)' -c -I../../Inc -I../../Picture -I../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../Drivers/CMSIS/Include -I../../../../../../Drivers/BSP/ST25-Discovery -I../../../../../../Drivers/BSP/Components/Common -I../../../../../../Drivers/BSP/Components/stmpe811 -I../../../../../../Drivers/BSP/Components/ad5112 -I../../../../../../Drivers/BSP/Components/ili9341_cube -I../../../../../../Middlewares/Third_Party/LibJPEG/include -I../../../../../../Middlewares/ST/menu_demo -I../../../../../../Middlewares/ST/NDEF/include/message -I../../../../../../Middlewares/ST/NDEF/include/poller -I../../../../../../Middlewares/ST/RFAL/include -I../../../../../../Middlewares/ST/RFAL/source -I../../../../../../Middlewares/ST/RFAL/source/st25r3916 -I../../../../../../Middlewares/ST/Reader_common/firmware/STM/utils/Inc -I../../../../../../Middlewares/ST/Reader_common/firmware/STM/STM32/Inc -I../../../../../../Middlewares/ST/fw_3916/DISCO-STM32L4x6/Inc -I../../../../../../Middlewares/ST/p2p -I../../../../../../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../../../../../../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../../../../../../Middlewares/ST/STM32_USB_Device_Library/Class/HID/Inc -I../../../../../../Middlewares/Third_Party/LibJPEG/include -I../../../../../../Utilities/Fonts -O1 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Example-2f-Sys

clean-Example-2f-Sys:
	-$(RM) ./Example/Sys/syscalls.cyclo ./Example/Sys/syscalls.d ./Example/Sys/syscalls.o ./Example/Sys/syscalls.su ./Example/Sys/sysmem.cyclo ./Example/Sys/sysmem.d ./Example/Sys/sysmem.o ./Example/Sys/sysmem.su

.PHONY: clean-Example-2f-Sys

