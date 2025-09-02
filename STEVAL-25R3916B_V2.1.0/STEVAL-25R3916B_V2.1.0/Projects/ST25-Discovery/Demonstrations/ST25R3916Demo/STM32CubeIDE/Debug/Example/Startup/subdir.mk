################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../Application/Startup/startup_stm32l476vgtx.s 

OBJS += \
./Example/Startup/startup_stm32l476vgtx.o 

S_DEPS += \
./Example/Startup/startup_stm32l476vgtx.d 


# Each subdirectory must supply rules for building sources it contributes
Example/Startup/startup_stm32l476vgtx.o: C:/Users/lenat/STM32CubeIDE/workspace_1.18.1/STEVAL-25R3916B_V2.1.0/Projects/ST25-Discovery/Demonstrations/ST25R3916Demo/STM32CubeIDE/Application/Startup/startup_stm32l476vgtx.s Example/Startup/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m4 -g3 -DDEBUG -c -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

clean: clean-Example-2f-Startup

clean-Example-2f-Startup:
	-$(RM) ./Example/Startup/startup_stm32l476vgtx.d ./Example/Startup/startup_stm32l476vgtx.o

.PHONY: clean-Example-2f-Startup

