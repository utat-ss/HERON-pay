# This makefile should go in the manual_tests folder or examples folder of a repository

# Reference: http://www.atmel.com/webdoc/avrlibcreferencemanual/group__demo__project_1demo_project_compile.html


# Parameters that might need to be changed, depending on the repository
#-------------------------------------------------------------------------------
# Includes (header files)
INCLUDES = -I../../lib-common/include
# Libraries from lib-common to link
LIB = -L../../lib-common/lib -ladc -lcan -lconversions -ldac -lheartbeat -lpex -lqueue -lspi -ltimer -luart -lutilities -lprintf_flt -lm
#-------------------------------------------------------------------------------


# AVR-GCC compiler
CC = avr-gcc
# Compiler flags
CFLAGS = -std=gnu99 -Wall -Wl,-u,vfprintf -g -mmcu=atmega32m1 -Os -mcall-prologues
# Programmer
PGMR = stk500
# Microcontroller
MCU = m32m1


# Detect operating system - based on https://gist.github.com/sighingnow/deee806603ec9274fd47

# One of these flags will be set to true based on the operating system
WINDOWS := false
MAC_OS := false
LINUX := false

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
	# TODO - test this
	PORT = $(shell find /dev -name 'ttyS[0-9]*' | sort | head -n1)
endif

# If automatic port detection fails,
# uncomment one of these lines and change it to set the port manually
# PORT = COM3						# Windows
# PORT = /dev/tty.usbmodem00208212	# macOS
# PORT = /dev/ttyS3					# Linux


# Special commands
.PHONY: all clean debug help lib-common read-eeprom upload

# SRC - defined in the example-specific makefile
# All .c files in src map to .o files
OBJ = $(SRC:../../src/%.c=./%.o)

all: $(PROG)

# Make program
$(PROG): $(PROG).o $(OBJ)
	$(CC) $(CFLAGS) -o $@.elf $^ $(LIB)
	avr-objcopy -j .text -j .data -O ihex $@.elf $@.hex

# .o files depend on .c files
$(PROG).o: $(PROG).c
	$(CC) $(CFLAGS) -c $(PROG).c $(INCLUDES)

./%.o: ../../src/%.c
	$(CC) $(CFLAGS) -o $@ -c $< $(INCLUDES)

clean:
	rm -f ./*.o
	rm -f ./*.elf
	rm -f ./*.hex

# Print debug information
debug:
	@echo ------------
	@echo $(SRC)
	@echo ------------
	@echo $(OBJ)
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
	make -C ../../lib-common clean
	make -C ../../lib-common

# Create a file called eeprom.bin, which contains a raw binary copy of the micro's EEPROM memory.
# View the contents of the binary file in hex
read-eeprom:
	@echo "Reading EEPROM to binary file eeprom.bin..."
	avrdude -p m32m1 -c stk500 -P $(PORT) -U eeprom:r:eeprom.bin:r
	@echo "Displaying eeprom.bin in hex..."
ifeq ($(WINDOWS), true)
	powershell Format-Hex eeprom.bin
else
	hexdump eeprom.bin
endif

upload: $(PROG)
	avrdude -p $(MCU) -c $(PGMR) -P $(PORT) -U flash:w:./$^.hex
