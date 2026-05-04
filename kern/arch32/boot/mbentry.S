/**
* @file mbentry.S 
*
* @brief Contains the GRUB multiboot entry point for the 
* kernel, sets up the kernel's stack, passess off control 
*
* @author Nathan Porter 
*
*/

#include <mbinfo_header_fields.h>

/*
    Kernel's initial stack is 16KB, for initialization 
    of all the hardware, and remaining system.
*/
#define KERNEL_INITIAL_STACK_SIZE (16384)


#define HEADER (ALIGN_4K | GET_MEM)
#define MAGIC (0x1BADB002)
#define CHECKSUM (-(MAGIC + HEADER))

.section .multiboot
.align 4
.long MAGIC 
.long HEADER
.long CHECKSUM

.section .bss
.align 16
_k_init_stack_bottom:
    .skip 16384 # fill 16KB with zereos
_k_init_stack_top:

    .section .text
    .global mbentry 
mbentry:

    movl %eax, %esi # store the magic number multiboot gave us 
                    # for proper boot checking, to use as soon as we're ready

    movl $_k_init_stack_top, %esp

    /* zero .bss — symbols provided by linker script */
    
    movl $_bss_start, %edi
    movl $_bss_end,   %ecx
    subl %edi, %ecx          /* ECX = size in bytes */
    xorl %eax, %eax
    cld
    rep  stosb               /* zero fill */

    push %ebx # multiboot information structure pointer 
    push %esi # magic_number
    call kernel_setup
    hlt
    jmp  .
    

.size mbentry, . - mbentry