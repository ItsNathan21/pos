/**
 * @file pic.h
 * @author Nathan Porter
 * @brief Definitions and declarations for the 8259A
 * Programmable Interrupt Controller (PIC).
 *
 * The PC/AT has two chained 8259A PICs:
 *   Master: handles IRQ0–7,  default vectors 0x08–0x0F (conflicts with CPU exceptions)
 *   Slave:  handles IRQ8–15, default vectors 0x70–0x77
 *
 * After remapping:
 *   Master → INT 0x20–0x27
 *   Slave  → INT 0x28–0x2F
 */
#ifndef _PIC_H_
#define _PIC_H_

#include <stdint.h>

#define PIC_MASTER_CMD (0x20)
#define PIC_MASTER_DATA (0x21)
#define PIC_SLAVE_CMD (0xA0)
#define PIC_SLAVE_DATA (0xA1)

#define ICW1_INIT (0x10) /* begin initialization sequence  */
#define ICW1_ICW4 (0x01) /* ICW4 will be sent              */
#define ICW4_8086 (0x01) /* 8086/88 mode (not MCS-80/85)   */

#define PIC_MASTER_VECTOR_BASE (0x20)
#define PIC_SLAVE_VECTOR_BASE (0x28)

#define PIC_EOI (0x20)

#define IRQ_TIMER (0)    /* master IRQ0 → INT 0x20 */
#define IRQ_KEYBOARD (1) /* master IRQ1 → INT 0x21 */
#define IRQ_CASCADE (2)  /* master IRQ2, wired to slave, never fires */
#define IRQ_COM2 (3)     /* master IRQ3 → INT 0x23 */
#define IRQ_COM1 (4)     /* master IRQ4 → INT 0x24 */
#define IRQ_LPT2 (5)     /* master IRQ5 → INT 0x25 */
#define IRQ_FLOPPY (6)   /* master IRQ6 → INT 0x26 */
#define IRQ_LPT1 (7)     /* master IRQ7 → INT 0x27 */
#define IRQ_RTC (8)      /* slave  IRQ8 → INT 0x28 */
#define IRQ_MOUSE (12)   /* slave  IRQ12 → INT 0x2C */
#define IRQ_FPU (13)     /* slave  IRQ13 → INT 0x2D */
#define IRQ_ATA1 (14)    /* slave  IRQ14 → INT 0x2E */
#define IRQ_ATA2 (15)    /* slave  IRQ15 → INT 0x2F */

void pic_remap(void);

void pic_send_eoi(uint8_t irq);

void pic_mask_irq(uint8_t irq);

void pic_unmask_irq(uint8_t irq);

void pic_mask_all(void);

#endif /* _PIC_H_ */
