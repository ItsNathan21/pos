/**
 * @file pic.c
 * @author Nathan Porter
 * @brief Implements remapping and control of the 8259A PIC.
 */
#include "pic.h"

#include <x86_asm.h>

/*
 * Writing to port 0x80 (POST diagnostic port) costs ~1–4 µs on real
 * hardware — enough time for the 8259 to finish processing a command.
 */
static inline void io_wait(void)
{
    outb(0x80, 0);
}

/**
 * @brief remaps the master and slave PIC controllers
 * to not have overlapping interrupt's. After this, the
 * interrupt table (idt) is ready to take in the
 * interrupts the kernel wants to use.
 *
 *
 */
void pic_remap(void)
{
    /* save current IRQ masks so we can restore them after reinit */
    uint8_t mask_master = inb(PIC_MASTER_DATA);
    uint8_t mask_slave = inb(PIC_SLAVE_DATA);

    /* ICW1: start initialization sequence (cascade mode, ICW4 required) */
    outb(PIC_MASTER_CMD, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outb(PIC_SLAVE_CMD, ICW1_INIT | ICW1_ICW4);
    io_wait();

    /* ICW2: vector base offsets */
    outb(PIC_MASTER_DATA, PIC_MASTER_VECTOR_BASE);
    io_wait();
    outb(PIC_SLAVE_DATA, PIC_SLAVE_VECTOR_BASE);
    io_wait();

    /* ICW3: cascade wiring
       master: bitmask of the IRQ line the slave is connected to (IRQ2 = bit 2)0
       slave:  its own cascade identity number (2) */
    outb(PIC_MASTER_DATA, 0x04);
    io_wait();
    outb(PIC_SLAVE_DATA, 0x02);
    io_wait();

    /* ICW4: 8086 mode */
    outb(PIC_MASTER_DATA, ICW4_8086);
    io_wait();
    outb(PIC_SLAVE_DATA, ICW4_8086);
    io_wait();

    /* restore saved masks */
    outb(PIC_MASTER_DATA, mask_master);
    outb(PIC_SLAVE_DATA, mask_slave);
}

/**
 * @brief sends an EOI command to the PIC
 * which enabled the current running interrupt
 *
 * @param irq the interrupt number to acknowlegde
 * its termination
 */
void pic_send_eoi(uint8_t irq)
{
    /*
     * Slave-originated IRQs (8–15) pass through the master's cascade
     * line, so both PICs need an EOI or the master stays latched.
     */
    if (irq >= 8)
        outb(PIC_SLAVE_CMD, PIC_EOI);
    outb(PIC_MASTER_CMD, PIC_EOI);
}

/**
 * @brief Masks the given irq number from
 * running with its proper PIC.
 *
 * @param irq
 */
void pic_mask_irq(uint8_t irq)
{
    uint16_t port;
    uint8_t bit;

    if (irq < 8)
    {
        port = PIC_MASTER_DATA;
        bit = irq;
    }
    else
    {
        port = PIC_SLAVE_DATA;
        bit = irq - 8;
    }

    outb(port, inb(port) | (1 << bit));
}

/**
 * @brief unmasks the irq number from running
 * with its proper PIC
 *
 * @param irq
 */
void pic_unmask_irq(uint8_t irq)
{
    uint16_t port;
    uint8_t bit;

    if (irq < 8)
    {
        port = PIC_MASTER_DATA;
        bit = irq;
    }
    else
    {
        port = PIC_SLAVE_DATA;
        bit = irq - 8;
    }

    outb(port, inb(port) & ~(1 << bit));
}

/**
 * @brief Masks all PIC's from
 * causing interrupts.
 *
 */
void pic_mask_all(void)
{
    outb(PIC_MASTER_DATA, 0xFF);
    outb(PIC_SLAVE_DATA, 0xFF);
}
