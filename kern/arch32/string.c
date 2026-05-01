/**
 * @file string.c
 * @author Nathan Porter
 * @brief Contains implementations for the libc style
 * string functions
 *
 */

#include <stdint.h>
#include <stddef.h>

/**
 * @brief sets n bytes of s to c
 *
 * @param s destination
 * @param c character to set
 * @param n the number of characters
 * @return void*
 */
void *memset(void *s, int c, size_t n)
{
    uint8_t *p = s;
    while (n--)
        *p++ = (uint8_t)c;
    return s;
}

void *memcpy(void *dest, const void *src, size_t n)
{
    uint8_t *d = dest;
    const uint8_t *s = src;
    while (n--)
        *d++ = *s++;
    return dest;
}

int memcmp(const void *s1, const void *s2, size_t n)
{
    const uint8_t *a = s1, *b = s2;
    while (n--)
        if (*a++ != *b++)
            return a[-1] - b[-1];
    return 0;
}

size_t strlen(const char *s)
{
    size_t n = 0;
    while (*s++)
        n++;
    return n;
}

int strcmp(const char *a, const char *b)
{
    while (*a && *a == *b)
    {
        a++;
        b++;
    }
    return (unsigned char)*a - (unsigned char)*b;
}

char *strcpy(char *dest, const char *src)
{
    char *d = dest;
    while ((*d++ = *src++))
        ;
    return dest;
}

void *memmove(void *dest, const void *src, size_t n)
{
    unsigned char *d = (unsigned char *)dest;
    const unsigned char *s = (const unsigned char *)src;

    if (d == s || n == 0)
    {
        return dest;
    }

    if (d < s)
    {
        for (size_t i = 0; i < n; i++)
        {
            d[i] = s[i];
        }
    }

    else
    {
        for (size_t i = n; i > 0; i--)
        {
            d[i - 1] = s[i - 1];
        }
    }

    return dest;
}
