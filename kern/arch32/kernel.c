/**
 * @file kernel.c
 * @author Nathan Porter
 * @brief C entrypoint for the kernel, coming from the
 * multiboot entry, with the stack defined.
 *
 */

#include <stdint.h>
#include <mbinfo.h>

#include <console.h>
#include <sim.h>

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
    console_puts("I am within the kernel\n");

    console_puts("fhiuwrfih\nurwehuirfew\n");

    while (1)
        continue;
}