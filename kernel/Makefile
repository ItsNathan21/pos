# === Toolchain ===
CROSS   := i686-elf-
CC      := $(CROSS)gcc
LD      := $(CROSS)ld
ASFLAGS := -m32
CFLAGS  := -ffreestanding -O2 -Wall -Wextra -m32
LDFLAGS := -T linker.ld -nostdlib

# === Directory layout ===
SRC_DIR     := src
BOOT_DIR    := boot
OBJ_DIR     := obj
GRUB_DIR    := ../grub
ISO_DIR     := $(GRUB_DIR)/isodir
KERNEL_OUT  := $(ISO_DIR)/boot/kernel.elf
GRUB_CFG    := $(ISO_DIR)/boot/grub/grub.cfg
ISO_FILE    := $(GRUB_DIR)/os.iso

# === Source files ===
C_SRCS    := $(wildcard $(SRC_DIR)/*.c)
S_SRCS    := $(wildcard $(BOOT_DIR)/*.s) $(wildcard $(BOOT_DIR)/*.S)
C_OBJS    := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(C_SRCS))
S_OBJS    := $(patsubst $(BOOT_DIR)/%.s,$(OBJ_DIR)/%.o,$(S_SRCS))
OBJS      := $(C_OBJS) $(S_OBJS)

# === Targets ===

.PHONY: all clean iso run

default: clean all iso run

all: $(KERNEL_OUT)

# Final kernel output
$(KERNEL_OUT): $(OBJS) linker.ld | $(ISO_DIR)/boot/grub
	@echo "[LD] Linking kernel..."
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

# Compile C files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "[CC] $<"
	$(CC) $(CFLAGS) -c $< -o $@

# Assemble .s/.S files using GCC
$(OBJ_DIR)/%.o: $(BOOT_DIR)/%.s
	@mkdir -p $(dir $@)
	@echo "[AS] $<"
	$(CC) $(ASFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(BOOT_DIR)/%.S
	@mkdir -p $(dir $@)
	@echo "[AS] $<"
	$(CC) $(ASFLAGS) -c $< -o $@

# Create GRUB folder structure and config
$(ISO_DIR)/boot/grub:
	@echo "[MKDIR] Creating GRUB folder structure..."
	mkdir -p $@
	@echo "[CFG] Writing grub.cfg..."
	echo 'menuentry "pos" {\n\
    multiboot /boot/pos.elf\n\
    boot\n\
}' > $(GRUB_CFG)

# Build ISO
iso: all
	@echo "[ISO] Creating bootable ISO at $(ISO_FILE)..."
	grub-mkrescue -o $(ISO_FILE) $(ISO_DIR)

# Run in QEMU
run: iso
	@echo "[QEMU] Booting..."
	qemu-system-i386 -cdrom $(ISO_FILE) -m 512M -S -s

# Clean everything
clean:
	@echo "[CLEAN] Removing obj/ and kernel.elf..."
	rm -rf $(OBJ_DIR) $(KERNEL_OUT) $(ISO_FILE)