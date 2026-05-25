/**
 * @file segment_asm.h
 * @author Nathan Porter
 * @brief Declarations for the assembly routines that
 * load the GDT/TSS and update the kernel stack pointer.
 */
#ifndef _SEGMENT_ASM_H_
#define _SEGMENT_ASM_H_

#include <stdint.h>

/**
 * @brief layout of the 6-byte descriptor the CPU
 * reads when lgdt is executed
 */
struct gdtr
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));
typedef struct gdtr gdtr_t;

/**
 * @brief Executes lgdt from the given descriptor, then
 * reloads all data segment registers with ds and flushes
 * the pipeline with a far return into cs.
 *
 * @param gdtr pointer to the GDTR descriptor to load
 * @param cs   code segment selector to far-return into
 * @param ds   data segment selector for ds/es/fs/gs/ss
 */
void load_gdt(gdtr_t *gdtr, uint16_t cs, uint16_t ds);

/**
 * @brief Executes ltr to load the task register
 * with the given TSS selector.
 *
 * @param selector TSS segment selector (index << 3)
 */
void load_tss(uint16_t selector);

/**
 * @brief Updates TSS.esp0 — the kernel stack pointer
 * the CPU switches to on ring-3 -> ring-0 transitions.
 * Call this on every context switch.
 *
 * @param esp0 new kernel stack pointer
 */
void set_esp0(uint32_t esp0);

#endif /* _SEGMENT_ASM_H_ */
