# reference: http://www.atmel.com/webdoc/avrlibcreferencemanual/group__demo__project_1demo_project_compile.html

# Don't change these
CC = avr-gcc
CFLAGS = -std=gnu99 -g -mmcu=atmega32m1 -Os -mcall-prologues
PROG = stk500
MCU = m32m1
INCLUDES = -I./lib-common/include/

# Change this line depending on what you're using
LIB = -L./lib-common/lib -luart -lspi -lcan -ltimer -lqueue -lpex

# Change this line based on your OS and port
ifeq ($(OS),Windows_NT)
	PORT = COM3
else
	# Automatically find the port on macOS (lower number)
	PORT = $(shell find /dev -name 'tty.usbmodem[0-9]*' | sort | head -n1)
	#PORT = /dev/tty.usbmodem00187462
	#PORT = /dev/tty.usbmodem00208212
endif

SRC = $(wildcard ./src/*.c)
OBJ = $(SRC:./src/%.c=./build/%.o)
DEP = $(OBJ:.o=.d)

# Build directory
DIR = build

pay: $(OBJ)
	$(CC) $(CFLAGS) -o ./build/$@.elf $(OBJ) $(LIB)
	avr-objcopy -j .text -j .data -O ihex ./build/$@.elf ./build/$@.hex

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

upload: pay
	avrdude -c $(PROG) -p $(MCU) -P $(PORT) -U flash:w:./build/$^.hex

debug:
	@echo ————————————
	@echo $(SRC)
	@echo ————————————
	@echo $(OBJ)
	@echo ————————————

# Update and make lib-common
lib-common:
	git submodule update --remote
	cd lib-common
	make clean
	make
	cd ..

help:
	@echo "usage: make [clean | upload | debug | lib-common | help]"
