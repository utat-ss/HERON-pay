# This makefile should go in the examples folder, harness_tests folder, or
# manual_tests folder of a repository

# Reference: http://www.atmel.com/webdoc/avrlibcreferencemanual/group__demo__project_1demo_project_compile.html


# Parameters that might need to be changed, depending on the repository
#-------------------------------------------------------------------------------
# Path to lib-common (relative to current directory)
LIB_COMMON = ../../lib-common
# Includes (header files)
INCLUDES = -I$(LIB_COMMON)/include
# Libraries from lib-common to link
# For some reason, conversions needs to come after dac or else it gives an error
# Need to put dac before conversions, uptime before timer, heartbeat before can,
# or else gives an error for undefined reference
LIB = -L$(LIB_COMMON)/lib -ladc -lheartbeat -lcan -ldac -lconversions -lpex -lqueue -lspi -lstack -ltest -luptime -ltimer -luart -lutilities -lwatchdog -lprintf_flt -lm
# Name of microcontroller ("32m1" or "64m1")
MCU = 64m1
#-------------------------------------------------------------------------------


# AVR-GCC compiler
CC = avr-gcc
# Compiler flags
# Should have "-mmcu=atmega32m1" or "-mmcu=atmega64m1"
CFLAGS = -std=gnu99 -Wall -Wl,-u,vfprintf -g -mmcu=atmega$(MCU) -Os -mcall-prologues
# Programmer
PGMR = stk500
# AVR device for avrdude uploading - must be prefixed with "m"
DEVICE = m$(MCU)


# Detect operating system - based on https://gist.github.com/sighingnow/deee806603ec9274fd47

# One of these flags will be set to true based on the operating system
WINDOWS := false
MAC_OS := false
LINUX := false

REMOTE := false

ifeq ($(OS),Windows_NT)
	WINDOWS := true
else
	# Unix - get the operating system
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Darwin)
		MAC_OS := true
	endif
	ifeq ($(UNAME_S),Linux)
		LINUX := true
	endif
endif

# PORT - Computer port that the programmer is connected to
# Try to automatically detect the port
ifeq ($(WINDOWS), true)
	# higher number
	PORT = $(shell powershell "[System.IO.Ports.SerialPort]::getportnames() | sort | select -First 2 | select -Last 1")
endif
ifeq ($(MAC_OS), true)
	# lower number
	PORT = $(shell find /dev -name 'tty.usbmodem[0-9]*' | sort | head -n1)
endif
ifeq ($(LINUX), true)
	# lower number
	PORT = $(shell pavr2cmd --prog-port)
	UART = $(shell pavr2cmd --ttl-port)
	ifeq ($(shell whoami),ss)
		REMOTE := true
	endif
endif

# Special commands
.PHONY: all clean debug help lib-common read-eeprom upload

# SRC - defined in the example-specific makefile
# All .c files in src map to .o files
OBJ = $(SRC:../../src/%.c=./%.o)

all: $(PROG)

# Link .o files to make .elf program, then copies to .hex file
$(PROG): $(PROG).o $(OBJ)
	$(CC) $(CFLAGS) -o $@.elf $^ $(LIB)
	avr-objcopy -j .text -j .data -O ihex $@.elf $@.hex

# .o files depend on .c files
$(PROG).o: $(PROG).c
	$(CC) $(CFLAGS) -c $(PROG).c $(INCLUDES)

# Compile .c file to .o files
./%.o: ../../src/%.c
	$(CC) $(CFLAGS) -o $@ -c $< $(INCLUDES)

clean:
	rm -f ./*.o
	rm -f ./*.elf
	rm -f ./*.hex

# Print debug information
debug:
	@echo ------------
	@echo "WINDOWS:" $(WINDOWS)
	@echo ------------
	@echo "MAC_OS:" $(MAC_OS)
	@echo ------------
	@echo "LINUX:" $(LINUX)
	@echo ------------
	@echo "SRC:" $(SRC)
	@echo ------------
	@echo "OBJ:" $(OBJ)
	@echo ------------

help:
	@echo "usage: make [all | clean | debug | help | lib-common | read-eeprom | upload]"
	@echo "Running make without any arguments is equivalent to running make all."
	@echo "all            build this program"
	@echo "clean          clear build files"
	@echo "debug          display debugging information"
	@echo "help           display this help message"
	@echo "lib-common     fetch and build the latest version of lib-common"
	@echo "read-eeprom    read and display the contents of the microcontroller's EEPROM"
	@echo "upload         upload this program to a board"

# Update and build lib-common
lib-common:
	@echo "Fetching latest version of lib-common..."
	git submodule update --remote
	@echo "Compiling lib-common..."
	make -C $(LIB_COMMON) clean MCU=$(MCU)
	make -C $(LIB_COMMON) MCU=$(MCU)

# Create a file called eeprom.bin, which contains a raw binary copy of the micro's EEPROM memory.
# View the contents of the binary file in hex
read-eeprom:
	@echo "Reading EEPROM to binary file eeprom.bin..."
	avrdude -p $(DEVICE) -c stk500 -P $(PORT) -U eeprom:r:eeprom.bin:r
	@echo "Displaying eeprom.bin in hex..."
ifeq ($(WINDOWS), true)
	powershell Format-Hex eeprom.bin
else
	hexdump eeprom.bin
endif

# Upload .hex program to device
upload: $(PROG)
ifeq ($(REMOTE),true)
	gpio -g mode 14 IN
	gpio -g mode 15 IN
endif
	avrdude -p $(DEVICE) -c $(PGMR) -C $(LIB_COMMON)/avrdude.conf -P $(PORT) -U flash:w:./$^.hex
ifeq ($(REMOTE),true)
	gpio -g mode 14 ALT0
	gpio -g mode 15 ALT0
endif

# Upload .hex program to device, using a precompiled binary that must already be
# in the folder and be named $(PROG).hex
# (does not compile anything, just uploads the hex file)
upload_bin:
	avrdude -p $(DEVICE) -c $(PGMR) -C $(LIB_COMMON)/avrdude.conf -P $(PORT) -U flash:w:./$(PROG).hex
