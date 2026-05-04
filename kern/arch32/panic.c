/**
 * @file panic.c
 * @author Nathan Porter
 * @brief contains the implementation for the kernel's panic
 * function, to be called when there is no possible recovery for
 * the kernel once a given situation has arised.
 *
 */

#include <console.h>
#include <kprintf.h>

/**
 * @brief The kernel's panic function, called when there is
 * no recoverable situation for the kernel. This will
 * display the message on the console, and loop forever, for the
 * writer to debug or user to report. Hopefully the user never sees
 * this, but mistakes happen.
 *
 * @param s the string to print onto the console, ideally a useful message
 * to show to the programmer or user.
 */
void panic(const char *fmt, ...)
{

    va_list args;
    va_start(args, fmt);
    kvprintf(fmt, args);
    va_end(args);

    while (1)
        continue;
}
