CC = avr-gcc
INCLUDES = -I../../lib-common/include
LIB = -L../../lib-common/lib -luart -lspi -lcan
CFLAGS = -Wall -g -mmcu=atmega32m1 -Os -mcall-prologues

PGMR = stk500
MCU = m32m1

PORT = COM8
#PORT = /dev/tty.usbmodem00208212

# SRC is defined in the example-specific makefile
OBJ = $(SRC:../../src%.c=..%.o)

$(PROG): $(PROG).o $(OBJ)
	$(CC) $(CFLAGS) -o $@.elf $^ $(LIB)
	avr-objcopy -j .text -j .data -O ihex $@.elf $@.hex

$(PROG).o: $(PROG).c
	$(CC) $(CFLAGS) -c $(PROG).c $(INCLUDES)

../%.o: ../../src/%.c
	$(CC) $(CFLAGS) -o $@ -c $< $(INCLUDES)

.PHONY: clean upload

clean:
	rm -f ./*.o
	rm -f ../*.o
	rm -f ./*.elf
	rm -f ./*.hex

upload: $(PROG)
	avrdude -p $(MCU) -c $(PGMR) -P $(PORT) -U flash:w:./$^.hex
