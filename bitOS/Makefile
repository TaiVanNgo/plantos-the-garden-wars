#--------------------------------------Makefile-------------------------------------

# Directories
SRC_DIR = src
ARCH_DIR = $(SRC_DIR)/arch
UART_DIR = uart
BUILD_DIR = build

# Files
CFILES = $(wildcard $(SRC_DIR)/*.c)
OFILES = $(CFILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
GCCFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -Iinclude
IMAGE = $(BUILD_DIR)/kernel8.img

# Detect operating system
ifeq ($(OS),Windows_NT)
    # Windows
    RM = del /Q
    RM_PATH = $(BUILD_DIR)\kernel8.elf $(BUILD_DIR)\*.o $(BUILD_DIR)\*.img
    MKDIR = if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
else
    # macOS/Linux
    RM = rm -f
    RM_PATH = $(BUILD_DIR)/kernel8.elf $(BUILD_DIR)/*.o $(BUILD_DIR)/*.img
    MKDIR = mkdir -p $(BUILD_DIR)
endif

# Default target
all: clean uart1_build $(IMAGE) run1

# UART targets
uart1: clean uart1_build $(IMAGE) run1
uart0: clean uart0_build $(IMAGE) run0

# Build UART for channel 1
uart1_build: $(UART_DIR)/uart1.c | $(BUILD_DIR)
	aarch64-none-elf-gcc $(GCCFLAGS) -c $(UART_DIR)/uart1.c -o $(BUILD_DIR)/uart.o

# Build UART for channel 0
uart0_build: $(UART_DIR)/uart0.c | $(BUILD_DIR)
	aarch64-none-elf-gcc $(GCCFLAGS) -c $(UART_DIR)/uart0.c -o $(BUILD_DIR)/uart.o

# Build boot assembly
$(BUILD_DIR)/boot.o: $(ARCH_DIR)/boot.S | $(BUILD_DIR)
	aarch64-none-elf-gcc $(GCCFLAGS) -c $(ARCH_DIR)/boot.S -o $(BUILD_DIR)/boot.o

# Build C files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	aarch64-none-elf-gcc $(GCCFLAGS) -c $< -o $@

# Link and generate kernel image
$(IMAGE): $(BUILD_DIR)/boot.o $(BUILD_DIR)/uart.o $(OFILES) | $(BUILD_DIR)
	aarch64-none-elf-ld -nostdlib $(BUILD_DIR)/boot.o $(BUILD_DIR)/uart.o $(OFILES) -T $(ARCH_DIR)/link.ld -o $(BUILD_DIR)/kernel8.elf
	aarch64-none-elf-objcopy -O binary $(BUILD_DIR)/kernel8.elf $(IMAGE)

# Ensure build directory exists
$(BUILD_DIR):
	$(MKDIR)

# Clean build artifacts
clean:
	$(RM) $(RM_PATH)

# Run emulation with QEMU (UART1)
run1: 
	qemu-system-aarch64 -M raspi3b -kernel $(IMAGE) -serial null -serial stdio

# Run emulation with QEMU (UART0)
run0: 
	qemu-system-aarch64 -M raspi3b -kernel $(IMAGE) -serial stdio