/**
 * @file keyboard.h
 * @author your name (you@domain.com)
 * @brief Contains macros and declarations of the
 * PS/2 keyboard interface used for the kernel.
 *
 */

#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

/** @brief the port to communicte with the PS/2 controller */
#define PS2_KBD_PORT (0x60)

char keyboard_getbyte(void);

int keyboard_readline(char *buf, int buflen);

#endif /* _KEYBOARD_H_ */
