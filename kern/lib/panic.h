/**
 * @file panic.h
 * @author Nathan Porter
 * @brief contains the definition for the kernels
 * panic function, as well as helper macro's for
 * assertions within the kernel, that should not fail.
 *
 */

#ifndef _PANIC_H_
#define _PANIC_H_

#define assert(exp)                                                          \
    do                                                                       \
    {                                                                        \
        if (!(exp))                                                          \
            panic("assert failed! In file %s, on line %d in function %s\n"); \
    } while (0)

void panic(const char *fmt, ...);

#endif /* _PANIC_H_ */
