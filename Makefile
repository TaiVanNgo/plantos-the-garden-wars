#--------------------------------------Makefile-------------------------------------

# Directories
SRC_DIR = src
ARCH_DIR = $(SRC_DIR)/arch
DRIVERS_DIR = $(SRC_DIR)/drivers
UART_DIR = $(DRIVERS_DIR)/uart
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

# Variables to track which UART is used
UART_SOURCE = 

# Build UART for channel 1
uart1_build: | $(BUILD_DIR)
	$(eval UART_SOURCE = $(UART_DIR)/uart1.c)
	aarch64-none-elf-gcc $(GCCFLAGS) -c $(UART_SOURCE) -o $(BUILD_DIR)/uart.o

# Build UART for channel 0
uart0_build: | $(BUILD_DIR)
	$(eval UART_SOURCE = $(UART_DIR)/uart0.c)
	aarch64-none-elf-gcc $(GCCFLAGS) -c $(UART_SOURCE) -o $(BUILD_DIR)/uart.o

# Build boot assembly
$(BUILD_DIR)/boot.o: $(ARCH_DIR)/boot.S | $(BUILD_DIR)
	aarch64-none-elf-gcc $(GCCFLAGS) -c $< -o $@

# Build mbox
$(BUILD_DIR)/mbox.o: $(SRC_DIR)/drivers/mbox.c | $(BUILD_DIR)
	aarch64-none-elf-gcc $(GCCFLAGS) -c $< -o $@

# Build framebf
$(BUILD_DIR)/framebf.o: $(SRC_DIR)/drivers/framebf.c | $(BUILD_DIR)
	aarch64-none-elf-gcc $(GCCFLAGS) -c $< -o $@

# Build C files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	aarch64-none-elf-gcc $(GCCFLAGS) -c $< -o $@

# Link and generate kernel image
$(IMAGE): $(BUILD_DIR)/boot.o $(BUILD_DIR)/uart.o $(BUILD_DIR)/mbox.o $(BUILD_DIR)/framebf.o $(OFILES) | $(BUILD_DIR)
	aarch64-none-elf-ld -nostdlib $(BUILD_DIR)/boot.o $(BUILD_DIR)/mbox.o  $(BUILD_DIR)/framebf.o $(BUILD_DIR)/uart.o $(OFILES) -T $(ARCH_DIR)/link.ld -o $(BUILD_DIR)/kernel8.elf
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