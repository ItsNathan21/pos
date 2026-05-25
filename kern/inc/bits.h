/**
 * @file bits.h
 * @author Nathan Porter
 * @brief Contains macros for bit manipulations
 * and retrievals for values
 *
 */

#ifndef _BITS_H_
#define _BITS_H_

/**
 * @def expands to the N'th byte of X
 */
#define BYTE(X, N) (((X) >> (8 * N)) & 0xFF)

/**
 * @def Gets the N'th bit of X
 *
 */
#define GETBIT(X, N) (((X) & (1u << (N))))

/**
 * @def gets the N'th 16 bit word from the number X
 *
 */
#define GET_16BIT_WORD(X, N) ((uint16_t)(((X) >> ((N) * 16)) & 0xffff))

#endif /* _BITS_H_ */
