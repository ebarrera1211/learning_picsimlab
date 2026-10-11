MCU = atmega328p
F_CPU = 16000000UL
BAUD = 9600
PORT = /dev/ttyUSB0

CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE = avr-size

CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall -Wextra

TARGET = main

SRC = main.c
OBJ = $(SRC: .c=.o)

all: $(TARGET).hex

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET).elf: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@
	$(SIZE) --format=avr --mcu=$(MCU) $<

clean:
	rm -f $(OBJ) $(TARGET).elf $(TARGET).hex

.PHONY: all clean
