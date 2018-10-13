# Reference: http://www.atmel.com/webdoc/avrlibcreferencemanual/group__demo__project_1demo_project_compile.html

# Don't change these
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
DIR = build
# Program name
PROG = pay

# Libraries from lib-common to link
# May need to change this line
LIB = -L./lib-common/lib -luart -lspi -lcan -ltimer -lqueue -lpex -ldac -ladc

# Computer port that the programmer is connected to
# May need to change the PORT definition based on your OS and port
# Operating system detection based on https://gist.github.com/sighingnow/deee806603ec9274fd47

# Windows
ifeq ($(OS),Windows_NT)
	PORT = COM3
	# PORT = COM5

else
	# If on Unix, get the operating system
	UNAME_S := $(shell uname -s)

	# Linux
	ifeq ($(UNAME_S),Linux)
		PORT = /dev/ttyS3
		# PORT = /dev/ttyS5
	endif

	# macOS
	ifeq ($(UNAME_S),Darwin)
		# Automatically find the port (lower number)
		PORT = $(shell find /dev -name 'tty.usbmodem[0-9]*' | sort | head -n1)

		# Manual ports
		# PORT = /dev/tty.usbmodem00187462
		# PORT = /dev/tty.usbmodem00208212
	endif
endif

# Get all .c files in src folder
SRC = $(wildcard ./src/*.c)
# All .c files in src map to .o files in build
OBJ = $(SRC:./src/%.c=./build/%.o)
DEP = $(OBJ:.o=.d)


# Make program
$(PROG): $(OBJ)
	$(CC) $(CFLAGS) -o ./build/$@.elf $(OBJ) $(LIB)
	avr-objcopy -j .text -j .data -O ihex ./build/$@.elf ./build/$@.hex

# .o files depend on .c files
./build/%.o: ./src/%.c
	$(CC) $(CFLAGS) -o $@ -c $< $(INCLUDES)

-include $(DEP)

./build/%.d: ./src/%.c | $(DIR)
	@$(CC) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

# Create the build directory if it doesn't exist
$(DIR):
	mkdir $(DIR)


# Special commands
.PHONY: clean upload debug lib-common help

# Remove all files in the build directory
clean:
	rm -f $(DIR)/*

# Upload program to board
upload: $(PROG)
	avrdude -c $(PGMR) -p $(MCU) -P $(PORT) -U flash:w:./build/$^.hex

# Print debug information
debug:
	@echo ————————————
	@echo $(SRC)
	@echo ————————————
	@echo $(OBJ)
	@echo ————————————

# Update and make lib-common
lib-common:
	@echo "Fetching latest version of lib-common..."
	git submodule update --remote
	@echo "Compiling lib-common..."
	make -C lib-common clean
	make -C lib-common

# Help shows available commands
help:
	@echo "usage: make [clean | upload | debug | lib-common | help]"
