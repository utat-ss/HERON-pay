# Reference: http://www.atmel.com/webdoc/avrlibcreferencemanual/group__demo__project_1demo_project_compile.html

# Don't change these
# AVR-GCC compiler
CC = avr-gcc
# Compiler flags
CFLAGS = -Wall -std=gnu99 -g -mmcu=atmega32m1 -Os -mcall-prologues
# Includes (header files)
INCLUDES = -I../../lib-common/include
# Programmer
PGMR = stk500
# Microcontroller
MCU = m32m1

# Libraries from lib-common to link
# May need to change this line
LIB = -L../../lib-common/lib -luart -lspi -lcan -lpex -lconversions

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

# SRC - defined in the example-specific makefile
# All .c files in src map to .o files
OBJ = $(SRC:../../src/%.c=./%.o)

# Make program
$(PROG): $(PROG).o $(OBJ)
	$(CC) $(CFLAGS) -o $@.elf $^ $(LIB)
	avr-objcopy -j .text -j .data -O ihex $@.elf $@.hex

# .o files depend on .c files
$(PROG).o: $(PROG).c
	$(CC) $(CFLAGS) -c $(PROG).c $(INCLUDES)

./%.o: ../../src/%.c
	$(CC) $(CFLAGS) -o $@ -c $< $(INCLUDES)


# Special commands
.PHONY: clean upload debug lib-common help

clean:
	rm -f ./*.o
	rm -f ./*.elf
	rm -f ./*.hex

upload: $(PROG)
	avrdude -p $(MCU) -c $(PGMR) -P $(PORT) -U flash:w:./$^.hex

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
	make -C ../../lib-common clean
	make -C ../../lib-common

help:
	@echo "usage: make [clean | upload | debug | lib-common | help]"
