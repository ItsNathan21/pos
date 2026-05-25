/**
 * @file idt.h
 * @author Nathan Porter
 * @brief Contains macros used for the IDT's
 * declaration, and headers for the functions
 * used to initialize the IDT
 *
 */

#ifndef _IDT_H_
#define _IDT_H_

#include <stdint.h>

#include "idt_macros.h"

/**
 * @brief memory descrption of both used gate types
 * (trap, interrupt). They differ only through the
 * required bits in the flags field, but they both
 * have the same layout.
 *
 */
struct gate
{
    uint16_t offset_low;
    uint16_t segment_selector;
    uint16_t flags;
    uint16_t offset_upper;
} __attribute__((packed));

typedef struct gate gate_t;

/**
 * @brief the x86 IDT is 256 gate entries
 *
 */
typedef struct gate x86_IDT[IDT_ENTRIES];

/**
 * @brief the description of the IDT register.
 * limit corresponds to how large the IDT is, in bytes.
 * Base corresponds to the base address of the IDT.
 *
 */
struct idtr
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

x86_IDT *idt_base(void);

/**
 * @brief places the idtr into the given
 * srtuct pointer
 *
 * @note should not be called elsewhere outsise
 * of here, only exists here since it is an
 * assembly function, use idt_base instead to
 * avoid picking through the contents of the idtr.
 *
 * @param idtr the struct to place the current
 * idtr's contents into.
 */
void __get_idtr(struct idtr *idtr);

void store_idt(x86_IDT *idt);

/**
 * @brief places the given idtr into the IDT.
 *
 * @note should not be called elsewhere, it exists
 * here as a declaration to use since its an assembly
 * function. Use store_idt instead.
 *
 * @param idtr the new idt pointer to store in
 */
void __store_idtr(struct idtr *idtr);

void idt_initialize(void);

void idt_install_hardware_handlers(uint32_t timer_tickback);

#endif /* _IDT_H_  */
