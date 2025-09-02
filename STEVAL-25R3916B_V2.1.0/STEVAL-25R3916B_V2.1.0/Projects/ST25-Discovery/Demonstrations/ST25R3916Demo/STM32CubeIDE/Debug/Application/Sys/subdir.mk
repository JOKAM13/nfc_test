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
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu99 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -DUSE_LCD -DMENU_DEMO_CENTER_ICONS=1 -DUSE_LOGGER=1 -DUSE_MB1749_A=1 -DST25R3916B -DUSE_M25R16B -DUSE_JOYSTICKONLY '-DANALOG_CONFIG_OFFSET=(0x000E1000U)' -c -I../../Inc -I../../Picture -I../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../Drivers/CMSIS/Include -I../../../../../../Drivers/BSP/ST25-Discovery -I../../../../../../Drivers/BSP/Components/Common -I../../../../../../Drivers/BSP/Components/stmpe811 -I../../../../../../Drivers/BSP/Components/ad5112 -I../../../../../../Drivers/BSP/Components/ili9341_cube -I../../../../../../Middlewares/Third_Party/LibJPEG/include -I../../../../../../Middlewares/ST/menu_demo -I../../../../../../Middlewares/ST/NDEF/include/message -I../../../../../../Middlewares/ST/NDEF/include/poller -I../../../../../../Middlewares/ST/RFAL/include -I../../../../../../Middlewares/ST/RFAL/source -I../../../../../../Middlewares/ST/RFAL/source/st25r3916 -I../../../../../../Middlewares/ST/Reader_common/firmware/STM/utils/Inc -I../../../../../../Middlewares/ST/Reader_common/firmware/STM/STM32/Inc -I../../../../../../Middlewares/ST/fw_3916/DISCO-STM32L4x6/Inc -I../../../../../../Middlewares/ST/p2p -I../../../../../../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../../../../../../Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Inc -I../../../../../../Middlewares/ST/STM32_USB_Device_Library/Class/HID/Inc -I../../../../../../Middlewares/Third_Party/LibJPEG/include -I../../../../../../Utilities/Fonts -O1 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Application-2f-Sys

clean-Application-2f-Sys:
	-$(RM) ./Application/Sys/syscalls.cyclo ./Application/Sys/syscalls.d ./Application/Sys/syscalls.o ./Application/Sys/syscalls.su ./Application/Sys/sysmem.cyclo ./Application/Sys/sysmem.d ./Application/Sys/sysmem.o ./Application/Sys/sysmem.su

.PHONY: clean-Application-2f-Sys

