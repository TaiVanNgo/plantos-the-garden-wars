#--------------------------------------Makefile-------------------------------------

# Directories
SRC_DIR = src
ARCH_DIR = $(SRC_DIR)/arch
DRIVERS_DIR = $(SRC_DIR)/drivers
UART_DIR = $(DRIVERS_DIR)/uart
VIDEO_DIR = $(SRC_DIR)/video
CMD_DIR = $(SRC_DIR)/cmd
BUILD_DIR = build
GAMES_DIR = $(SRC_DIR)/games

# Files
COMMON_CFILES = $(wildcard $(SRC_DIR)/*.c) 
COMMON_OFILES = $(COMMON_CFILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
VIDEO_CFILES = $(wildcard $(VIDEO_DIR)/*.c)
VIDEO_OFILES = $(VIDEO_CFILES:$(VIDEO_DIR)/%.c=$(BUILD_DIR)/%.o)
ASSETS_CFILES = $(wildcard assets/*/*.c) $(wildcard assets/*/*/*.c)
ASSETS_OFILES = $(patsubst assets/%.c,$(BUILD_DIR)/%.o,$(ASSETS_CFILES))
GAMES_CFILES = $(wildcard $(GAMES_DIR)/*.c)
GAMES_OFILES = $(GAMES_CFILES:$(GAMES_DIR)/%.c=$(BUILD_DIR)/games/%.o)

# Compiler and flags
GCCFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -Iinclude
IMAGE = $(BUILD_DIR)/kernel8.img

# Detect operating system
ifeq ($(OS),Windows_NT)
    # Windows
    RM = del /S /Q
    RM_PATH = $(BUILD_DIR)\kernel8.elf $(BUILD_DIR)\*.o $(BUILD_DIR)\*.img
    MKDIR = if not exist $(subst /,\\,$(dir $@)) mkdir $(subst /,\\,$(dir $@))
else
    # macOS/Linux
    RM = rm -f
    RM_PATH = $(BUILD_DIR)/kernel8.elf $(BUILD_DIR)/*.o $(BUILD_DIR)/**/*.o $(BUILD_DIR)/**/**/*.o $(BUILD_DIR)/*.img
    MKDIR = mkdir -p $(dir $@)
endif

#//////////////////////////////////////////////////////////////
#                                TARGETS
#//////////////////////////////////////////////////////////////

# Build all files
all: clean uart0_build build_core_all build_video $(IMAGE)_video run0

# Build basic OS without video
basic: clean uart0_build build_core build_game $(IMAGE) run0

# Build game mode
game: clean uart0_build build_core_zombie build_game $(IMAGE)_game run0

# Build video mode
video: clean uart0_build build_core_vid build_video $(IMAGE)_video run0

# Build CLI mode
cli: clean uart0_build build_core_cli build_video $(IMAGE)_video run0

# Build TEAM mode
team: clean uart0_build build_core_team build_video $(IMAGE)_video run0

# Build ALL mode (video + team + CLI)
all_mode: clean uart0_build build_core_all build_video $(IMAGE)_video run0

# # UART targets
# uart1: clean uart1_build $(IMAGE) run1
# uart0: clean uart0_build $(IMAGE) run0


#//////////////////////////////////////////////////////////////
#                             BUILD STEPS
#//////////////////////////////////////////////////////////////

# Build UART for channel 1
uart1_build: | $(BUILD_DIR)
	@echo "[BUILD] Compiling UART1 driver: $(UART_DIR)/uart1.c -> $(BUILD_DIR)/uart.o"
	aarch64-none-elf-gcc $(GCCFLAGS) -c $(UART_DIR)/uart1.c -o $(BUILD_DIR)/uart.o

# Build UART for channel 0
uart0_build: | $(BUILD_DIR)
	@echo "[BUILD] Compiling UART0 driver: $(UART_DIR)/uart0.c -> $(BUILD_DIR)/uart.o"
	aarch64-none-elf-gcc $(GCCFLAGS) -c $(UART_DIR)/uart0.c -o $(BUILD_DIR)/uart.o

# Build core components
build_core: $(BUILD_DIR)/boot.o $(BUILD_DIR)/mbox.o $(BUILD_DIR)/framebf.o $(COMMON_OFILES)
	@echo "[BUILD] Building core components"

# Build video components
build_video: $(VIDEO_OFILES)
	@echo "[BUILD] Building video components"

	
# Build boot assembly
$(BUILD_DIR)/boot.o: $(ARCH_DIR)/boot.S | $(BUILD_DIR)
	@echo "[BUILD] Compiling boot assembly: $< -> $@"
	aarch64-none-elf-gcc $(GCCFLAGS) -c $< -o $@

# Build mbox
$(BUILD_DIR)/mbox.o: $(SRC_DIR)/drivers/mbox.c | $(BUILD_DIR)
	@echo "[BUILD] Compiling mbox: $< -> $@"
	aarch64-none-elf-gcc $(GCCFLAGS) -c $< -o $@

# Build framebf
$(BUILD_DIR)/framebf.o: $(SRC_DIR)/drivers/framebf.c | $(BUILD_DIR)
	@echo "[BUILD] Compiling framebf: $< -> $@"
	aarch64-none-elf-gcc $(GCCFLAGS) -c $< -o $@

# Build common C files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	@echo "[BUILD] Compiling common source: $< -> $@"
	aarch64-none-elf-gcc $(GCCFLAGS) -c $< -o $@

# Build video C files
$(BUILD_DIR)/%.o: $(VIDEO_DIR)/%.c | $(BUILD_DIR)
	@echo "[BUILD] Compiling video source: $< -> $@"
	aarch64-none-elf-gcc $(GCCFLAGS) -c $< -o $@
$(BUILD_DIR)/%.o: $(CMD_DIR)/%.c | $(BUILD_DIR)
	@echo "[BUILD] Compiling command source: $< -> $@"
	aarch64-none-elf-gcc $(GCCFLAGS) -c $< -o $@

# Build assets C files
$(BUILD_DIR)/%.o: assets/%.c | $(BUILD_DIR)
	@echo "[BUILD] Compiling asset: $< -> $@"
	$(MKDIR)
	aarch64-none-elf-gcc $(GCCFLAGS) -c $< -o $@

# Build game C files
$(BUILD_DIR)/games/%.o: $(SRC_DIR)/games/%.c | $(BUILD_DIR)
	@echo "[BUILD] Compiling game source: $< -> $@"
	$(MKDIR)
	aarch64-none-elf-gcc $(GCCFLAGS) -c $< -o $@
build_game: 
# Build game C files
$(BUILD_DIR)/games/%.o: $(SRC_DIR)/games/%.c | $(BUILD_DIR)
	$(MKDIR)
	aarch64-none-elf-gcc $(GCCFLAGS) -c $< -o $@
# Build core components for game
build_core_zombie: $(BUILD_DIR)/boot.o $(BUILD_DIR)/mbox.o $(BUILD_DIR)/framebf.o $(COMMON_OFILES)
	@echo "[BUILD] Building core components (GAME_INIT)"
	aarch64-none-elf-gcc $(GCCFLAGS) -DGAME_INIT -c $(SRC_DIR)/kernel.c -o $(BUILD_DIR)/kernel.o

# Build core components for video mode (VID)
build_core_vid: $(BUILD_DIR)/boot.o $(BUILD_DIR)/mbox.o $(BUILD_DIR)/framebf.o $(COMMON_OFILES)
	@echo "[BUILD] Building core components (VID)"
	aarch64-none-elf-gcc $(GCCFLAGS) -DVID_INIT -c $(SRC_DIR)/kernel.c -o $(BUILD_DIR)/kernel.o

# Build core components for CLI mode (CLI_INIT)
build_core_cli: $(BUILD_DIR)/boot.o $(BUILD_DIR)/mbox.o $(BUILD_DIR)/framebf.o $(COMMON_OFILES)
	@echo "[BUILD] Building core components (CLI_INIT)"
	aarch64-none-elf-gcc $(GCCFLAGS) -DCLI_INIT -c $(SRC_DIR)/kernel.c -o $(BUILD_DIR)/kernel.o

# Build core components for TEAM mode (TEAM_INIT)
build_core_team: $(BUILD_DIR)/boot.o $(BUILD_DIR)/mbox.o $(BUILD_DIR)/framebf.o $(COMMON_OFILES)
	@echo "[BUILD] Building core components (TEAM_INIT)"
	aarch64-none-elf-gcc $(GCCFLAGS) -DTEAM_INIT -c $(SRC_DIR)/kernel.c -o $(BUILD_DIR)/kernel.o

# Build core components for ALL mode (ALL_INIT)
build_core_all: $(BUILD_DIR)/boot.o $(BUILD_DIR)/mbox.o $(BUILD_DIR)/framebf.o $(COMMON_OFILES)
	@echo "[BUILD] Building core components (ALL_INIT)"
	aarch64-none-elf-gcc $(GCCFLAGS) -DALL_INIT -c $(SRC_DIR)/kernel.c -o $(BUILD_DIR)/kernel.o

# Link and generate kernel image (without video)
$(IMAGE): $(BUILD_DIR)/boot.o $(BUILD_DIR)/uart.o $(BUILD_DIR)/mbox.o $(BUILD_DIR)/framebf.o $(COMMON_OFILES) $(BUILD_DIR)/cmd.o $(ASSETS_OFILES) $(GAMES_OFILES) | $(BUILD_DIR)
	@echo "[LINK] Linking kernel image (no video): $(BUILD_DIR)/kernel8.elf"
	aarch64-none-elf-ld -nostdlib $(BUILD_DIR)/boot.o $(BUILD_DIR)/mbox.o $(BUILD_DIR)/framebf.o $(BUILD_DIR)/uart.o $(COMMON_OFILES) $(BUILD_DIR)/cmd.o $(ASSETS_OFILES) $(GAMES_OFILES) -T $(ARCH_DIR)/link.ld -o $(BUILD_DIR)/kernel8.elf
	@echo "[OBJCOPY] Creating binary image: $(IMAGE)"
	aarch64-none-elf-objcopy -O binary $(BUILD_DIR)/kernel8.elf $(IMAGE)

# Link and generate kernel image (with video)
$(IMAGE)_video: $(BUILD_DIR)/boot.o $(BUILD_DIR)/uart.o $(BUILD_DIR)/mbox.o $(BUILD_DIR)/framebf.o $(COMMON_OFILES) $(VIDEO_OFILES) $(BUILD_DIR)/cmd.o $(ASSETS_OFILES) $(GAMES_OFILES) | $(BUILD_DIR)
	@echo "[LINK] Linking kernel image (with video): $(BUILD_DIR)/kernel8.elf"
	aarch64-none-elf-ld -nostdlib $(BUILD_DIR)/boot.o $(BUILD_DIR)/mbox.o $(BUILD_DIR)/framebf.o $(BUILD_DIR)/uart.o $(COMMON_OFILES) $(VIDEO_OFILES) $(BUILD_DIR)/cmd.o $(ASSETS_OFILES) $(GAMES_OFILES) -T $(ARCH_DIR)/link.ld -o $(BUILD_DIR)/kernel8.elf
	@echo "[OBJCOPY] Creating binary image: $(IMAGE)"
	aarch64-none-elf-objcopy -O binary $(BUILD_DIR)/kernel8.elf $(IMAGE)

$(IMAGE)_game: $(BUILD_DIR)/boot.o $(BUILD_DIR)/uart.o $(BUILD_DIR)/mbox.o $(BUILD_DIR)/framebf.o $(COMMON_OFILES) $(BUILD_DIR)/cmd.o $(ASSETS_OFILES) $(GAMES_OFILES) | $(BUILD_DIR)
	@echo "[LINK] Linking kernel image (with video): $(BUILD_DIR)/kernel8.elf"
	aarch64-none-elf-ld -nostdlib $(BUILD_DIR)/boot.o $(BUILD_DIR)/mbox.o $(BUILD_DIR)/framebf.o $(BUILD_DIR)/uart.o $(COMMON_OFILES) $(BUILD_DIR)/cmd.o $(ASSETS_OFILES) $(GAMES_OFILES) -T $(ARCH_DIR)/link.ld -o $(BUILD_DIR)/kernel8.elf
	@echo "[OBJCOPY] Creating binary image: $(IMAGE)"
	aarch64-none-elf-objcopy -O binary $(BUILD_DIR)/kernel8.elf $(IMAGE)

$(BUILD_DIR):
ifeq ($(OS),Windows_NT)
	if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
	if not exist $(BUILD_DIR)\backgrounds mkdir $(BUILD_DIR)\backgrounds
	if not exist $(BUILD_DIR)\fonts mkdir $(BUILD_DIR)\fonts
	if not exist $(BUILD_DIR)\sprites mkdir $(BUILD_DIR)\sprites
	if not exist $(BUILD_DIR)\games mkdir $(BUILD_DIR)\games
else
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)/backgrounds
	mkdir -p $(BUILD_DIR)/fonts
	mkdir -p $(BUILD_DIR)/sprites
	mkdir -p $(BUILD_DIR)/games
endif

# Clean build artifacts
clean:
	$(RM) $(RM_PATH)

#//////////////////////////////////////////////////////////////
#                             RUN TARGETS
#//////////////////////////////////////////////////////////////
# Run emulation with QEMU (UART1)
run1: 
	qemu-system-aarch64 -M raspi4b -kernel $(IMAGE) -serial null -serial stdio

# Run emulation with QEMU (UART0)
run0: 
	qemu-system-aarch64 -M raspi4b -kernel $(IMAGE) -serial stdio