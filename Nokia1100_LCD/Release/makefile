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
lib1100.lss \

FLASH_IMAGE += \
lib1100.hex \

EEPROM_IMAGE += \
lib1100.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: lib1100.elf secondary-outputs

# Tool invocations
lib1100.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,lib1100.map -mmcu=atmega328p -o "lib1100.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

lib1100.lss: lib1100.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S lib1100.elf  >"lib1100.lss"
	@echo 'Finished building: $@'
	@echo ' '

lib1100.hex: lib1100.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -O ihex lib1100.elf  "lib1100.hex"
	@echo 'Finished building: $@'
	@echo ' '

lib1100.eep: lib1100.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex lib1100.elf  "lib1100.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: lib1100.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega328p lib1100.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ASM_DEPS)$(EEPROM_IMAGE)$(FLASH_IMAGE)$(ELFS)$(LSS)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS) lib1100.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
