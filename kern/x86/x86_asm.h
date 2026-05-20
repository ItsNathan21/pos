/**
 * @file x86_asm.h
 * @author Nathan Porter
 * @brief Contains headers for assembly functions used by the
 * x86 architecture
 */

#ifndef _X86_ASM_H_
#define _X86_ASM_H_

#include <stdint.h>

/**
 * @brief Sends the data byte data to the
 * port
 *
 * @param port the 16 bit port to send to
 * @param data the data to send
 */
void outb(uint16_t port, uint8_t data);

/**
 * @brief Reads a byte from the specified port
 *
 * @param port the port to read from
 * @return the byte returned by the port
 */
uint8_t inb(uint16_t port);

#endif /* _X86_ASM_H_ */
