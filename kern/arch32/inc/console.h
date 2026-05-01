/**
 * @file console.h
 * @author Nathan Porter
 * @brief Contains functions used for the memory
 * mapped VGA console, to be used by the remainder of the
 * kernel.
 *
 * @note for users of these functions, the kernels typical
 * failure is to call the panic function defined in
 * panic.c, which prints text out to the VGA console. In the
 * case of the console itself, any failure codes returned by
 * functions absolutely must be checked, as there is no
 * failure printed to the screen and panic.
 *
 */

#define CONSOLE_WIDTH (80)
#define CONSOLE_HEIGHT (25)

#define FGND_BLACK 0x0
#define FGND_BLUE 0x1
#define FGND_GREEN 0x2
#define FGND_CYAN 0x3
#define FGND_RED 0x4
#define FGND_MAG 0x5
#define FGND_BRWN 0x6
#define FGND_LGRAY 0x7 /* Light gray. */
#define FGND_DGRAY 0x8 /* Dark gray. */
#define FGND_BBLUE 0x9 /* Bright blue. */
#define FGND_BGRN 0xA  /* Bright green. */
#define FGND_BCYAN 0xB /* Bright cyan. */
#define FGND_PINK 0xC
#define FGND_BMAG 0xD /* Bright magenta. */
#define FGND_YLLW 0xE
#define FGND_WHITE 0xF

#define BGND_BLACK 0x00
#define BGND_BLUE 0x10
#define BGND_GREEN 0x20
#define BGND_CYAN 0x30
#define BGND_RED 0x40
#define BGND_MAG 0x50
#define BGND_BRWN 0x60
#define BGND_LGRAY 0x70 /* Light gray. */

#define BLINK 0x80 /* Annoying. */

void console_putc(char c);

void console_puts(char *s);

void console_set_cursor(int row, int col);

void console_get_cursor(int *row, int *col);

void console_hide_cursor(void);

void console_show_cursor(void);

char console_getchar(int row, int col);

void console_set_color(int color);

void clear_console(void);
