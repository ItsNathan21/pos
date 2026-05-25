/**
 * @file idt.S
 * @author Nathan Porter
 * @brief Contains helper functions for 
 * loading and getting the base of the IDT 
 */

#include "idt_macros.h"

    .global __store_idtr
__store_idtr:
    movl 0x4(%esp), %eax # move new idt pointer 
    lidt (%eax) # store the contents of eax into the idt 
    ret 

    .global __get_idtr 
__get_idtr: 
    movl 0x4(%esp), %eax # move pointer into eax 
    sidt (%eax) # store contents of idt at the eax pointer
    ret 
