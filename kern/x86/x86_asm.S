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

