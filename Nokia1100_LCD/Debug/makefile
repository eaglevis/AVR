################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include lcd1100/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LSS += \
Nokia1100_LCD.lss \

FLASH_IMAGE += \
Nokia1100_LCD.hex \

SIZEDUMMY += \
sizedummy \


# All Target
all: Nokia1100_LCD.elf secondary-outputs

# Tool invocations
Nokia1100_LCD.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,Nokia1100_LCD.map -mmcu=atmega328p -o "Nokia1100_LCD.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

Nokia1100_LCD.lss: Nokia1100_LCD.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S Nokia1100_LCD.elf  >"Nokia1100_LCD.lss"
	@echo 'Finished building: $@'
	@echo ' '

Nokia1100_LCD.hex: Nokia1100_LCD.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -O ihex Nokia1100_LCD.elf  "Nokia1100_LCD.hex"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: Nokia1100_LCD.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega328p Nokia1100_LCD.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ASM_DEPS)$(FLASH_IMAGE)$(ELFS)$(LSS)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS) Nokia1100_LCD.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
