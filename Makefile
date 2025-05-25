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
all: clean uart0_build build_core_all $(IMAGE)_video run0

# Build game mode
game: clean uart0_build build_core_game build_game $(IMAGE)_game run0

# Build video mode
video: clean uart0_build build_core_vid $(IMAGE)_video run0

# Build CLI mode
cli: CLI_MODE=1
cli: clean uart0_build build_core_cli build_cmd $(IMAGE)_cli run0

# Build TEAM mode
team: clean uart0_build build_core_team $(IMAGE)_video run0

# Build ALL mode
all_mode: CLI_MODE=1
all_mode: clean uart0_build build_core_all $(IMAGE)_video run0

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

# Build command files
build_cmd: CLI_MODE=1
build_cmd: $(BUILD_DIR)/cmd.o

# Build command source files
$(BUILD_DIR)/cmd.o: $(CMD_DIR)/cmd.c | $(BUILD_DIR)
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

# More efficient way to handle build core targets
define BUILD_CORE_TEMPLATE
build_core_$(1): $(BUILD_DIR)/boot.o $(BUILD_DIR)/mbox.o $(BUILD_DIR)/framebf.o $(COMMON_OFILES)
	@echo "[BUILD] Building core components ($(2))"
	aarch64-none-elf-gcc $(GCCFLAGS) -D$(2) -c $(SRC_DIR)/kernel.c -o $(BUILD_DIR)/kernel.o
endef

# Generate build core targets
$(eval $(call BUILD_CORE_TEMPLATE,game,GAME_INIT))
$(eval $(call BUILD_CORE_TEMPLATE,vid,VID_INIT))
$(eval $(call BUILD_CORE_TEMPLATE,cli,CLI_INIT))
$(eval $(call BUILD_CORE_TEMPLATE,team,TEAM_INIT))
$(eval $(call BUILD_CORE_TEMPLATE,all,ALL_INIT))

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

$(IMAGE)_game: $(BUILD_DIR)/boot.o $(BUILD_DIR)/uart.o $(BUILD_DIR)/mbox.o $(BUILD_DIR)/framebf.o $(COMMON_OFILES) $(ASSETS_OFILES) $(GAMES_OFILES) | $(BUILD_DIR)
	@echo "[LINK] Linking kernel image (with video): $(BUILD_DIR)/kernel8.elf"
	aarch64-none-elf-ld -nostdlib $(BUILD_DIR)/boot.o $(BUILD_DIR)/mbox.o $(BUILD_DIR)/framebf.o $(BUILD_DIR)/uart.o $(COMMON_OFILES) $(ASSETS_OFILES) $(GAMES_OFILES) -T $(ARCH_DIR)/link.ld -o $(BUILD_DIR)/kernel8.elf
	@echo "[OBJCOPY] Creating binary image: $(IMAGE)"
	aarch64-none-elf-objcopy -O binary $(BUILD_DIR)/kernel8.elf $(IMAGE)

$(IMAGE)_cli: CLI_MODE=1
$(IMAGE)_cli: $(BUILD_DIR)/boot.o $(BUILD_DIR)/uart.o $(BUILD_DIR)/mbox.o $(BUILD_DIR)/framebf.o $(COMMON_OFILES) $(VIDEO_OFILES) $(BUILD_DIR)/cmd.o $(ASSETS_OFILES) $(GAMES_OFILES) | $(BUILD_DIR)
	@echo "[LINK] Linking kernel image (with CLI): $(BUILD_DIR)/kernel8.elf"
	aarch64-none-elf-ld -nostdlib $(BUILD_DIR)/boot.o $(BUILD_DIR)/mbox.o $(BUILD_DIR)/framebf.o $(BUILD_DIR)/uart.o $(COMMON_OFILES) $(VIDEO_OFILES) $(BUILD_DIR)/cmd.o $(ASSETS_OFILES) $(GAMES_OFILES) -T $(ARCH_DIR)/link.ld -o $(BUILD_DIR)/kernel8.elf
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