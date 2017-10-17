# reference: http://www.atmel.com/webdoc/avrlibcreferencemanual/group__demo__project_1demo_project_compile.html

# Shouldn't change these
CC = avr-gcc
CFLAGS = -g -mmcu=atmega32m1
PROG = stk500
PORT = /dev/tty.usbmodem00187462
MCU = m32m1
program_NAME = pay

# Might want to change these
INCLUDES = -I./lib-common/include/
#LIB = -L./lib-common/lib -luart -lspi -lcan -ltimer -lqueue
LIB = -L./lib-common/lib -luart -lspi -lcan

#CAN := can_test.c
PEX := pex.c
ADC := adc.c
SENSORS := pex.c sensors.c
MAIN := main.c

SRC_FILES = $(ADC) $(SENSORS) $(MAIN)

OBJS := $(SRC_FILES:.c=.o)
OBJS := $(OBJS:%=./build/%)

ELF := ./build/$(program_NAME).elf
HEX  := ./build/$(program_NAME).hex

# makes it look for .c files in src without having src in path
vpath %.c src

all: $(HEX)

$(HEX): $(ELF)
	avr-objcopy -j .text -j .data -O ihex $(ELF) $(HEX)

$(ELF): $(OBJS)
	$(CC) $(CFLAGS) -o $(ELF) $(OBJS) $(LIB)

./build/%.o: %.c
	$(CC) $(CFLAGS) -Os -c $< $(INCLUDES)
	@mv $(@F) $@


.PHONY: clean upload debug

clean:
	rm ./build/*

upload: all
	avrdude -c $(PROG) -p $(MCU) -P $(PORT) -U flash:w:$(HEX)

debug:
	@echo ————————————
	@echo $(SRC_FILES)
	@echo ————————————
	@echo $(OBJS)
	@echo ————————————
