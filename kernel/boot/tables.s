

.section .data

.global tss
.global end_tss
tss:
    .space 104
end_tss:


.set KERNEL_CODE_BASE,   0x0000000000000000
.set KERNEL_CODE_LIMIT,  0x000F00000000FFFF
.set KERNEL_CODE_ACCESS, 0x00009A0000000000
.set KERNEL_CODE_FLAGS,  0x00C0000000000000
.set KERNEL_CODE, (KERNEL_CODE_BASE | KERNEL_CODE_LIMIT | KERNEL_CODE_ACCESS | KERNEL_CODE_FLAGS)


.set KERNEL_DATA_BASE,   0x0000000000000000
.set KERNEL_DATA_LIMIT,  0x000F00000000FFFF
.set KERNEL_DATA_ACCESS, 0x0000920000000000
.set KERNEL_DATA_FLAGS,  0x00C0000000000000
.set KERNEL_DATA, (KERNEL_DATA_ACCESS | KERNEL_DATA_BASE | KERNEL_DATA_LIMIT | KERNEL_DATA_FLAGS)

.set USER_CODE_BASE,   0x0000000000000000
.set USER_CODE_LIMIT,  0x000F00000000FFFF
.set USER_CODE_ACCESS, 0x0000FA0000000000
.set USER_CODE_FLAGS,  0x00C0000000000000
.set USER_CODE, (USER_CODE_BASE | USER_CODE_LIMIT | USER_CODE_ACCESS | USER_CODE_FLAGS)

.set USER_DATA_BASE,   0x0000000000000000
.set USER_DATA_LIMIT,  0x000F00000000FFFF
.set USER_DATA_ACCESS, 0x0000F20000000000
.set USER_DATA_FLAGS,  0x00C0000000000000
.set USER_DATA, (USER_DATA_BASE | USER_DATA_LIMIT | USER_DATA_ACCESS | USER_DATA_FLAGS)

.global gdt_start
gdt_start:
    .quad 0x0000000000000000
    .quad KERNEL_CODE
    .quad KERNEL_DATA
    .quad USER_CODE
    .quad USER_DATA
    .quad 0x0000000000000000

gdtr:
    .word 0 
    .long 0

.global load_gdt
load_gdt:
    mov  4(%esp), %eax 
    mov  %eax, gdtr        
    mov  8(%esp), %eax       
    mov  %eax, gdtr+2         
    lgdt gdtr
    ret

idtr:
    .word 0
    .long 0

.global load_idt
load_idt:
    mov  4(%esp), %eax 
    mov  %eax, idtr        
    mov  8(%esp), %eax       
    mov  %eax, idtr+2         
    lidt idtr
    ret

.global idt
.global end_idt
idt:
    
end_idt:


.section .text 

.extern kbd_interrupt_c

.macro IDT_ENTRY handler
    .word \handler & 0xFFFF       # offset low
    .word 0x08                    # selector
    .byte 0
    .byte 0x8E                    # flags
    .word (\handler >> 16) & 0xFFFF  # offset high
.endm

.type kbd_interrupt, @function
kbd_interrupt:
    pushl %eax              # save eax

    inb   $0x60, %al        # read from port 0x60 into AL
    call kbd_interrupt_c
    movb  $0x20, %al        # load EOI (0x20) into AL
    outb  %al, $0x20        # send EOI to PIC (port 0x20)

    popl  %eax              # restore eax
    iret                    # return from interrupt
