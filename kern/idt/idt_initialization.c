/**
 * @file idt_initialization.c
 * @author your name (you@domain.com)
 * @brief Contains the code to initialize the
 * IDT and its contents in the IDTR.
 *
 */

#include <string.h>
#include <x86_asm.h>

#include "idt.h"

/** @brief the IDT we're actually using, scary! */
static x86_IDT IDT;

/**
 * @brief initializes the idt within the IDTR.
 *
 * Blanks out all entries within the IDT then loads them in,
 * after this function is called, no entries are valid.
 *
 * The caller should have already disabled interrupts! They
 * will be re-disabled within this function to ensure safety,
 * though this should be redundant. Since interrupts are most
 * definetly not ready to be called after this function, they
 * are left disabled.
 *
 */
void idt_initialize(void)
{
    disable_interrupts();

    /*
        Clear all entries within the IDT, this
        clears all P (gate present) flags, thus
        effectively neutralizing all interrupts
        that might've been placed before then.
    */
    memset(IDT, 0, sizeof(x86_IDT));

    store_idt(&IDT);
}