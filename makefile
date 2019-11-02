# This makefile should go in the root of a repository (except lib-common)

# Reference: http://www.atmel.com/webdoc/avrlibcreferencemanual/group__demo__project_1demo_project_compile.html


# Parameters that might need to be changed, depending on the repository
#-------------------------------------------------------------------------------
# Libraries from lib-common to link
# For some reason, conversions needs to come after dac or else it gives an error
# Need to put dac before conversions, uptime before timer, heartbeat before can,
# or else gives an error for undefined reference
LIB = -L./lib-common/lib -ladc -lheartbeat -lcan -ldac -lconversions -lpex -lqueue -lspi -luptime -ltimer -luart -lutilities -lwatchdog
# Program name
PROG = pay
# Name of microcontroller ("32m1" or "64m1")
MCU = 32m1
#-------------------------------------------------------------------------------


# AVR-GCC compiler
CC = avr-gcc
# Compiler flags
CFLAGS = -Wall -std=gnu99 -g -mmcu=atmega$(MCU) -Os -mcall-prologues
# Includes (header files)
INCLUDES = -I./lib-common/include/
# Programmer
PGMR = stk500
# avrdude device
DEVICE = m$(MCU)
# Build directory
BUILD = build
# Manual tests directory
MANUAL_TESTS = $(dir $(wildcard manual_tests/*/.))
# Harness testing folder
TEST = harness_tests
# HARNESS_ARGS - can specify from the command line when calling `make`

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
 	UART = $(shell powershell "[System.IO.Ports.SerialPort]::getportnames() | sort | select -First 1")
endif
ifeq ($(MAC_OS), true)
	# lower number
	PORT = $(shell find /dev -name 'tty.usbmodem[0-9]*' | sort | head -n1)
	UART = $(shell find /dev -name 'tty.usbmodem[0-9]*' | sort | sed -n 2p)
endif
ifeq ($(LINUX), true)
	# lower number
	PORT = $(shell pavr2cmd --prog-port)
	UART = $(shell pavr2cmd --ttl-port)
	ifeq ($(shell whoami),ss)
		REMOTE := true
	endif
endif

# If automatic port detection fails,
# uncomment one of these lines and change it to set the port manually
# PORT = COM3						# Windows
# PORT = /dev/tty.usbmodem00208212	# macOS
# PORT = /dev/ttyS3					# Linux


# Set the PYTHON variable - Python interpreter
# Windows uses `python` for either Python 2 or 3,
# while macOS/Linux use `python3` to explicitly use Python 3
ifeq ($(WINDOWS), true)
	PYTHON := python
endif
ifeq ($(MAC_OS), true)
	PYTHON := python3
endif
ifeq ($(LINUX), true)
	PYTHON := python3
endif


# Special commands
.PHONY: all clean debug harness help lib-common manual_tests read-eeprom upload

# Get all .c files in src folder
SRC = $(wildcard ./src/*.c)
# All .c files in src get compiled to to .o files in build
OBJ = $(SRC:./src/%.c=./build/%.o)
DEP = $(OBJ:.o=.d)

all: $(PROG)

# Make main program
$(PROG): $(OBJ)
	$(CC) $(CFLAGS) -o ./build/$@.elf $(OBJ) $(LIB)
	avr-objcopy -j .text -j .data -O ihex ./build/$@.elf ./build/$@.hex

# .o files depend on .c files
./build/%.o: ./src/%.c
	$(CC) $(CFLAGS) -o $@ -c $< $(INCLUDES)

-include $(DEP)

./build/%.d: ./src/%.c | $(BUILD)
	@$(CC) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

# Create the build directory if it doesn't exist
$(BUILD):
	mkdir $(BUILD)

# Remove all files in the build directory
clean:
	rm -f $(BUILD)/*

# Print debug information
debug:
	@echo ------------
	@echo "MCU:" $(MCU)
	@echo ------------
	@echo "DEVICE:" $(DEVICE)
	@echo ------------
	@echo "TEST:" $(TEST)
	@echo ------------
	@echo "HARNESS_ARGS:" $(HARNESS_ARGS)
	@echo ------------
	@echo "WINDOWS:" $(WINDOWS)
	@echo ------------
	@echo "MAC_OS:" $(MAC_OS)
	@echo ------------
	@echo "LINUX:" $(LINUX)
	@echo ------------
	@echo "PYTHON:" $(PYTHON)
	@echo ------------
	@echo "SRC:" $(SRC)
	@echo ------------
	@echo "OBJ:" $(OBJ)
	@echo ------------

# Need to cd into lib-common and refer back up one directory to the harness_tests folder
# because harness.py has the `include` and `src` paths hardcoded
harness:
	cd lib-common && \
	$(PYTHON) ./bin/harness.py -p $(PORT) -u $(UART) -d ../$(TEST) $(HARNESS_ARGS)

# Help shows available commands
help:
	@echo "usage: make [all | clean | debug | harness | help | lib-common | manual_tests | read-eeprom | upload]"
	@echo "Running make without any arguments is equivalent to running make all."
	@echo "all            build the main program (src directory)"
	@echo "clean          clear the build directory and all subdirectories"
	@echo "debug          display debugging information"
	@echo "harness        run the test harness"
	@echo "help           display this help message"
	@echo "lib-common     fetch and build the latest version of lib-common"
	@echo "manual_tests   build all manual test programs (manual_tests directory)"
	@echo "read-eeprom    read and display the contents of the microcontroller's EEPROM"
	@echo "upload         upload the main program to a board"

lib-common:
	@echo "Fetching latest version of lib-common..."
	git submodule update --remote
	@echo "Building lib-common..."
	make -C lib-common clean MCU=$(MCU)
	make -C lib-common MCU=$(MCU)

manual_tests:
	@for dir in $(MANUAL_TESTS) ; do \
		cd $$dir ; \
		make clean ; \
		make ; \
		cd ../.. ; \
	done

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

# Upload program to board
upload: $(PROG)
ifeq ($(REMOTE),true)
	gpio -g mode 14 IN
	gpio -g mode 15 IN
endif
	avrdude -c $(PGMR) -C ./lib-common/avrdude.conf -p $(DEVICE) -P $(PORT) -U flash:w:./build/$^.hex
ifeq ($(REMOTE),true)
	gpio -g mode 14 ALT0
	gpio -g mode 15 ALT0
endif
