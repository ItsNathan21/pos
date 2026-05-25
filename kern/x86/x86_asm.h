/**
 * @file x86_asm.h
 * @author Nathan Porter
 * @brief Contains headers for assembly functions used by the
 * x86 architecture
 */

#ifndef _X86_ASM_H_
#define _X86_ASM_H_

#include <stdint.h>
#include "eflags.h"

/**
 * @brief handy x86 macro to get whether or not interrupts
 * are enabled
 *
 */
#define hardware_interrupts_enabled (get_eflags() & EFLAGS_IF)

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

/**
 * @brief enables all interrupts
 *
 */
void enable_interrupts(void);

/**
 * @brief disables all interrupts
 *
 */
void disable_interrupts(void);

/**
 * @brief Get the eflags register
 *
 * @return the contents of the eflags register
 */
uint32_t get_eflags(void);

/**
 * @brief stores the word into the eflags register.
 *
 * @note this should probably be used scarecely, triple
 * check theres no other option when using!
 *
 * @param new_eflags the new eflags to be stored
 */
void store_eflags(uint32_t new_eflags);

#endif /* _X86_ASM_H_ */
