# --- Toolchain (i686-elf cross compiler) ---
CROSS   := i686-elf-
CC      := $(CROSS)gcc
LD      := $(CROSS)ld
 
# Auto-discover every directory under kern/ that contains a .h file,
# then de-duplicate with sort. Works no matter how deep the tree goes.
INC_DIRS := $(sort $(dir $(shell find kern -name '*.h')))
 
CFLAGS  := -ffreestanding -std=gnu11 -O0 -Wall -Wextra -m32 \
            -nostdinc \
            $(addprefix -I, $(INC_DIRS)) \
            -I $(shell $(CC) -print-file-name=include) \
			-MMD -MP \
            -g \
			-DDEBUG # in debug mode always, take away for booting on hardware
ASFLAGS := -m32
LDFLAGS := -T linker.ld -nostdlib -z noexecstack
 
# --- Directory layout ---
KERN_DIR := kern
OBJ_DIR  := obj
ISO_DIR  := isodir
KERNEL   := $(ISO_DIR)/boot/kernel.elf
GRUB_CFG := $(ISO_DIR)/boot/grub/grub.cfg
ISO_FILE := pOS.iso
 
# --- Recursive source discovery across all of kern/ ---
C_SRCS := $(shell find $(KERN_DIR) -name '*.c')
S_SRCS := $(shell find $(KERN_DIR) -name '*.S')
 
# --- Object paths mirror the source tree under obj/ ---
C_OBJS := $(patsubst $(KERN_DIR)/%.c, $(OBJ_DIR)/%.o, $(C_SRCS))
S_OBJS := $(patsubst $(KERN_DIR)/%.S, $(OBJ_DIR)/%.o, $(S_SRCS))
OBJS   := $(C_OBJS) $(S_OBJS)
 
# =============================================================================
.PHONY: all iso run clean
 
all: $(KERNEL)
 
# --- Link ---
$(KERNEL): $(OBJS) linker.ld | $(ISO_DIR)/boot/grub
	@echo "[LD]  Linking kernel.elf"
	$(LD) $(LDFLAGS) -o $@ $(OBJS)
 
# --- Compile C source files ---
$(OBJ_DIR)/%.o: $(KERN_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "[CC]  $<"
	$(CC) $(CFLAGS) -c $< -o $@
 
# --- Assemble .S files (via GCC frontend for preprocessor support) ---
$(OBJ_DIR)/%.o: $(KERN_DIR)/%.S
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
#     QEMU is started in the background; gdb-multiarch then connects.
#     Expects an init.gdb in the same directory as this Makefile.
run: iso
	@echo "[QEMU] Starting — GDB stub on :1234"
	qemu-system-i386 -cdrom $(ISO_FILE) -m 2G -S -s &
	gdb-multiarch -x init.gdb
 
# --- Clean build artifacts (keeps source tree intact) ---
clean:
	@echo "[CLEAN] Removing obj/, isodir/, $(ISO_FILE)"
	rm -rf $(OBJ_DIR) $(ISO_DIR) $(ISO_FILE)

-include $(OBJS:.o=.d)
