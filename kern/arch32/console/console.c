/**
 * @file console.c
 *
 * @author Nathan Porter
 *
 * @brief Contains implementations for the VGA
 * text buffer console used in x86
 *
 * @note Logical cursor is mentioned alot. This is
 * where the cursor *should* be, even though it might
 * not be appearing on screen, due to relevant function
 * calls.
 *
 */

#include <console.h>
#include <stdint.h>
#include <string.h>
#include <x86_asm.h>
#include <bits.h>

/**
 * @brief the structure of each VGA cell for each
 * character, contains the character itself
 * along with its specified color.
 *
 */
typedef struct
{
    char c;
    uint8_t color;
} vga_char_cell_t;

/**
 * @brief the memory layout of the VGA screen, being a
 *  double array of height x width of each VGA cell
 */
typedef vga_char_cell_t vga_screen_t[CONSOLE_HEIGHT][CONSOLE_WIDTH];

/** @brief the base of the VGA text buffer in the memory map */
#define VGA_BASE (0xB8000)

/** @brief macro to use for reading/writing to the VGA screen  */
#define VGA_SCREEN (*(volatile vga_screen_t *)VGA_BASE)

/**
 * @brief the information structure used
 * internally to track logical cursor information,
 * whether or not to update the cursor visibly,
 * and more.
 *
 */
typedef struct
{
    int row, col,
        cursor_visible,
        color;
} console_info_t;

// local info struct to use, initialized
static console_info_t vga_info =
    {
        .row = 0,
        .col = 0,
        .cursor_visible = 1,
        .color = FGND_WHITE | BGND_BLACK,
};

/**
 * @def Macros for the CRTC cursor to be
 * used to set the consoles physical cursor
 *
 */
#define CRTC_IDX_PORT (0x3D4)
#define CRTC_DATA_PORT (0x3D5)
#define CRTC_IDX_CURSOR_LOW (0xF)
#define CRTC_IDX_CURSOR_HIGH (0xE)
#define CRTC_SCANLINE_START (0xA)
#define CRTC_SCANLINE_END (0xB)
#define CRTC_CURSOR_DISABLED (0x20)

/**
 * @brief given a character and color, returns a vga cell
 * corresponding to that color and character, to write to the
 * VGA text screen presumably.
 *
 * @param c the character to pack
 * @param color the color to write the character as
 * @return the vga cell to write
 */
static inline vga_char_cell_t vga_cell(char c, uint8_t color)
{
    return (vga_char_cell_t){.c = c, .color = color};
}

/**
 * @brief Places character c at the location specified with the
 * vga consols internal color. Does not update internal row/column
 *
 * @param c the character to write
 * @param row the row to write the character at
 * @param col the column to write the character at
 */
static inline void put_char_at(char c, int row, int col)
{
    VGA_SCREEN[row][col] = vga_cell(c, vga_info.color);
}

/**
 * @brief shifts the VGA screen down, blanking the bottom
 * row for new input, losing the text at the top of the
 * screen.
 *
 */
static void shift_screen_down()
{
    static const int remaining_size =
        ((CONSOLE_WIDTH * CONSOLE_HEIGHT) - CONSOLE_WIDTH) * sizeof(vga_char_cell_t);

    static const void *second_row_addr =
        (void *)(VGA_BASE + (sizeof(vga_char_cell_t) * CONSOLE_WIDTH));

    memmove((void *)VGA_BASE, second_row_addr, remaining_size);

    for (int i = 0; i < CONSOLE_WIDTH; i++)
        put_char_at(' ', CONSOLE_HEIGHT - 1, i);
}

static void update_physical_cursor(int row, int col)
{
    if (vga_info.cursor_visible == 0)
        return;

    uint16_t cursor_pos = (row * CONSOLE_WIDTH) + col;
    // lower half of position, send it
    outb(CRTC_IDX_PORT, CRTC_IDX_CURSOR_LOW);
    outb(CRTC_DATA_PORT, BYTE(cursor_pos, 0));

    // send the upper half of the position
    outb(CRTC_IDX_PORT, CRTC_IDX_CURSOR_HIGH);
    outb(CRTC_DATA_PORT, BYTE(cursor_pos, 1));
}

/**
 * @brief puts the desired character on the
 * console at the current logical cursors position.
 *
 * If a newline '\n' is encountered, the console will increment to the
 * next row. If a return '\r' is encountered the cursor (logical
 * and physical if active) will return to the first column
 * within the current row. If a backspace is encounterd, it will
 * delete the character behind the cursor, and update the cursor
 * (logical and physical if active), at most to the start of the
 * current row.
 *
 * @param c
 */
void console_putc(char c)
{
    switch (c)
    {
    case '\n':
    {
        vga_info.row++;
        vga_info.col = 0;
        break;
    }
    case '\r':
    {
        vga_info.col = 0;
        break;
    }
    case '\b':
    {
        if (vga_info.col == 0)
            return;

        put_char_at(' ', vga_info.row, --vga_info.col);
        break;
    }
    default:
    {
        put_char_at(c, vga_info.row, vga_info.col++);
        break;
    }
    }

    if (vga_info.col >= CONSOLE_WIDTH)
    {
        vga_info.col = 0;
        vga_info.row++;
    }

    if (vga_info.row >= CONSOLE_HEIGHT)
    {
        vga_info.row = CONSOLE_HEIGHT - 1;
        shift_screen_down();
    }

    update_physical_cursor(vga_info.row, vga_info.col);
}

/**
 * @brief Prints a null-terminated C style string onto the
 * VGA console.
 *
 * @param s the string to print
 */
void console_puts(char *s)
{
    while (*s)
        console_putc(*s++);
}

/**
 * @brief Sets the VGA cursor to the specified value.
 * This is the logical cursor, and not the hardware value.
 *
 * @param row the new logical cursors row
 * @param colthe new logical cursors column
 */
void console_set_cursor(int row, int col)
{
    vga_info.row = row;
    vga_info.col = col;
}

/**
 * @brief Sets the values of the current VGA logical
 * cursor.
 *
 * @param row A pointer which will be set to the consoles
 * row value after return
 * @param col A pointer which will be set to the consoles
 * column value after return
 */
void console_get_cursor(int *row, int *col)
{
    *row = vga_info.row;
    *col = vga_info.col;
}

/**
 * @brief Disables the VGA cursor so it is no
 * longer present on the screen.
 *
 */
void console_hide_cursor(void)
{
    outb(CRTC_IDX_PORT, CRTC_SCANLINE_START);
    outb(CRTC_DATA_PORT, CRTC_CURSOR_DISABLED);

    vga_info.cursor_visible = 0;
}

/**
 * @brief Enables and displays the VGA hardware cursor
 *
 */
void console_show_cursor(void)
{
    /*
        Enable the hardware cursor with
        the cursor start scanline and end
        scanline registers.
    */
    outb(CRTC_IDX_PORT, CRTC_SCANLINE_START);
    outb(CRTC_DATA_PORT, 0x00);

    outb(CRTC_IDX_PORT, CRTC_SCANLINE_END);
    outb(CRTC_DATA_PORT, 0x0F);

    vga_info.cursor_visible = 1;

    update_physical_cursor(vga_info.row, vga_info.col);
}

/**
 * @brief returns the character placed at the VGA screen
 * at the given row and column
 *
 * @param row the row to retrieve from
 * @param col the column to retrieve from
 * @return the character displayed at the row
 */
char console_getchar(int row, int col)
{
    vga_char_cell_t *cell =
        (vga_char_cell_t *)(VGA_BASE + ((row * CONSOLE_WIDTH) + col) * sizeof(vga_char_cell_t));

    return cell->c;
}

/**
 * @brief Sets the color (both foreground and background) of all
 * subsequent calls to console_putc and console_puts to the
 * secified color, until another call is made to this function
 * to change it.
 *
 * @param color the new color for future written characters
 */
void console_set_color(int color)
{
    vga_info.color = color;
}

void clear_console(void)
{
    for (int i = 0; i < CONSOLE_HEIGHT; i++)
        for (int j = 0; j < CONSOLE_WIDTH; j++)
            put_char_at(' ', i, j);

    vga_info.col = 0;
    vga_info.row = 0;
}