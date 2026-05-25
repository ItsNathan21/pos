/**
* @file hardware_interrupts.S
* @author Nathan Porter
* @brief contains the hardware interrupt 
* stubs used within the kernel. 
*/


.macro HARDWARE_INTERRUPT name, wrapper_func 
    .global \name 
\name: 
    pushal 
    call \wrapper_func 
    popal 
    iret
.endm

# PIT timer interrupt, calls the C timer_wrapper 
HARDWARE_INTERRUPT timer_interrupt_main, timer_wrapper
# PS/2 keyboard interrupt, calls the C keyboard_wrapper
HARDWARE_INTERRUPT keyboard_interrupt_main, keyboard_wrapper
    