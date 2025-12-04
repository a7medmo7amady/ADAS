# Compiler settings
MCU = atmega32
CC = avr-gcc
CFLAGS = -mmcu=$(MCU) -Os
OBJCPY = avr-objcopy
AVRDUDE = avrdude
USBASP = usbasp

# Directories
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj

# Files
SRC = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/HAL/*.c) $(wildcard $(SRC_DIR)/MCAL/*.c) $(SRC_DIR)/APP/main.c
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Targets
all: out.hex

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

out.elf: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

out.hex: out.elf
	$(OBJCPY) -O ihex out.elf out.hex

flash: out.hex
	$(AVRDUDE) -c $(USBASP) -p m32 -U flash:w:out.hex:i

clean:
	rm -rf $(OBJ_DIR) out.elf out.hex
