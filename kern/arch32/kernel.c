/**
 * @file kernel.c
 * @author Nathan Porter
 * @brief C entrypoint for the kernel, coming from the
 * multiboot entry, with the stack defined.
 *
 */

#include <stdint.h>
#include <mbinfo.h>
#include <panic.h>
#include <console.h>
#include <bits.h>
#include <kprintf.h>

#define MBOOT_MAGIC_NUMBER (0x2BADB002)

/* mmap entry type values from the multiboot spec */
#define MMAP_TYPE_AVAILABLE 1 /* free RAM */
#define MMAP_TYPE_RESERVED 2  /* do not use */
#define MMAP_TYPE_ACPI 3      /* ACPI reclaimable */
#define MMAP_TYPE_NVS 4       /* ACPI non-volatile storage */
#define MMAP_TYPE_BADRAM 5    /* bad memory */

static const char *mmap_type_str(uint32_t type)
{
    switch (type)
    {
    case MMAP_TYPE_AVAILABLE:
        return "available";
    case MMAP_TYPE_RESERVED:
        return "reserved";
    case MMAP_TYPE_ACPI:
        return "ACPI reclaimable";
    case MMAP_TYPE_NVS:
        return "ACPI NVS";
    case MMAP_TYPE_BADRAM:
        return "bad RAM";
    default:
        return "unknown";
    }
}

static void display_mmap_information(mbinfo_t *info)
{
    assert(info->flags & MMAP);

    kprintf("--- memory map ---\n");

    /*
      The mmap region is a packed array of variable-length entries.
      Each entry starts with a uint32_t 'size' field that gives the
      number of bytes in the rest of the entry (NOT including the
      size field itself). You must advance by (entry->size + 4) to
      reach the next entry — do NOT use sizeof(mmap_info_t) here,
      as the BIOS may produce entries of varying sizes.
     */
    uint8_t *entry_ptr = (uint8_t *)info->mmap_addr;
    uint8_t *mmap_end = entry_ptr + info->mmap_length;

    int region = 0;
    while (entry_ptr < mmap_end)
    {
        mmap_info_t *entry = (mmap_info_t *)entry_ptr;

        uint32_t base_lo = (uint32_t)(entry->base_addr & 0xFFFFFFFF);
        uint32_t base_hi = (uint32_t)(entry->base_addr >> 32);
        uint32_t len_lo = (uint32_t)(entry->length & 0xFFFFFFFF);
        uint32_t len_hi = (uint32_t)(entry->length >> 32);

        kprintf("  [%d] base=", region++);
        if (base_hi)
            kprintf("%#010x%08x", base_hi, base_lo);
        else
            kprintf("%#010x", base_lo);

        kprintf("  length=");
        if (len_hi)
            kprintf("%#010x%08x", len_hi, len_lo);
        else
            kprintf("%#010x", len_lo);

        kprintf("  type=%u (%s)\n", entry->type, mmap_type_str(entry->type));

        /* advance to the next entry: size field is 4 bytes, not included in entry->size */
        entry_ptr += entry->size + sizeof(entry->size);
    }

    kprintf("------------------\n");
}

/**
 * @brief
 *
 * @param magic_number the multiboot magic number provided
 * to the bootloader, given back to us to check the validity
 * of the multiboot loader.
 *
 * @param info Information about the system, provided by the
 * bootloader during the boot process. Must verify that the
 * sections are valid with magic number before using any fields.
 *
 * @return Never returns
 */
void kernel_setup(uint32_t magic_number, mbinfo_t *info)
{
    console_show_cursor();
    clear_console();
    console_set_color(FGND_GREEN | BGND_BLACK);

    if (magic_number != MBOOT_MAGIC_NUMBER)
        panic("Incorrect magic number received during boot, "
              "was %d but should have been %d\n",
              magic_number, MBOOT_MAGIC_NUMBER);

    if (info->flags & MEM)
    {
        kprintf("Amount of lower memory is %ld\n", info->mem_lower);
        kprintf("Amount of upper memory is %ld\n", info->mem_upper);
    }
    if (info->flags & MMAP)
    {
        kprintf("mmap field is valid\n");
        display_mmap_information(info);
    }

    kprintf("Spinning forever...\n");

    while (1)
        continue;
}