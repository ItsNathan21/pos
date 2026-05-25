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

/*
    Affirmations are used for debugging purposes, but can be
    taken away without the debug flag
*/
#ifdef DEBUG

#define affirm(exp)                                                              \
    do                                                                           \
    {                                                                            \
        if (!(exp))                                                              \
            panic("affirmation failed! In file %s, on line %d in function %s\n", \
                  __FILE__, __LINE__, __func__);                                 \
    } while (0)

#else

#define affirm(exp) (void(0))

#endif /* DEBUG */

/*
    Assertions are *always* checked, even in the kernel implemented
    in real hardware. If they fail, the kernel will panic. Assertions
    should only be called in unrecoverable kernel situations.
*/
#define assert(exp)                                                         \
    do                                                                      \
    {                                                                       \
        if (!(exp))                                                         \
            panic("assert failed! In file %s, on line %d in function %s\n", \
                  __FILE__, __LINE__, __func__);                            \
    } while (0)

void panic(const char *fmt, ...);

#endif /* _PANIC_H_ */
