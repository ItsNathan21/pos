/**
 * @file scancodes.h
 * @author Nathan Porter
 * @brief Contains types and macros for processing
 * PS/2 Set 1 scancodes into key events.
 *
 * A key_event_t packs everything about a single
 * keypress or keyrelease into one uint32_t:
 *
 *   bits [7:0]   ASCII character (0 if none)
 *   bit  [8]     1 = make (down), 0 = break (up)
 *   bit  [9]     1 = character field is valid
 *   bit  [10]    current shift state
 *   bit  [11]    current caps lock state
 *   bits [23:16] raw make scancode
 *
 */
#ifndef _SCANCODES_H_
#define _SCANCODES_H_

#include <stdint.h>

typedef uint32_t key_event_t;

/* --- key_event_t field positions and masks --- */

/** @brief mask to extract the ASCII character from a key event */
#define KEY_EVENT_CHAR_MASK (0xFF)
/** @brief bit position of the make/break flag */
#define KEY_EVENT_MAKE_BIT (8)
/** @brief bit position of the has-character flag */
#define KEY_EVENT_HAS_CHAR_BIT (9)
/** @brief bit position of the shift state flag */
#define KEY_EVENT_SHIFT_BIT (10)
/** @brief bit position of the caps lock state flag */
#define KEY_EVENT_CAPS_BIT (11)
/** @brief bit position of the raw scancode field */
#define KEY_EVENT_SCANCODE_SHIFT (16)
/** @brief mask to extract the raw scancode after shifting */
#define KEY_EVENT_SCANCODE_MASK (0xFF)

/* --- key_event_t accessor macros --- */

/** @brief extract the ASCII character from a key event (0 if none) */
#define KEY_EVENT_CHAR(e) ((char)((e) & KEY_EVENT_CHAR_MASK))

/** @brief true if the key was pressed down (make) */
#define KEY_EVENT_IS_MAKE(e) ((e) & (1 << KEY_EVENT_MAKE_BIT))

/** @brief true if the key was released (break) */
#define KEY_EVENT_IS_BREAK(e) (!((e) & (1 << KEY_EVENT_MAKE_BIT)))

/** @brief true if the character field contains a valid character */
#define KEY_EVENT_HAS_CHAR(e) ((e) & (1 << KEY_EVENT_HAS_CHAR_BIT))

/** @brief true if shift was held when this event was generated */
#define KEY_EVENT_SHIFT(e) ((e) & (1 << KEY_EVENT_SHIFT_BIT))

/** @brief true if caps lock was active when this event was generated */
#define KEY_EVENT_CAPS(e) ((e) & (1 << KEY_EVENT_CAPS_BIT))

/** @brief the raw make scancode (break bit stripped) */
#define KEY_EVENT_SCANCODE(e) (((e) >> KEY_EVENT_SCANCODE_SHIFT) & KEY_EVENT_SCANCODE_MASK)

/* --- PS/2 Set 1 scancode structure --- */

/** @brief number of entries in each scancode lookup table */
#define SCANCODE_TABLE_SIZE (128)

/** @brief bit set on any scancode to indicate a key release (break) */
#define SC_BREAK_BIT (0x80)

/** @brief mask to strip the break bit and recover the make code */
#define SC_MAKE_MASK (0x7F)

/* --- Set 1 make scancode identifiers for special keys --- */

/** @brief Escape */
#define SC_ESCAPE (0x01)
/** @brief Backspace */
#define SC_BACKSPACE (0x0E)
/** @brief Tab */
#define SC_TAB (0x0F)
/** @brief Enter */
#define SC_ENTER (0x1C)
/** @brief Left control */
#define SC_LCTRL (0x1D)
/** @brief Left shift */
#define SC_LSHIFT (0x2A)
/** @brief Right shift */
#define SC_RSHIFT (0x36)
/** @brief Left alt */
#define SC_LALT (0x38)
/** @brief Space */
#define SC_SPACE (0x39)
/** @brief Caps lock */
#define SC_CAPS (0x3A)

/**
 * @brief Processes a raw PS/2 Set 1 scancode into a key event.
 *
 * Tracks shift and caps lock state internally across calls.
 * Both make (key down) and break (key up) codes should be passed in.
 *
 * @param scancode raw byte from PS/2 port 0x60
 * @return key_event_t packed event; use KEY_EVENT_* macros to read it
 */
key_event_t scancode_process(uint8_t scancode);

#endif /* _SCANCODES_H_ */
