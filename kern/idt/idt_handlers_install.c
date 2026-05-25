/**
 * @file idt_handlers_install.c
 * @author your name (you@domain.com)
 * @brief Contains the functions used to
 * install various handler types within the
 * kernel.
 *
 *
 *
 */

#include "idt.h"
#include "idt_macros.h"

#include <pit_timer.h>
#include <timer.h>
#include <panic.h>
#include <x86_asm.h>
#include <hardware_interrupts.h>
#include <bits.h>
#include <gdt.h>
#include <pic.h>

/** @brief the gate present bit in the 16 bit word of flags  */
#define GATE_PRESENT (1 << 15)
/** @brief kernel DPL for the gate, can only be called in kernel mode */
#define DPL_KERNEL (0x0 << 13)
/** @brief user DPL for the gate, can be called in user mode */
#define DPL_USER (0x3 << 13)

/** @brief identifier for a 32 bit interrupt gate, always the same */
#define INTERRUPT_GATE_IDENTIFIER (0xE << 8)
/** @brief identifier for a 32 bit trap gate, always the same */
#define TRAP_GATE_IDENTIFIER (0xF << 8)

/**
 * @brief Set the PIT to fire at the rate given by timer_cycles.
 *
 * Writes the mode and two-byte period to the IO ports.
 */
void set_timer_cycles()
{
    static const int timer_cycles = TIMER_2MS;

    // send the period to the IO port one byte at a time
    outb(TIMER_MODE_IO_PORT, TIMER_SQUARE_WAVE);
    outb(TIMER_PERIOD_IO_PORT, BYTE(timer_cycles, 0));
    outb(TIMER_PERIOD_IO_PORT, BYTE(timer_cycles, 1));
}

/**
 * @brief Installs the given function as an interrupt gate at IDT entry entry_num
 *
 * @param entry_num the entry number into the IDT
 * @param function the new function to install, as an interrupt gate into the IDT
 */
static void install_interrupt_gate_entry_at(int entry_num, uint32_t function)
{
    assert(entry_num >= 0);

    x86_IDT *IDT = idt_base();
    /*
        Index entry_num entries into the IDT, so we get
        the pointer to the gate we want to modify
    */
    gate_t *gate = &((*IDT)[entry_num]);

    /* place the chopped up function pointer in the IDT */
    gate->offset_low = GET_16BIT_WORD(function, 0);
    gate->offset_upper = GET_16BIT_WORD(function, 1);
    /* since this is kernel code... */
    gate->segment_selector = KERNEL_CS;
    /* mark the final present flag, and the remaining required identifiers! */
    gate->flags = INTERRUPT_GATE_IDENTIFIER | DPL_KERNEL | GATE_PRESENT;
}

/**
 * @brief installs the base hardware handlers required for the
 * kernel within the IDT.
 *
 * Thus far, this includes
 *
 * keyboard (PS/2)
 * PIT timer
 *
 * @param timer_tickback
 */
void idt_install_hardware_handlers(uint32_t timer_tickback)
{
    /* set the timer to run at the rate we want */
    set_timer_cycles();
    /*
        Register this timer tickback function as the
    */
    timer_install_tickback((void (*)(void))timer_tickback);

    install_interrupt_gate_entry_at(IDT_ENTRY_PIT, (uint32_t)timer_interrupt_main);
    pic_unmask_irq(IRQ_TIMER);

    install_interrupt_gate_entry_at(IDT_ENTRY_PS2_KBD, (uint32_t)keyboard_interrupt_main);
    pic_unmask_irq(IRQ_KEYBOARD);
}