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
SRC = $(wildcard $(SRC_DIR)/APP/*.c) \
      $(wildcard $(SRC_DIR)/HAL/*.c) \
      $(wildcard $(SRC_DIR)/MCAL/*.c)

OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Targets
all: out.hex

# Rule to compile .c to .o files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)/APP $(OBJ_DIR)/HAL $(OBJ_DIR)/MCAL  # Create necessary obj directories
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

# Link object files to create ELF file
out.elf: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Convert ELF file to HEX file
out.hex: out.elf
	$(OBJCPY) -O ihex out.elf out.hex

# Flash the HEX file to the device
flash: out.hex
	$(AVRDUDE) -c $(USBASP) -p m32 -U flash:w:out.hex:i

# Clean up all generated files
clean:
	rm -rf $(OBJ_DIR) out.elf out.hex
