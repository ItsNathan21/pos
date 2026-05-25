/**
 * @file keyboard.c
 * @author your name (you@domain.com)
 * @brief Contains the PS/2 functions dealing
 * with keyboard interrupts, and the implementations
 * of the keyboard interface.
 *
 */

#include "keyboard.h"

#include <pic.h>

/**
 * @brief interrupt wrapper for the keyboard, this has been
 *
 *
 */
void keyboard_wrapper(void)
{
    pic_send_eoi(IRQ_KEYBOARD);
}

char keyboard_getbyte(void)
{
    return '\0';
}

int keyboard_readline(char *buf, int buflen)
{
    return -1;
}
