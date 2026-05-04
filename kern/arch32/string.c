/**
 * @file string.c
 * @author Nathan Porter
 * @brief Contains implementations for the libc style
 * string functions
 *
 */
#include <stdint.h>
#include <stddef.h>

/* =========================================================================
 * Memory functions
 * ========================================================================= */

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

/**
 * @brief copies n bytes from src to dest. Regions must not overlap;
 * use memmove if they might.
 */
void *memcpy(void *dest, const void *src, size_t n)
{
    uint8_t *d = dest;
    const uint8_t *s = src;
    while (n--)
        *d++ = *s++;
    return dest;
}

/**
 * @brief copies n bytes from src to dest, handling overlapping regions
 * correctly.
 */
void *memmove(void *dest, const void *src, size_t n)
{
    unsigned char *d = (unsigned char *)dest;
    const unsigned char *s = (const unsigned char *)src;
    if (d == s || n == 0)
        return dest;
    if (d < s)
    {
        for (size_t i = 0; i < n; i++)
            d[i] = s[i];
    }
    else
    {
        for (size_t i = n; i > 0; i--)
            d[i - 1] = s[i - 1];
    }
    return dest;
}

/**
 * @brief compares n bytes of s1 and s2.
 *
 * @return 0 if equal, <0 if s1 < s2, >0 if s1 > s2
 */
int memcmp(const void *s1, const void *s2, size_t n)
{
    const uint8_t *a = s1, *b = s2;
    while (n--)
        if (*a++ != *b++)
            return a[-1] - b[-1];
    return 0;
}

/**
 * @brief searches n bytes of s for byte c.
 *
 * @return pointer to first occurrence of c in s, or NULL if not found
 */
void *memchr(const void *s, int c, size_t n)
{
    const uint8_t *p = s;
    while (n--)
    {
        if (*p == (uint8_t)c)
            return (void *)p;
        p++;
    }
    return NULL;
}

/* =========================================================================
 * String length / comparison
 * ========================================================================= */

/**
 * @brief returns the length of s, not including the null terminator.
 */
size_t strlen(const char *s)
{
    size_t n = 0;
    while (*s++)
        n++;
    return n;
}

/**
 * @brief returns the length of s, examining at most maxlen bytes.
 * Safe against non-null-terminated strings.
 */
size_t strnlen(const char *s, size_t maxlen)
{
    size_t n = 0;
    while (n < maxlen && *s++)
        n++;
    return n;
}

/**
 * @brief compares strings a and b.
 *
 * @return 0 if equal, <0 if a < b, >0 if a > b
 */
int strcmp(const char *a, const char *b)
{
    while (*a && *a == *b)
    {
        a++;
        b++;
    }
    return (unsigned char)*a - (unsigned char)*b;
}

/**
 * @brief compares at most n characters of strings a and b.
 *
 * @return 0 if equal, <0 if a < b, >0 if a > b
 */
int strncmp(const char *a, const char *b, size_t n)
{
    while (n-- && *a && *a == *b)
    {
        a++;
        b++;
    }
    if (n == (size_t)-1)
        return 0;
    return (unsigned char)*a - (unsigned char)*b;
}

/**
 * @brief case-insensitive comparison of strings a and b.
 *
 * @return 0 if equal (ignoring case), <0 or >0 otherwise
 */
int strcasecmp(const char *a, const char *b)
{
    unsigned char ca, cb;
    do
    {
        ca = (unsigned char)*a++;
        cb = (unsigned char)*b++;
        if (ca >= 'A' && ca <= 'Z')
            ca += 32;
        if (cb >= 'A' && cb <= 'Z')
            cb += 32;
    } while (ca && ca == cb);
    return ca - cb;
}

/**
 * @brief case-insensitive comparison of at most n characters.
 */
int strncasecmp(const char *a, const char *b, size_t n)
{
    unsigned char ca, cb;
    if (n == 0)
        return 0;
    do
    {
        ca = (unsigned char)*a++;
        cb = (unsigned char)*b++;
        if (ca >= 'A' && ca <= 'Z')
            ca += 32;
        if (cb >= 'A' && cb <= 'Z')
            cb += 32;
        if (--n == 0)
            return ca - cb;
    } while (ca && ca == cb);
    return ca - cb;
}

/* =========================================================================
 * String copy / concatenation
 * ========================================================================= */

/**
 * @brief copies src into dest including the null terminator.
 * dest must be large enough to hold src.
 *
 * @return dest
 */
char *strcpy(char *dest, const char *src)
{
    char *d = dest;
    while ((*d++ = *src++))
        ;
    return dest;
}

/**
 * @brief copies at most n characters from src into dest.
 * If src is shorter than n, the remainder of dest is padded with nulls.
 * If src is n or more characters, dest will NOT be null-terminated —
 * always ensure dest[n] = '\0' after calling if you are unsure.
 *
 * @return dest
 */
char *strncpy(char *dest, const char *src, size_t n)
{
    char *d = dest;
    while (n > 0)
    {
        if ((*d = *src) != '\0')
            src++;
        d++;
        n--;
    }
    return dest;
}

/**
 * @brief safer strncpy: copies at most (size - 1) characters from src
 * into dest, always null-terminates. Returns number of characters in src
 * (not including null). If return >= size, the output was truncated.
 *
 * Prefer this over strncpy in new code.
 */
size_t strlcpy(char *dest, const char *src, size_t size)
{
    size_t src_len = strlen(src);
    if (size == 0)
        return src_len;
    size_t copy = src_len < size - 1 ? src_len : size - 1;
    memcpy(dest, src, copy);
    dest[copy] = '\0';
    return src_len;
}

/**
 * @brief appends src to the end of dest. dest must be large enough
 * to hold both strings and the null terminator.
 *
 * @return dest
 */
char *strcat(char *dest, const char *src)
{
    char *d = dest + strlen(dest);
    while ((*d++ = *src++))
        ;
    return dest;
}

/**
 * @brief appends at most n characters of src to dest, always
 * null-terminates.
 *
 * @return dest
 */
char *strncat(char *dest, const char *src, size_t n)
{
    char *d = dest + strlen(dest);
    while (n-- && *src)
        *d++ = *src++;
    *d = '\0';
    return dest;
}

/**
 * @brief safer strncat: appends src to dest, writing at most
 * (size - strlen(dest) - 1) characters, always null-terminates.
 * Returns the total length the result would have had (strlen(dest) +
 * strlen(src)) — if >= size, output was truncated.
 */
size_t strlcat(char *dest, const char *src, size_t size)
{
    size_t dest_len = strnlen(dest, size);
    size_t src_len = strlen(src);
    if (dest_len == size)
        return size + src_len;
    size_t copy = src_len < size - dest_len - 1 ? src_len : size - dest_len - 1;
    memcpy(dest + dest_len, src, copy);
    dest[dest_len + copy] = '\0';
    return dest_len + src_len;
}

/* =========================================================================
 * String search
 * ========================================================================= */

/**
 * @brief finds the first occurrence of character c in string s.
 *
 * @return pointer to the character, or NULL if not found.
 * The null terminator is considered part of the string (c may be '\0').
 */
char *strchr(const char *s, int c)
{
    do
    {
        if ((unsigned char)*s == (unsigned char)c)
            return (char *)s;
    } while (*s++);
    return NULL;
}

/**
 * @brief finds the last occurrence of character c in string s.
 *
 * @return pointer to the character, or NULL if not found
 */
char *strrchr(const char *s, int c)
{
    const char *last = NULL;
    do
    {
        if ((unsigned char)*s == (unsigned char)c)
            last = s;
    } while (*s++);
    return (char *)last;
}

/**
 * @brief finds the first occurrence of substring needle in haystack.
 *
 * @return pointer to start of the match, or NULL if not found
 */
char *strstr(const char *haystack, const char *needle)
{
    if (!*needle)
        return (char *)haystack;
    for (; *haystack; haystack++)
    {
        const char *h = haystack, *n = needle;
        while (*h && *n && *h == *n)
        {
            h++;
            n++;
        }
        if (!*n)
            return (char *)haystack;
    }
    return NULL;
}

/**
 * @brief returns the length of the initial segment of s that consists
 * entirely of characters in accept.
 */
size_t strspn(const char *s, const char *accept)
{
    size_t n = 0;
    while (*s && strchr(accept, *s++))
        n++;
    return n;
}

/**
 * @brief returns the length of the initial segment of s that contains
 * no characters from reject.
 */
size_t strcspn(const char *s, const char *reject)
{
    size_t n = 0;
    while (*s && !strchr(reject, *s++))
        n++;
    return n;
}

/**
 * @brief returns a pointer to the first character in s that is in
 * the string accept, or NULL if none found.
 */
char *strpbrk(const char *s, const char *accept)
{
    while (*s)
    {
        if (strchr(accept, *s))
            return (char *)s;
        s++;
    }
    return NULL;
}

/* =========================================================================
 * String conversion
 * ========================================================================= */

/**
 * @brief converts integer value to a null-terminated string in the
 * given base and stores it in buf. buf must be large enough to hold
 * the result (32 bytes is always sufficient for any 32-bit value in
 * any base).
 *
 * @param value   the integer to convert
 * @param buf     destination buffer
 * @param base    number base: 10 for decimal, 16 for hex, 2 for binary, etc.
 *                valid range: 2–36
 * @return        buf
 */
char *itoa(int value, char *buf, int base)
{
    static const char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    char tmp[34];
    char *t = tmp + sizeof(tmp) - 1;
    char *b = buf;
    int negative = 0;

    *t = '\0';

    if (base < 2 || base > 36)
    {
        *buf = '\0';
        return buf;
    }

    if (value < 0 && base == 10)
    {
        negative = 1;
        /* cast to unsigned to handle INT_MIN correctly */
        unsigned int uval = (unsigned int)value;
        do
        {
            *--t = digits[uval % (unsigned int)base];
            uval /= (unsigned int)base;
        } while (uval);
    }
    else
    {
        unsigned int uval = (unsigned int)value;
        do
        {
            *--t = digits[uval % (unsigned int)base];
            uval /= (unsigned int)base;
        } while (uval);
    }

    if (negative)
        *b++ = '-';

    while (*t)
        *b++ = *t++;
    *b = '\0';
    return buf;
}

/**
 * @brief converts unsigned integer value to a string. Same as itoa
 * but for unsigned values — avoids sign issues when base != 10.
 *
 * @param value  the value to convert
 * @param buf    destination buffer
 * @param base   number base: 2–36
 * @return       buf
 */
char *utoa(unsigned int value, char *buf, int base)
{
    static const char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    char tmp[34];
    char *t = tmp + sizeof(tmp) - 1;
    char *b = buf;

    *t = '\0';

    if (base < 2 || base > 36)
    {
        *buf = '\0';
        return buf;
    }

    do
    {
        *--t = digits[value % (unsigned int)base];
        value /= (unsigned int)base;
    } while (value);

    while (*t)
        *b++ = *t++;
    *b = '\0';
    return buf;
}

/**
 * @brief converts a decimal string to an integer. Skips leading
 * whitespace, handles an optional leading '+' or '-'.
 * Stops at the first non-digit character.
 *
 * @return the converted value, or 0 if no valid conversion was possible
 */
int atoi(const char *s)
{
    int result = 0, sign = 1;
    while (*s == ' ' || *s == '\t' || *s == '\n')
        s++;
    if (*s == '-')
    {
        sign = -1;
        s++;
    }
    else if (*s == '+')
        s++;
    while (*s >= '0' && *s <= '9')
        result = result * 10 + (*s++ - '0');
    return sign * result;
}

/* =========================================================================
 * Character classification (no-dependency replacements for ctype.h)
 * ========================================================================= */

/** @return 1 if c is an uppercase letter, 0 otherwise */
int isupper(int c) { return c >= 'A' && c <= 'Z'; }

/** @return 1 if c is a lowercase letter, 0 otherwise */
int islower(int c) { return c >= 'a' && c <= 'z'; }

/** @return 1 if c is a letter, 0 otherwise */
int isalpha(int c) { return isupper(c) || islower(c); }

/** @return 1 if c is a decimal digit, 0 otherwise */
int isdigit(int c) { return c >= '0' && c <= '9'; }

/** @return 1 if c is a hex digit, 0 otherwise */
int isxdigit(int c)
{
    return isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

/** @return 1 if c is a letter or digit, 0 otherwise */
int isalnum(int c) { return isalpha(c) || isdigit(c); }

/** @return 1 if c is a whitespace character, 0 otherwise */
int isspace(int c)
{
    return c == ' ' || c == '\t' || c == '\n' ||
           c == '\r' || c == '\f' || c == '\v';
}

/** @return 1 if c is a printable character (including space), 0 otherwise */
int isprint(int c) { return c >= 0x20 && c < 0x7F; }

/** @return the uppercase equivalent of c, or c if not a lowercase letter */
int toupper(int c) { return islower(c) ? c - 32 : c; }

/** @return the lowercase equivalent of c, or c if not an uppercase letter */
int tolower(int c) { return isupper(c) ? c + 32 : c; }
