# reference: http://www.atmel.com/webdoc/avrlibcreferencemanual/group__demo__project_1demo_project_compile.html

CC = avr-gcc
CFLAGS = -g -mmcu=atmega32m1
PROG = stk500
PORT = /dev/tty.usbmodem00187462
MCU = m32m1
program_NAME = pay
INCLUDES = -Ilib-common -Isrc

CAN := can_drv.c can_lib.c can_test.c
UART := uart.c log.c
SPI := spi.c
PEX := pex.c

SRC_FILES = $(SPI) $(UART) pressure_sensor.c

OBJS := $(SRC_FILES:.c=.o)
OBJS := $(OBJS:%=./build/%)

ELF := ./build/$(program_NAME).elf
HEX  := ./build/$(program_NAME).hex

# first look for .c files in lib-common and then in src
vpath %.c lib-common
vpath %.c src

all: $(HEX)

$(HEX): $(ELF)
	avr-objcopy -j .text -j .data -O ihex $(ELF) $(HEX)

$(ELF): $(OBJS)
	$(CC) $(CFLAGS) -o $(ELF) $(OBJS)

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