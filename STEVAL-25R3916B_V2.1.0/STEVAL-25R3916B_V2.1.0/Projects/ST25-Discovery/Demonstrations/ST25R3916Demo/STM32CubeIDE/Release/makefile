################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Middleware/p2p/subdir.mk
-include Middleware/menu_demo/subdir.mk
-include Middleware/fw_3916/subdir.mk
-include Middleware/STM32_USB_Device_Library/subdir.mk
-include Middleware/Reader_common/subdir.mk
-include Middleware/RFAL/subdir.mk
-include Middleware/NDEF/subdir.mk
-include Middleware/JPEG/subdir.mk
-include Example/Sys/subdir.mk
-include Example/Startup/subdir.mk
-include Example/subdir.mk
-include Drivers/STM32L4xx_HAL_Driver/subdir.mk
-include Drivers/BSP/ST25-Discovery/subdir.mk
-include Drivers/BSP/Components/subdir.mk
-include Application/Sys/subdir.mk
-include Application/Startup/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := STEVAL25R3916B
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
STEVAL25R3916B.elf \

MAP_FILES += \
STEVAL25R3916B.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
STEVAL25R3916B.list \


# All Target
all: main-build

# Main-build Target
main-build: STEVAL25R3916B.elf secondary-outputs

# Tool invocations
STEVAL25R3916B.elf STEVAL25R3916B.map: $(OBJS) $(USER_OBJS) C:\Users\lenat\STM32CubeIDE\workspace_1.18.1\STEVAL-25R3916B_V2.1.0\Projects\ST25-Discovery\Demonstrations\ST25R3916Demo\STM32CubeIDE\STM32L476VGTX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "STEVAL25R3916B.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"C:\Users\lenat\STM32CubeIDE\workspace_1.18.1\STEVAL-25R3916B_V2.1.0\Projects\ST25-Discovery\Demonstrations\ST25R3916Demo\STM32CubeIDE\STM32L476VGTX_FLASH.ld" --specs=nosys.specs -Wl,-Map="STEVAL25R3916B.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

STEVAL25R3916B.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "STEVAL25R3916B.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) STEVAL25R3916B.elf STEVAL25R3916B.list STEVAL25R3916B.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
