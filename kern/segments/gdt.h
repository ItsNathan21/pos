/**
 * @file gdt.h
 * @author Nathan Porter
 * @brief Contains macros and segment selector
 * numbers used within the GDT.
 */
#ifndef _GDT_H_
#define _GDT_H_

#include <stdint.h>

/**
 * @brief the layout in memory of a
 * single GDT entry
 *
 */
struct gdt_entry
{
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    // can't split it up into two 4 bit C variables...sad
    uint8_t flags_and_limit_high;
    uint8_t base_high;
} __attribute__((packed));
typedef struct gdt_entry gdt_entry_t;

struct tss_entry
{
    uint32_t prev_tss;
    uint32_t esp0;
    uint32_t ss0;
    uint32_t esp1;
    uint32_t ss1;
    uint32_t esp2;
    uint32_t ss2;
    uint32_t cr3;
    uint32_t eip, eflags;
    uint32_t eax, ecx, edx, ebx;
    uint32_t esp, ebp, esi, edi;
    uint32_t es, cs, ss, ds, fs, gs;
    uint32_t ldt;
    uint16_t trap;
    uint16_t iomap_base;
} __attribute__((packed));
typedef struct tss_entry tss_entry_t;

/**
 * @brief number of GDT entries
 *
 * Currently its the NULL descriptor,
 * 2 for kernel code+data,
 * 2 for user code+data,
 * 1 for TSS
 *
 */
#define NUM_GDT_ENTRIES (6)
#define KERNEL_CS_ENTRY (1)
#define KERNEL_DS_ENTRY (2)
#define USER_CS_ENTRY (3)
#define USER_DS_ENTRY (4)
#define TSS_ENTRY (5)

#define ACCESS_PRESENT (1 << 7)
#define ACCESS_DPL_KERNEL (0x0 << 5)
#define ACCESS_DPL_USER (0x3 << 5)
#define ACCESS_DESCRIPTOR (1 << 4)
#define ACCESS_EXECUTABLE (1 << 3)
#define ACCESS_DIRECTION_CONFORMING (0 << 2)
#define ACCESS_WRITEABLE (1 << 1)
#define ACCESS_READABLE (1 << 1)
#define ACCESS_TSS_32_AVAILABLE (0x09)

// other variations exist...but this is a
// 32 bit kernel. Add more if you want more
// segments like 16 bit or 64 bit.
#define FLAGS_GRANULARITY_BYTE (0 << 7)
#define FLAGS_GRANULARITY_PAGE (1 << 7)
#define FLAGS_PROTECTED_MODE (1 << 6)

#define KERNEL_CS (0x08) // index 1, TI=0, RPL=0
#define KERNEL_DS (0x10) // index 2, TI=0, RPL=0
#define USER_CS (0x1B)   // index 3, TI=0, RPL=3
#define USER_DS (0x23)   // index 4, TI=0, RPL=3

void initialize_and_load_gdt(void);

#endif /* _GDT_H_ */
