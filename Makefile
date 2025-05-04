#--------------------------------------Makefile-------------------------------------

# Directories
SRC_DIR = src
ARCH_DIR = $(SRC_DIR)/arch
DRIVERS_DIR = $(SRC_DIR)/drivers
UART_DIR = $(DRIVERS_DIR)/uart
VIDEO_DIR = $(SRC_DIR)/video
CMD_DIR = $(SRC_DIR)/cmd
BUILD_DIR = build

# Files
COMMON_CFILES = $(filter-out $(SRC_DIR)/background.c,$(wildcard $(SRC_DIR)/*.c)) 
COMMON_OFILES = $(COMMON_CFILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
VIDEO_CFILES = $(wildcard $(VIDEO_DIR)/*.c)
VIDEO_OFILES = $(VIDEO_CFILES:$(VIDEO_DIR)/%.c=$(BUILD_DIR)/%.o)

# Compiler and flags
GCCFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -Iinclude
IMAGE = $(BUILD_DIR)/kernel8.img

# Detect operating system
ifeq ($(OS),Windows_NT)
    # Windows
    RM = del /S /Q
    RM_PATH = $(BUILD_DIR)\kernel8.elf $(BUILD_DIR)\*.o $(BUILD_DIR)\*.img
    MKDIR = if not exist $(BUILD_DIR) mkdir $(BUILD_DIR) & if not exist $(BUILD_DIR)\video mkdir $(BUILD_DIR)\video
else
    # macOS/Linux
    RM = rm -f
    RM_PATH = $(BUILD_DIR)/kernel8.elf $(BUILD_DIR)/*.o $(BUILD_DIR)/**/*.o $(BUILD_DIR)/*.img
    MKDIR = mkdir -p $(BUILD_DIR)/video
endif

#//////////////////////////////////////////////////////////////
#                                TARGETS
#//////////////////////////////////////////////////////////////

# Build all files
all: clean uart1_build build_core build_video $(IMAGE)_video run1

# Build basic OS without video
basic: clean uart0_build build_core $(IMAGE) run0

# Build OS including video
video: clean uart1_build build_core build_video $(IMAGE)_video run1

# all: clean uart1_build build_core $(IMAGE) run1
# video: clean uart1_build build_video $(IMAGE) run1

# # UART targets
# uart1: clean uart1_build $(IMAGE) run1
# uart0: clean uart0_build $(IMAGE) run0


#//////////////////////////////////////////////////////////////
#                             BUILD STEPS
#//////////////////////////////////////////////////////////////

# Build UART for channel 1
uart1_build: | $(BUILD_DIR)
	aarch64-none-elf-gcc $(GCCFLAGS) -c $(UART_DIR)/uart1.c -o $(BUILD_DIR)/uart.o

# Build UART for channel 0
uart0_build: | $(BUILD_DIR)
	aarch64-none-elf-gcc $(GCCFLAGS) -c $(UART_DIR)/uart0.c -o $(BUILD_DIR)/uart.o

# Build core components
build_core: $(BUILD_DIR)/boot.o $(BUILD_DIR)/mbox.o $(BUILD_DIR)/framebf.o $(COMMON_OFILES)

# Build video components
build_video: $(VIDEO_OFILES)
	aarch64-none-elf-gcc $(GCCFLAGS) -c $(SRC_DIR)/kernel.c -o $(BUILD_DIR)/kernel.o
	
# Build boot assembly
$(BUILD_DIR)/boot.o: $(ARCH_DIR)/boot.S | $(BUILD_DIR)
	aarch64-none-elf-gcc $(GCCFLAGS) -c $< -o $@

# Build mbox
$(BUILD_DIR)/mbox.o: $(SRC_DIR)/drivers/mbox.c | $(BUILD_DIR)
	aarch64-none-elf-gcc $(GCCFLAGS) -c $< -o $@

# Build framebf
$(BUILD_DIR)/framebf.o: $(SRC_DIR)/drivers/framebf.c | $(BUILD_DIR)
	aarch64-none-elf-gcc $(GCCFLAGS) -c $< -o $@

# Build common C files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	aarch64-none-elf-gcc $(GCCFLAGS) -c $< -o $@

# Build video C files
$(BUILD_DIR)/%.o: $(VIDEO_DIR)/%.c | $(BUILD_DIR)
	aarch64-none-elf-gcc $(GCCFLAGS) -c $< -o $@
$(BUILD_DIR)/%.o: $(CMD_DIR)/%.c | $(BUILD_DIR)
	aarch64-none-elf-gcc $(GCCFLAGS) -c $< -o $@

# Link and generate kernel image (without video)
$(IMAGE): $(BUILD_DIR)/boot.o $(BUILD_DIR)/uart.o $(BUILD_DIR)/mbox.o $(BUILD_DIR)/framebf.o $(COMMON_OFILES) $(BUILD_DIR)/cmd.o | $(BUILD_DIR)
	aarch64-none-elf-ld -nostdlib $(BUILD_DIR)/boot.o $(BUILD_DIR)/mbox.o $(BUILD_DIR)/framebf.o $(BUILD_DIR)/uart.o $(COMMON_OFILES) $(BUILD_DIR)/cmd.o -T $(ARCH_DIR)/link.ld -o $(BUILD_DIR)/kernel8.elf
	aarch64-none-elf-objcopy -O binary $(BUILD_DIR)/kernel8.elf $(IMAGE)

# Link and generate kernel image (with video)
$(IMAGE)_video: $(BUILD_DIR)/boot.o $(BUILD_DIR)/uart.o $(BUILD_DIR)/mbox.o $(BUILD_DIR)/framebf.o $(COMMON_OFILES) $(VIDEO_OFILES) | $(BUILD_DIR)
	aarch64-none-elf-ld -nostdlib $(BUILD_DIR)/boot.o $(BUILD_DIR)/mbox.o $(BUILD_DIR)/framebf.o $(BUILD_DIR)/uart.o $(COMMON_OFILES) $(VIDEO_OFILES) -T $(ARCH_DIR)/link.ld -o $(BUILD_DIR)/kernel8.elf
	aarch64-none-elf-objcopy -O binary $(BUILD_DIR)/kernel8.elf $(IMAGE)

# Ensure build directory exists
$(BUILD_DIR):
	$(MKDIR)

# Clean build artifacts
clean:
	$(RM) $(RM_PATH)

#//////////////////////////////////////////////////////////////
#                             RUN TARGETS
#//////////////////////////////////////////////////////////////
# Run emulation with QEMU (UART1)
run1: 
	qemu-system-aarch64 -M raspi3b -kernel $(IMAGE) -serial null -serial stdio

# Run emulation with QEMU (UART0)
run0: 
	qemu-system-aarch64 -M raspi3b -kernel $(IMAGE) -serial stdio