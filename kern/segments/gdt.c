/**
 * @file gdt.c
 * @author Nathan Porter
 * @brief Contains the description of the GDT
 * and its place in memory, and the functions to
 * fill in its entries
 */

#include "gdt.h"
#include "segment_asm.h"

#include <string.h>
#include <panic.h>

static gdt_entry_t GDT[NUM_GDT_ENTRIES];
tss_entry_t TSS;

/**
 * @brief initializes the given index within the
 * GDT as a segment with base 0x0 and limit 0xffffffff.
 *
 * Zereos out any non base/limit fields
 *
 * @param index the index into the GDT to initialize
 */
static void initialize_infinte_gdt_seg(int index)
{
    assert(index < NUM_GDT_ENTRIES);
    gdt_entry_t *entry = &GDT[index];
    entry->limit_low = 0xffff;
    entry->base_low = 0x0000;
    entry->base_middle = 0x00;
    entry->access = 0x00;
    // flags are the upper 4 bits, don't touch those! (yet :3)
    entry->flags_and_limit_high = 0x0f;
    entry->base_high = 0x00;
}

static void initialize_tss_gdt_seg(int index, uint32_t base, uint32_t limit)
{
    assert(index < NUM_GDT_ENTRIES);
    gdt_entry_t *entry = &GDT[index];
    entry->limit_low = limit & 0xffff;
    entry->base_low = base & 0xffff;
    entry->base_middle = (base >> 16) & 0xff;
    entry->access = ACCESS_PRESENT | ACCESS_DPL_KERNEL | ACCESS_TSS_32_AVAILABLE;
    entry->flags_and_limit_high = (limit >> 16) & 0x0f;
    entry->base_high = (base >> 24) & 0xff;
}

void initialize_and_load_gdt(void)
{
    /* NULL descriptor, should be
        all 0's, never touch it! */
    memset(&GDT[0], 0, sizeof(gdt_entry_t));

    /*
        Kernel's CS segment. Should be base of 0x000..00
        and limit of maximum (0xff...ff)
    */
    initialize_infinte_gdt_seg(KERNEL_CS_ENTRY);
    GDT[KERNEL_CS_ENTRY].access = (ACCESS_PRESENT | ACCESS_DPL_KERNEL |
                                   ACCESS_DESCRIPTOR | ACCESS_EXECUTABLE |
                                   ACCESS_READABLE);
    GDT[KERNEL_CS_ENTRY].flags_and_limit_high |= (FLAGS_GRANULARITY_PAGE | FLAGS_PROTECTED_MODE);

    /*
        Kernel DS entry...same thing
    */
    initialize_infinte_gdt_seg(KERNEL_DS_ENTRY);
    GDT[KERNEL_DS_ENTRY].access = (ACCESS_PRESENT | ACCESS_DPL_KERNEL |
                                   ACCESS_DESCRIPTOR | ACCESS_WRITEABLE);
    GDT[KERNEL_DS_ENTRY].flags_and_limit_high |= (FLAGS_GRANULARITY_PAGE | FLAGS_PROTECTED_MODE);

    /*
        User CS entry...same as kernel code except privilage level
    */
    initialize_infinte_gdt_seg(USER_CS_ENTRY);
    GDT[USER_CS_ENTRY].access = (ACCESS_PRESENT | ACCESS_DPL_USER |
                                 ACCESS_DESCRIPTOR | ACCESS_EXECUTABLE |
                                 ACCESS_READABLE);
    GDT[USER_CS_ENTRY].flags_and_limit_high |= (FLAGS_GRANULARITY_PAGE | FLAGS_PROTECTED_MODE);

    /*
        User DS entry...
    */
    initialize_infinte_gdt_seg(USER_DS_ENTRY);
    GDT[USER_DS_ENTRY].access = (ACCESS_PRESENT | ACCESS_DPL_USER |
                                 ACCESS_DESCRIPTOR | ACCESS_WRITEABLE);
    GDT[USER_DS_ENTRY].flags_and_limit_high |= (FLAGS_GRANULARITY_PAGE | FLAGS_PROTECTED_MODE);

    /*
        TSS, the "hardware" task we setup to switch between
        all tasks. We only use this task and rather switch its esp0
        between contexts for switches.
    */

    memset(&TSS, 0, sizeof(tss_entry_t));
    TSS.ss0 = KERNEL_DS;
    TSS.esp0 = 0;
    TSS.iomap_base = sizeof(tss_entry_t);
    initialize_tss_gdt_seg(TSS_ENTRY, (uint32_t)&TSS, sizeof(tss_entry_t) - 1);

    gdtr_t gdtr = {
        .limit = sizeof(GDT) - 1,
        .base = (uint32_t)GDT,
    };

    load_gdt(&gdtr, KERNEL_CS, KERNEL_DS);
    load_tss(TSS_ENTRY << 3);
}
