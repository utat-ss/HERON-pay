# This makefile should go in the root of a repository (except lib-common)

# Reference: http://www.atmel.com/webdoc/avrlibcreferencemanual/group__demo__project_1demo_project_compile.html


# Parameters that might need to be changed, depending on the repository
#-------------------------------------------------------------------------------
# Libraries from lib-common to link
LIB = -L./lib-common/lib -ladc -lcan -ldac -lpex -lqueue -lspi -ltimer -luart -lutilities
# Program name
PROG = pay
#-------------------------------------------------------------------------------


# AVR-GCC compiler
CC = avr-gcc
# Compiler flags
CFLAGS = -Wall -std=gnu99 -g -mmcu=atmega32m1 -Os -mcall-prologues
# Includes (header files)
INCLUDES = -I./lib-common/include/
# Programmer
PGMR = stk500
# Microcontroller
MCU = m32m1
# Build directory
BUILD = build
# Manual tests directory
MANUAL_TESTS = $(dir $(wildcard manual_tests/*/.))


# PORT - Computer port that the programmer is connected to
# Operating system detection based on https://gist.github.com/sighingnow/deee806603ec9274fd47

# Windows
ifeq ($(OS),Windows_NT)
	PORT = $(shell powershell "[System.IO.Ports.SerialPort]::getportnames() | select -First 2 | select -Last 1")

# Unix
else
	# Get the operating system
	UNAME_S := $(shell uname -s)

	# macOS
	ifeq ($(UNAME_S),Darwin)
		# Automatically find the port (lower number)
		PORT = $(shell find /dev -name 'tty.usbmodem[0-9]*' | sort | head -n1)
	endif
	# Linux
	ifeq ($(UNAME_S),Linux)
		PORT = $(shell find /dev -name 'ttyS[0-9]*' | sort | head -n1)
	endif
endif

# If automatic port detection fails,
# uncomment one of these lines and change it to set the port manually
# PORT = COM3						# Windows
# PORT = /dev/tty.usbmodem00208212	# macOS
# PORT = /dev/ttyS3					# Linux


# Special commands
.PHONY: all clean debug help lib-common manual_tests upload

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
	@echo ————————————
	@echo $(SRC)
	@echo ————————————
	@echo $(OBJ)
	@echo ————————————

# Help shows available commands
help:
	@echo "usage: make [all | clean | debug | help | lib-common | manual_tests | upload]"
	@echo "Running make without any arguments is equivalent to running make all."
	@echo "all            build the main program (src directory)"
	@echo "clean          clear the build directory and all subdirectories"
	@echo "debug          display debugging information"
	@echo "help           display this help message"
	@echo "lib-common     fetch and build the latest version of lib-common"
	@echo "manual_tests   build all manual test programs (manual_tests directory)"
	@echo "upload         upload the main program to a board"

# Update and build lib-common
lib-common:
	@echo "Fetching latest version of lib-common..."
	git submodule update --remote
	@echo "Building lib-common..."
	make -C lib-common clean
	make -C lib-common

manual_tests:
	@for dir in $(MANUAL_TESTS) ; do \
		cd $$dir ; \
		make clean ; \
		make ; \
		cd ../.. ; \
	done

# Upload program to board
upload: $(PROG)
	avrdude -c $(PGMR) -p $(MCU) -P $(PORT) -U flash:w:./build/$^.hex
