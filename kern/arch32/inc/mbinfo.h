/**
 * @file mbinfo.h
 *
 * @author Nathan Porter
 *
 * @brief Defines the multiboot struct used by the kernel
 *
 * Source for information used here:
 * https://www.gnu.org/software/grub/manual/multiboot/multiboot.html#Machine-state
 *
 */

#ifndef _MBINFO_H_
#define _MBINFO_H_

#include <stdint.h>

/**
 * @brief bit fields of the flags field for
 * multiboot stucture, presence of each bit
 * implying its validity.
 *
 */

#define MEM (1u << 0)
#define BOOT_DEVICE (1u << 1)
#define CMDLINE (1u << 2)
#define MODS (1u << 3)
#define SYMS (1u << 4)
/**
 * syms 4 and 5 are mutually exclusive, don't
 * use both at the same time!
 */
// #define SYMS (1u << 5)
#define MMAP (1u << 6)
#define DRIVES (1u << 7)
#define CONFIG (1u << 8)
#define BOOT_LOADER_NAME (1u << 9)
#define APM_TABLE (1u << 10)
#define VBE (1u << 11)
#define FRAMEBUFFER (1u << 12)

/**
 * @brief structure pointed to by the
 * mmap field in the multiboot structure.
 *
 * Indicates a memory mapped region the BIOS
 * gave us, thanks, BIOS!
 */
typedef struct
{
    uint32_t size;
    uint64_t base_addr;
    uint64_t length;
    uint32_t type;
} __attribute__((packed)) mmap_info_t;

/**
 * @brief Structure returned by the bootloader in
 * %ebx given to the kernel in 32-bit protected mode,
 * for the kernel to have fun with in whichever way
 * it sees fit.
 *
 * Presence of fields is determined by its corresponding
 * bit being set within the first entry flags, which is
 * always present. Info at
 *
 * https://www.gnu.org/software/grub/manual/multiboot/multiboot.html#Machine-state
 *
 * for which fields correspond to which bits exactly.
 *
 */
typedef struct
{
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    uint32_t syms[4];
    uint32_t mmap_length;
    uint32_t mmap_addr;
    uint32_t drives_length;
    uint32_t drives_addr;
    uint32_t config_table;
    uint32_t boot_loader_name;
    uint32_t apm_table;
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint32_t vbe_mode;
    uint32_t vbe_interface_seg;
    uint32_t vbe_interface_off;
    uint32_t vbe_interface_len;
    uint32_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint32_t framebuffer_bpp;
    uint32_t framebuffer_type;
    uint32_t color_info;
} mbinfo_t;

#endif /* _MBINFO_H_ */
