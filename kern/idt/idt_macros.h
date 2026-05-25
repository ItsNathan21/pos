/**
 * @file idt_macros.h
 * @author Nathan Porter
 * @brief Contains macros to be used for the IDT, which
 * can also be used in .S files (hence the existence
 * of this file)
 *
 */
#ifndef _IDT_MACROS_H_
#define _IDT_MACROS_H_

/** @brief number of entries in the IDT (256 for IA32) */
#define IDT_ENTRIES (0x100)

#define IDT_ENTRY_PIT (0x20)
#define IDT_ENTRY_PS2_KBD (0x21)

#endif /* _IDT_MACROS_H_ */
