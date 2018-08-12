CC = avr-gcc
INCLUDES = -I../../lib-common/include
LIB = -L../../lib-common/lib -luart -lspi -lcan -lpex -lconversions
CFLAGS = -Wall -g -mmcu=atmega32m1 -Os -mcall-prologues

PGMR = stk500
MCU = m32m1

# Change this line based on your OS and port
ifeq ($(OS),Windows_NT)
	PORT = COM3
else
	# Automatically find the port on macOS (lower number)
	PORT = $(shell find /dev -name 'tty.usbmodem[0-9]*' | sort | head -n1)
	#PORT = /dev/tty.usbmodem00187462
	#PORT = /dev/tty.usbmodem00208212
endif

# SRC is defined in the example-specific makefile
OBJ = $(SRC:../../src/%.c=./%.o)

$(PROG): $(PROG).o $(OBJ)
	$(CC) $(CFLAGS) -o $@.elf $^ $(LIB)
	avr-objcopy -j .text -j .data -O ihex $@.elf $@.hex

$(PROG).o: $(PROG).c
	$(CC) $(CFLAGS) -c $(PROG).c $(INCLUDES)

./%.o: ../../src/%.c
	$(CC) $(CFLAGS) -o $@ -c $< $(INCLUDES)


# Special commands
.PHONY: clean upload help

clean:
	rm -f ./*.o
	rm -f ./*.elf
	rm -f ./*.hex

upload: $(PROG)
	avrdude -p $(MCU) -c $(PGMR) -P $(PORT) -U flash:w:./$^.hex

help:
	@echo "usage: make [clean | upload | help]"
