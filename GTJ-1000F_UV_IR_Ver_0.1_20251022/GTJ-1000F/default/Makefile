###############################################################################
# Makefile for the project GTJ-1000F
###############################################################################

## General Flags
PROJECT = GTJ-1000F
MCU = atmega2560
TARGET = GTJ-1000F.elf
CC = avr-gcc

CPP = avr-g++

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99       -DF_CPU=16000000UL -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=GTJ-1000F.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = adc.o eeprom.o gpio.o lcd.o main.o mkey.o mmi.o modbus.o potentiometer.o sw_lib.o timer.o uart2560.o dac7571.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) GTJ-1000F.hex GTJ-1000F.eep GTJ-1000F.lss size

## Compile
adc.o: ../adc.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

eeprom.o: ../eeprom.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

gpio.o: ../gpio.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

lcd.o: ../lcd.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

main.o: ../main.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

mkey.o: ../mkey.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

mmi.o: ../mmi.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

modbus.o: ../modbus.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

potentiometer.o: ../potentiometer.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

sw_lib.o: ../sw_lib.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

timer.o: ../timer.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

uart2560.o: ../uart2560.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

dac7571.o: ../dac7571.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: ${TARGET}
	@echo
	@avr-size -C --mcu=${MCU} ${TARGET}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) GTJ-1000F.elf dep/* GTJ-1000F.hex GTJ-1000F.eep GTJ-1000F.lss GTJ-1000F.map


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

