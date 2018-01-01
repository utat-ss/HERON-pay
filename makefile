# reference: http://www.atmel.com/webdoc/avrlibcreferencemanual/group__demo__project_1demo_project_compile.html

# Don't change these
CC = avr-gcc
CFLAGS = -g -mmcu=atmega32m1 -Os -mcall-prologues
PROG = stk500
MCU = m32m1
INCLUDES = -I./lib-common/include/

# Change this line depending on what you're using
LIB = -L./lib-common/lib -luart -lspi -lcan -ltimer -lqueue

# Change this line based on your OS and port
PORT = /dev/tty.usbmodem00187462


SRC_FILES = $(MAIN)

OBJS := $(SRC_FILES:.c=.o)
OBJS := $(OBJS:%=./build/%)

ELF := ./build/$(PROG_NAME).elf
HEX  := ./build/$(PROG_NAME).hex

# makes it look for .c files in src without having src in path
vpath %.c src

all: $(HEX)

$(HEX): $(ELF)
	avr-objcopy -j .text -j .data -O ihex $(ELF) $(HEX)

$(ELF): $(OBJS)
	$(CC) $(CFLAGS) -o $(ELF) $(OBJS) $(LIB)

./build/%.o: %.c ./build
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
