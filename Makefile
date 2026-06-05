.PHONY: build run clean iso

# Compiler and tools
NASM = nasm
GCC = gcc
LD = ld
QEMU = qemu-system-i386

# Directories
BUILD_DIR = build
BIN_DIR = bin
SRC_DIR = src

# Flags
NASM_FLAGS = -f elf32
GCC_FLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -c
LD_FLAGS = -m elf_i386 -T $(SRC_DIR)/linker.ld -o

# Files
BOOT_SRC = $(SRC_DIR)/boot.asm
KERNEL_SRC = $(SRC_DIR)/kernel.c
LINKER_SCRIPT = $(SRC_DIR)/linker.ld

BOOT_OBJ = $(BUILD_DIR)/boot.o
KERNEL_OBJ = $(BUILD_DIR)/kernel.o
KERNEL_BIN = $(BIN_DIR)/kernel.bin
OS_ISO = $(BIN_DIR)/B22-OS.iso

# Default target
all: iso

# Create directories
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Compile bootloader
$(BOOT_OBJ): $(BOOT_SRC) | $(BUILD_DIR)
	$(NASM) $(NASM_FLAGS) -o $@ $<

# Compile kernel
$(KERNEL_OBJ): $(KERNEL_SRC) | $(BUILD_DIR)
	$(GCC) $(GCC_FLAGS) -o $@ $<

# Link kernel
$(KERNEL_BIN): $(BOOT_OBJ) $(KERNEL_OBJ) $(LINKER_SCRIPT) | $(BIN_DIR)
	$(LD) $(LD_FLAGS) $@ $(BOOT_OBJ) $(KERNEL_OBJ)

# Create ISO image
iso: $(KERNEL_BIN)
	mkdir -p $(BUILD_DIR)/iso/boot/grub
	cp $(KERNEL_BIN) $(BUILD_DIR)/iso/boot/kernel.bin
	cp $(SRC_DIR)/grub.cfg $(BUILD_DIR)/iso/boot/grub/grub.cfg
	grub-mkrescue -o $(OS_ISO) $(BUILD_DIR)/iso

# Run in QEMU
run: iso
	$(QEMU) -cdrom $(OS_ISO) -m 256

# Debug with QEMU
debug: iso
	$(QEMU) -cdrom $(OS_ISO) -m 256 -s -S

# Clean
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Help
help:
	@echo "B22-OS Makefile targets:"
	@echo "  make build    - Build the OS kernel"
	@echo "  make iso      - Create bootable ISO image (default)"
	@echo "  make run      - Build and run in QEMU"
	@echo "  make debug    - Run in QEMU with debugger"
	@echo "  make clean    - Remove build files"
	@echo "  make help     - Show this help message"
