/**
 * @file string.h
 * @author Nathan Porter
 * @brief Contains string functions libc style to use
 * within the kernel
 *
 */

#ifndef _STRING_H_
#define _STRING_H_

#include <stdint.h>
#include <stddef.h>

void *memset(void *s, int c, size_t n);

void *memcpy(void *dest, const void *src, size_t n);

int memcmp(const void *s1, const void *s2, size_t n);

size_t strlen(const char *s);

int strcmp(const char *a, const char *b);

char *strcpy(char *dest, const char *src);

void *memmove(void *dest, const void *src, size_t n);

#endif /* _STRING_H_ */