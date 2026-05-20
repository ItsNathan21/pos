/**
 * @file kprintf.h
 * @author Nathan Porter
 * @brief Kernel printf declarations
 *
 */
#ifndef _KPRINTF_H_
#define _KPRINTF_H_

#include <stdarg.h>

/**
 * @brief Kernel printf — formats and writes to the VGA console.
 *
 * @param fmt format string (see printf.c for supported specifiers)
 * @param ... format arguments
 * @return number of characters written
 */
int kprintf(const char *fmt, ...);

/**
 * @brief va_list variant of kprintf, for use when building
 * wrapper functions that accept variadic arguments themselves.
 *
 * @param fmt  format string
 * @param args argument list (caller must va_start / va_end)
 * @return number of characters written
 */
int kvprintf(const char *fmt, va_list args);

#endif /* _KPRINTF_H_ */
