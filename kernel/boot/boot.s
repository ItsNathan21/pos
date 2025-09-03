.set ALIGN,     1 << 0
.set MEMINFO,   1 << 1
.set FLAGS,     ALIGN | MEMINFO 
.set MAGIC,     0x1BADB002
.set CHECKSUM, -(MAGIC + FLAGS)

.section .multiboot
.align 4
.long MAGIC 
.long FLAGS
.long CHECKSUM

.section .bss
.align 16
_stack_bottom:
.skip 16384
_stack_top:

.section .text
.global _start 
.type _start, @function 
_start:

    push %ebx
    push %eax
    call kmain

    hlt 
    jmp .

.size _start, . - _start


