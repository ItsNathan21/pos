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

void shitballs() {
	terminal_initialize();
	terminal_writestring("shitballs");
	while(1);
}

void kmain(unsigned int magic_number, grub_multiboot_information *info) {
	terminal_initialize();
	if (magic_number != 0x2BADB002) {
		terminal_writestring("Fuck");
	}
	/* Initialize terminal interface */

	/* Newline support is left as an exercise. */
	printint(info->mem_lower);
	printint(info->mem_upper);
	while (1);
}
