#include "include/terminal.h"
#include <stdint.h>

typedef struct {

	int32_t flags;
	int32_t mem_lower;
	int32_t mem_upper;
	int32_t boot_device;
	int32_t cmdline;
	int32_t mods_count;
	int32_t mods_addr;

	int16_t syms;

	int32_t mmap_length;
	int32_t mmap_addr;



} __attribute__((packed)) grub_multiboot_information;

typedef struct {
	int16_t lim_low;
	int16_t base_low;
	int8_t base_mid;
	int8_t access;
	int8_t lim_high_flags;
	int8_t base_high;
} gdt_entry_t;

#define GDT_ENTRIES (6)
typedef struct {
	gdt_entry_t entry[GDT_ENTRIES];
} gdt_t;

extern uint8_t tss[], end_tss[];
extern uint8_t gdt_start[];
#define GDT ((gdt_t *)gdt_start)

extern uint8_t idt[], end_idt[];


void printint(unsigned int x) {
    char buf[11]; // "0x" + 8 hex digits + null terminator
    buf[0] = '0';
    buf[1] = 'x';
    buf[10] = '\0';

    for (int i = 0; i < 8; i++) {
        int shift = (7 - i) * 4;
        unsigned int nibble = (x >> shift) & 0xF;
        buf[2 + i] = (nibble < 10) ? ('0' + nibble) : ('a' + (nibble - 10));
    }

    terminal_writestring(buf);
}

void gdt_init() {

	uintptr_t TSS = (uintptr_t)tss;
	uintptr_t END_TSS = (uintptr_t)end_tss;

	#define TSS_BASE_HIGH ((TSS & (0xff << 24)) << 24)
	#define TSS_FLAGS (0x00)
	#define TSS_LIMIT (END_TSS - TSS - 1)
	#define TSS_LIM_HIGH_FLAGS (TSS_FLAGS | ((TSS_LIMIT & (0xf << 20)) << 16))
	#define TSS_ACCESS (0x89)
	#define TSS_BASE_MID ((TSS & (0xf << 16)) << 16)
	#define TSS_BASE_LOW ((TSS & 0xffff) << 16)
	#define TSS_LIM_LOW (TSS_LIMIT & 0xffff)

	GDT->entry[5].base_high = TSS_BASE_HIGH;
	GDT->entry[5].lim_high_flags = TSS_LIM_HIGH_FLAGS;
	GDT->entry[5].access = TSS_ACCESS;
	GDT->entry[5].base_mid = TSS_BASE_MID;
	GDT->entry[5].base_low = TSS_BASE_LOW;
	GDT->entry[5].lim_low = TSS_LIM_LOW;
	
}

extern void load_gdt(int32_t limit, int32_t base);
extern void load_idt(int32_t limit, int32_t base);

void kmain(unsigned int magic_number, __attribute__((unused)) grub_multiboot_information *info) {
	gdt_init();
	load_gdt(sizeof(gdt_t) - 1, (int32_t)gdt_start);
	load_idt((int32_t)end_idt - (int32_t)idt - 1, (int32_t)idt);
	 // need a "add idt IRQ TODO:"

	terminal_initialize();
	if (magic_number != 0x2BADB002) {
		terminal_writestring("Fuck");
	}

	printint((unsigned int)tss);
	printint((unsigned int)end_tss);
	printint((unsigned int)gdt_start);
	while (1);
}
