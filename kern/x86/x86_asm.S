/**
* @file x86_asm.S 
* @author Nathan Porter 
* 
* @brief contains implementations of x86 specific functions 
* in assembly
*/


    .global outb 
outb: 
    movw 4(%esp), %dx # port uint16_t in %dx 
	movb 8(%esp), %al # byte in %al 
	out %al, %dx # send the byte to the port 
	ret

    .global inb 
inb: 
    xor %al, %al # zero out %al before placing byte in 
    movw 4(%esp), %dx # port uint16_t in %dx 
    in %dx, %al # byte in from port to %al 
    ret 

    .global enable_interrupts
enable_interrupts:
    sti 
    ret

    .global disable_interrupts 
disable_interrupts:
    cli 
    ret

    .global store_eflags
store_eflags:
    pushl %eax # push the new flags 
    popfl # pop them into eflags 
    ret

    .global get_eflags 
get_eflags: 
    pushfl # push the current flags 
    popl %eax # pop them into eax for return
    ret 

