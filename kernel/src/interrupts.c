#include "include/terminal.h"
#include <stdint.h>

void kbd_interrupt_c(__attribute__((unused)) int8_t data) {
    terminal_writestring("here!\n");
}