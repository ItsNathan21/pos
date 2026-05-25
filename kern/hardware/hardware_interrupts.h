/**
 * @file hardware_interrupts.h
 * @author your name (you@domain.com)
 * @brief Contains the declarations of the
 * hardware interrupt stubs used. Necessary since
 * they are implemented in assembly, and the symbols
 * are needed to be placed within the IDT.
 *
 */

/**
 * @brief the timer tickback function, the stub that
 * is called each time the timer fires.
 *
 */
void timer_interrupt_main(void);

/**
 * @brief the keyboard function called on each scancode
 * the processor receives.
 *
 */
void keyboard_interrupt_main(void);
