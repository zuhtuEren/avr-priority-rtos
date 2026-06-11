MCU = atmega328p
F_CPU = 16000000UL
CC = avr-gcc
OBJCOPY = avr-objcopy
CFLAGS = -Os -DF_CPU=$(F_CPU) -mmcu=$(MCU)
PORT = /dev/ttyUSB0

# Project source files
SRCS = main.c driver.c timer0.c task.c

# Auto-generate object file names
OBJS = $(SRCS:.c=.o)

all: main.hex

# Link object files into final ELF executable
main.elf: $(OBJS)
	$(CC) $(CFLAGS) -o main.elf $(OBJS)

# Convert ELF to Intel HEX format for flashing
main.hex: main.elf
	$(OBJCOPY) -O ihex -R .eeprom main.elf main.hex

# Flash the microcontroller
upload: main.hex
	avrdude -F -V -c arduino -p $(MCU) -P $(PORT) -b 115200 -U flash:w:main.hex:i

# Clean build files
clean:
	rm -f *.o *.elf *.hex