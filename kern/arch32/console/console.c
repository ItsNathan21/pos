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

static inline vga_char_cell_t vga_cell(char c, uint8_t color)
{
    return (vga_char_cell_t){.c = c, .color = color};
}

static inline void put_char_at(char c, int row, int col)
{
    VGA_SCREEN[row][col] = vga_cell(c, vga_info.color);
}

static void shift_screen_down()
{
    static const int remaining_size =
        (CONSOLE_WIDTH * CONSOLE_HEIGHT) - (CONSOLE_WIDTH);

    static const void *second_row_addr =
        (void *)(VGA_BASE + (sizeof(vga_char_cell_t) * CONSOLE_WIDTH));

    memmove((void *)VGA_BASE, second_row_addr, remaining_size);

    for (int i = 0; i < CONSOLE_WIDTH; i++)
        put_char_at(' ', CONSOLE_HEIGHT - 1, i);
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
        if (vga_info.row == 0)
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
        if (++vga_info.row >= CONSOLE_HEIGHT)
            shift_screen_down();
    }
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

void console_set_cursor(int row, int col)
{
}

void console_get_cursor(int *row, int *col)
{
}

void console_hide_cursor(void) {}

void console_show_cursor(void) {}

char console_getchar(int row, int col)
{
    return -1;
}

void console_set_color(int color) {}

void clear_console(void) {}