# =============================================================================
#  Makefile — 32-bit hobby OS  (kern/arch32/)
#
#  Directory layout expected:
#    kern/arch32/inc/        → header files (.h)
#    kern/arch32/<any>/      → source files (.c, .S) — arbitrarily nested
#    linker.ld               → linker script (sits next to this Makefile)
#
#  Outputs:
#    obj/                    → compiled objects (mirrors source tree)
#    isodir/boot/kernel.elf  → linked kernel
#    pOS.iso                  → bootable GRUB ISO
# =============================================================================
 
# --- Toolchain (i686-elf cross compiler) ---
CROSS   := i686-elf-
CC      := $(CROSS)gcc
LD      := $(CROSS)ld
 
CFLAGS  := -ffreestanding -std=gnu11 -O0 -Wall -Wextra -m32 \
            -nostdinc \
            -I kern/arch32/inc \
            -I $(shell $(CC) -print-file-name=include) \
			-g

ASFLAGS := -m32
LDFLAGS := -T linker.ld -nostdlib -z noexecstack
 
# --- Directory layout ---
ARCH_DIR := kern/arch32
INC_DIR  := $(ARCH_DIR)/inc
OBJ_DIR  := obj
ISO_DIR  := isodir
KERNEL   := $(ISO_DIR)/boot/kernel.elf
GRUB_CFG := $(ISO_DIR)/boot/grub/grub.cfg
ISO_FILE := pOS.iso
 
# --- Recursive source discovery (skips the inc/ header directory) ---
C_SRCS := $(shell find $(ARCH_DIR) -path '$(INC_DIR)' -prune \
               -o -name '*.c' -print)
S_SRCS := $(shell find $(ARCH_DIR) -path '$(INC_DIR)' -prune \
               -o -name '*.S' -print)
 
# --- Object paths mirror the source tree under obj/ ---
C_OBJS := $(patsubst $(ARCH_DIR)/%.c, $(OBJ_DIR)/%.o, $(C_SRCS))
S_OBJS := $(patsubst $(ARCH_DIR)/%.S, $(OBJ_DIR)/%.o, $(S_SRCS))
OBJS   := $(C_OBJS) $(S_OBJS)
 
# =============================================================================
.PHONY: all iso run clean
 
all: $(KERNEL)
 
# --- Link ---
$(KERNEL): $(OBJS) linker.ld | $(ISO_DIR)/boot/grub
	@echo "[LD]  Linking kernel.elf"
	$(LD) $(LDFLAGS) -o $@ $(OBJS)
 
# --- Compile C source files ---
$(OBJ_DIR)/%.o: $(ARCH_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "[CC]  $<"
	$(CC) $(CFLAGS) -c $< -o $@
 
# --- Assemble .S files (via GCC frontend for preprocessor support) ---
$(OBJ_DIR)/%.o: $(ARCH_DIR)/%.S
	@mkdir -p $(dir $@)
	@echo "[AS]  $<"
	$(CC) $(CFLAGS) $(ASFLAGS) -c $< -o $@
 
# --- Create GRUB directory structure and write grub.cfg ---
$(ISO_DIR)/boot/grub:
	@mkdir -p $@
	@printf 'set timeout=0\nset default=0\n\nmenuentry "pOS" {\n\tmultiboot /boot/kernel.elf\n\tboot\n}\n' \
		> $(GRUB_CFG)
	@echo "[CFG] grub.cfg written"
 
# --- Build the bootable ISO ---
iso: all
	@echo "[ISO] Building $(ISO_FILE) ..."
	grub-mkrescue -o $(ISO_FILE) $(ISO_DIR)
 
# --- Run in QEMU with GDB stub on port 1234 ---
#     QEMU is started in the background, then gdb-multiarch connects.
#     Expects an init.gdb in the same directory as this Makefile.
run: iso
	@echo "[QEMU] Starting — GDB stub on :1234"
	qemu-system-i386 -cdrom $(ISO_FILE) -m 2G -S -s &
	gdb-multiarch -x init.gdb
 
# --- Clean build artifacts (keeps source tree intact) ---
clean:
	@echo "[CLEAN] Removing obj/, isodir/, $(ISO_FILE)"
	rm -rf $(OBJ_DIR) $(ISO_DIR) $(ISO_FILE)
