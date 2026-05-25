/**
 * @file segment.S
 * @author Nathan Porter
 * @brief Assembly implementations of load_gdt, load_tss,
 * and set_esp0.
 */
 
.section .text
 
/*
 * void load_gdt(gdtr_t *gdtr, uint16_t cs, uint16_t ds)
 *
 * cdecl stack layout on entry:
 *   4(%esp)  = gdtr pointer
 *   8(%esp)  = cs selector
 *   12(%esp) = ds selector
 *
 * Reloads all data segment registers from ds, then does a
 * far return to flush the prefetch queue and reload CS.
 */
.global load_gdt
load_gdt:
    movl  4(%esp), %eax
    lgdt  (%eax)
 
    movl  12(%esp), %eax
    movw  %ax, %ds
    movw  %ax, %es
    movw  %ax, %fs
    movw  %ax, %gs
    movw  %ax, %ss
 
    /*
     * Far return trick: push cs:addr, lret jumps there
     * with the new CS loaded into the pipeline.
     *
     * lret in 32-bit mode pops EIP then CS, so push in
     * reverse order: CS first, then the target address.
     */
    movl  8(%esp), %eax
    pushl %eax
    pushl $.Lcs_flush
    lret
.Lcs_flush:
    ret
 
 
/*
 * void load_tss(uint16_t selector)
 *
 * cdecl stack layout on entry:
 *   4(%esp) = TSS selector (index << 3)
 */
.global load_tss
load_tss:
    movl  4(%esp), %eax
    ltr   %ax
    ret
 
 
/*
 * void set_esp0(uint32_t esp0)
 *
 * Writes directly into TSS.esp0 (offset 4 from TSS base).
 * TSS is a non-static symbol exported from gdt.c.
 *
 * cdecl stack layout on entry:
 *   4(%esp) = new esp0 value
 */
.extern TSS
.global set_esp0
set_esp0:
    movl  4(%esp), %eax
    movl  %eax, TSS+4
    ret
