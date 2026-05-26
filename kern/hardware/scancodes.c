/**
 * @file scancodes.c
 * @author Nathan Porter
 * @brief Implements PS/2 Set 1 scancode processing.
 *
 * BIOS/GRUB translates the keyboard's native Set 2 scancodes
 * to Set 1 before handing off to the kernel, so Set 1 is what
 * we see on port 0x60.
 *
 * Make codes:  0x01–0x58        (key down)
 * Break codes: make code | 0x80 (key up)
 *
 */
#include "scancodes.h"

/*
 * Unshifted ASCII for each Set 1 make code (indexed by scancode).
 * 0 means the key produces no character (modifier, function key, etc.).
 */
static const char scancode_ascii[SCANCODE_TABLE_SIZE] = {
    0,    /* 0x00 - undefined          */
    0x1B, /* 0x01 - Escape             */
    '1',  /* 0x02                      */
    '2',  /* 0x03                      */
    '3',  /* 0x04                      */
    '4',  /* 0x05                      */
    '5',  /* 0x06                      */
    '6',  /* 0x07                      */
    '7',  /* 0x08                      */
    '8',  /* 0x09                      */
    '9',  /* 0x0A                      */
    '0',  /* 0x0B                      */
    '-',  /* 0x0C                      */
    '=',  /* 0x0D                      */
    '\b', /* 0x0E - Backspace          */
    '\t', /* 0x0F - Tab                */
    'q',  /* 0x10                      */
    'w',  /* 0x11                      */
    'e',  /* 0x12                      */
    'r',  /* 0x13                      */
    't',  /* 0x14                      */
    'y',  /* 0x15                      */
    'u',  /* 0x16                      */
    'i',  /* 0x17                      */
    'o',  /* 0x18                      */
    'p',  /* 0x19                      */
    '[',  /* 0x1A                      */
    ']',  /* 0x1B                      */
    '\n', /* 0x1C - Enter              */
    0,    /* 0x1D - Left Ctrl          */
    'a',  /* 0x1E                      */
    's',  /* 0x1F                      */
    'd',  /* 0x20                      */
    'f',  /* 0x21                      */
    'g',  /* 0x22                      */
    'h',  /* 0x23                      */
    'j',  /* 0x24                      */
    'k',  /* 0x25                      */
    'l',  /* 0x26                      */
    ';',  /* 0x27                      */
    '\'', /* 0x28                      */
    '`',  /* 0x29                      */
    0,    /* 0x2A - Left Shift         */
    '\\', /* 0x2B                      */
    'z',  /* 0x2C                      */
    'x',  /* 0x2D                      */
    'c',  /* 0x2E                      */
    'v',  /* 0x2F                      */
    'b',  /* 0x30                      */
    'n',  /* 0x31                      */
    'm',  /* 0x32                      */
    ',',  /* 0x33                      */
    '.',  /* 0x34                      */
    '/',  /* 0x35                      */
    0,    /* 0x36 - Right Shift        */
    '*',  /* 0x37 - Keypad *           */
    0,    /* 0x38 - Left Alt           */
    ' ',  /* 0x39 - Space              */
    0,    /* 0x3A - Caps Lock          */
    0,    /* 0x3B - F1                 */
    0,    /* 0x3C - F2                 */
    0,    /* 0x3D - F3                 */
    0,    /* 0x3E - F4                 */
    0,    /* 0x3F - F5                 */
    0,    /* 0x40 - F6                 */
    0,    /* 0x41 - F7                 */
    0,    /* 0x42 - F8                 */
    0,    /* 0x43 - F9                 */
    0,    /* 0x44 - F10                */
    0,    /* 0x45 - Num Lock           */
    0,    /* 0x46 - Scroll Lock        */
    '7',  /* 0x47 - Keypad 7           */
    '8',  /* 0x48 - Keypad 8           */
    '9',  /* 0x49 - Keypad 9           */
    '-',  /* 0x4A - Keypad -           */
    '4',  /* 0x4B - Keypad 4           */
    '5',  /* 0x4C - Keypad 5           */
    '6',  /* 0x4D - Keypad 6           */
    '+',  /* 0x4E - Keypad +           */
    '1',  /* 0x4F - Keypad 1           */
    '2',  /* 0x50 - Keypad 2           */
    '3',  /* 0x51 - Keypad 3           */
    '0',  /* 0x52 - Keypad 0           */
    '.',  /* 0x53 - Keypad .           */
    0,    /* 0x54                      */
    0,    /* 0x55                      */
    0,    /* 0x56                      */
    0,    /* 0x57 - F11                */
    0,    /* 0x58 - F12                */
    /* 0x59–0x7F all undefined, zero-initialized */
};

/*
 * Shifted ASCII for each Set 1 make code.
 * Caps lock only affects letters; shift affects everything.
 * This table is for shift held (caps lock handled separately).
 */
static const char scancode_ascii_shift[SCANCODE_TABLE_SIZE] = {
    0,    /* 0x00 - undefined          */
    0x1B, /* 0x01 - Escape             */
    '!',  /* 0x02                      */
    '@',  /* 0x03                      */
    '#',  /* 0x04                      */
    '$',  /* 0x05                      */
    '%',  /* 0x06                      */
    '^',  /* 0x07                      */
    '&',  /* 0x08                      */
    '*',  /* 0x09                      */
    '(',  /* 0x0A                      */
    ')',  /* 0x0B                      */
    '_',  /* 0x0C                      */
    '+',  /* 0x0D                      */
    '\b', /* 0x0E - Backspace          */
    '\t', /* 0x0F - Tab                */
    'Q',  /* 0x10                      */
    'W',  /* 0x11                      */
    'E',  /* 0x12                      */
    'R',  /* 0x13                      */
    'T',  /* 0x14                      */
    'Y',  /* 0x15                      */
    'U',  /* 0x16                      */
    'I',  /* 0x17                      */
    'O',  /* 0x18                      */
    'P',  /* 0x19                      */
    '{',  /* 0x1A                      */
    '}',  /* 0x1B                      */
    '\n', /* 0x1C - Enter              */
    0,    /* 0x1D - Left Ctrl          */
    'A',  /* 0x1E                      */
    'S',  /* 0x1F                      */
    'D',  /* 0x20                      */
    'F',  /* 0x21                      */
    'G',  /* 0x22                      */
    'H',  /* 0x23                      */
    'J',  /* 0x24                      */
    'K',  /* 0x25                      */
    'L',  /* 0x26                      */
    ':',  /* 0x27                      */
    '"',  /* 0x28                      */
    '~',  /* 0x29                      */
    0,    /* 0x2A - Left Shift         */
    '|',  /* 0x2B                      */
    'Z',  /* 0x2C                      */
    'X',  /* 0x2D                      */
    'C',  /* 0x2E                      */
    'V',  /* 0x2F                      */
    'B',  /* 0x30                      */
    'N',  /* 0x31                      */
    'M',  /* 0x32                      */
    '<',  /* 0x33                      */
    '>',  /* 0x34                      */
    '?',  /* 0x35                      */
    0,    /* 0x36 - Right Shift        */
    '*',  /* 0x37 - Keypad *           */
    0,    /* 0x38 - Left Alt           */
    ' ',  /* 0x39 - Space              */
    0,    /* 0x3A - Caps Lock          */
    0,    /* 0x3B - F1                 */
    0,    /* 0x3C - F2                 */
    0,    /* 0x3D - F3                 */
    0,    /* 0x3E - F4                 */
    0,    /* 0x3F - F5                 */
    0,    /* 0x40 - F6                 */
    0,    /* 0x41 - F7                 */
    0,    /* 0x42 - F8                 */
    0,    /* 0x43 - F9                 */
    0,    /* 0x44 - F10                */
    0,    /* 0x45 - Num Lock           */
    0,    /* 0x46 - Scroll Lock        */
    '7',  /* 0x47 - Keypad 7           */
    '8',  /* 0x48 - Keypad 8           */
    '9',  /* 0x49 - Keypad 9           */
    '-',  /* 0x4A - Keypad -           */
    '4',  /* 0x4B - Keypad 4           */
    '5',  /* 0x4C - Keypad 5           */
    '6',  /* 0x4D - Keypad 6           */
    '+',  /* 0x4E - Keypad +           */
    '1',  /* 0x4F - Keypad 1           */
    '2',  /* 0x50 - Keypad 2           */
    '3',  /* 0x51 - Keypad 3           */
    '0',  /* 0x52 - Keypad 0           */
    '.',  /* 0x53 - Keypad .           */
    0,    /* 0x54                      */
    0,    /* 0x55                      */
    0,    /* 0x56                      */
    0,    /* 0x57 - F11                */
    0,    /* 0x58 - F12                */
};

key_event_t scancode_process(uint8_t scancode)
{
    static int shift_active = 0;
    static int caps_active = 0;

    int is_break = (scancode & SC_BREAK_BIT) != 0;
    uint8_t make_code = scancode & SC_MAKE_MASK;

    /* update modifier state */
    if (make_code == SC_LSHIFT || make_code == SC_RSHIFT)
        shift_active = !is_break;

    if (make_code == SC_CAPS && !is_break)
        caps_active = !caps_active;

    /*
     * Character selection:
     *   shift only      → shifted table
     *   caps only       → base table, letters uppercased
     *   shift + caps    → shifted table, letters lowercased (they cancel)
     *   neither         → base table
     */
    char c;
    if (shift_active)
    {
        c = scancode_ascii_shift[make_code];
        if (caps_active && c >= 'A' && c <= 'Z')
            c = c - 'A' + 'a';
    }
    else
    {
        c = scancode_ascii[make_code];
        if (caps_active && c >= 'a' && c <= 'z')
            c = c - 'a' + 'A';
    }

    key_event_t event = 0;
    event |= (uint8_t)c;
    if (!is_break)
        event |= (1 << KEY_EVENT_MAKE_BIT);
    if (c != 0)
        event |= (1 << KEY_EVENT_HAS_CHAR_BIT);
    if (shift_active)
        event |= (1 << KEY_EVENT_SHIFT_BIT);
    if (caps_active)
        event |= (1 << KEY_EVENT_CAPS_BIT);
    event |= ((uint32_t)make_code << KEY_EVENT_SCANCODE_SHIFT);

    return event;
}
