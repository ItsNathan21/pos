/**
 * @file timer.c
 * @author your name (you@domain.com)
 * @brief Contains the C function that the
 * timer assembly handler calls, which
 * is a wrapper around the "installed" timer
 * function.
 *
 * This feels like a lot of layers of indirection
 *
 * asm stub -> timer_wrapper -> timer function
 *
 * but it allows the function to be called by each
 * timer interrupt to change dynamically (safely, with
 * interrupts disabled of course).
 *
 * It also allows the timer to be active and running when the
 * kernel does not want a timer interrupt (in the early stages),
 * so interrupts can be enabled without an official "timer function"
 * being called.
 *
 */

#include "timer.h"

#include <stddef.h>
#include <pic.h>

static void (*timer_tickback)(void) = NULL;

void timer_install_tickback(void (*tickback)(void))
{
    timer_tickback = tickback;
}

/**
 * @brief wrapper function for the timer interrupt.
 * Forwards the interrupt to the user's specified interrupt,
 * if they have one.
 *
 *
 *
 */
void timer_wrapper(void)
{
    pic_send_eoi(IRQ_TIMER);

    if (timer_tickback != NULL)
        timer_tickback();
}