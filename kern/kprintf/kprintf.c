/**
 * @file kprintf.c
 * @author Nathan Porter
 * @brief Freestanding printf implementation for the kernel.
 * Outputs via console_putc. Not thread safe.
 *
 * Supported format specifiers:
 *   %c        character
 *   %s        null-terminated string
 *   %d / %i   signed decimal integer
 *   %u        unsigned decimal integer
 *   %o        unsigned octal integer
 *   %x        unsigned hex (lowercase)
 *   %X        unsigned hex (uppercase)
 *   %b        unsigned binary
 *   %p        pointer (0x prefixed hex)
 *   %%        literal percent sign
 *
 * Supported flags:
 *   -         left-align within field width
 *   0         zero-pad (instead of space-pad) — ignored when left-aligning
 *   +         always print sign for signed conversions
 *   ' '       print a space instead of '+' for non-negative signed values
 *   #         alternate form: prefix 0x/0X for %x/%X, 0 for %o, 0b for %b
 *
 * Supported width:     decimal integer, e.g. %8d
 * Supported precision: decimal integer after '.', e.g. %.4d or %8.4d
 *                      for strings:  max characters to print
 *                      for integers: minimum digits (zero-pads)
 *
 * Supported length modifiers:
 *   hh   char / unsigned char
 *   h    short / unsigned short
 *   l    long / unsigned long
 *   z    size_t
 *
 * Note: ll / uint64_t are intentionally omitted — this kernel targets
 * 32-bit IA-32 and avoids the libgcc __udivdi3/__umoddi3 dependency.
 *
 */

#include <kprintf.h>
#include <console.h>
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>

/* ── internal helpers ─────────────────────────────────────────────────── */

static inline void emit(char c)
{
    console_putc(c);
}

static void emit_buf(const char *s, size_t len)
{
    while (len--)
        emit(*s++);
}

static void emit_repeat(char c, int n)
{
    while (n-- > 0)
        emit(c);
}

/* ── integer formatting ───────────────────────────────────────────────── */

/** Scratch buffer large enough for 32 binary digits + sign + prefix + null */
#define IBUF_SIZE 40

typedef struct
{
    int left_align;
    int zero_pad;
    int plus_sign;
    int space_sign;
    int alt_form;
    int width;
    int precision; /* -1 = not specified */
} fmt_flags_t;

/**
 * @brief Formats an unsigned 32-bit integer into a scratch buffer.
 *
 * @param value   the value to format
 * @param base    2, 8, 10, or 16
 * @param upper   1 for uppercase hex digits
 * @param buf     scratch buffer of at least IBUF_SIZE bytes
 * @return        pointer to first digit within buf
 */
static char *fmt_uint32(uint32_t value, uint32_t base, int upper, char *buf)
{
    static const char lo[] = "0123456789abcdef";
    static const char hi[] = "0123456789ABCDEF";
    const char *digits = upper ? hi : lo;

    char *p = buf + IBUF_SIZE - 1;
    *p = '\0';

    if (value == 0)
    {
        *--p = '0';
        return p;
    }

    while (value)
    {
        *--p = digits[value % base];
        value /= base;
    }
    return p;
}

/**
 * @brief Writes a fully formatted integer field to the console,
 * applying width, precision, padding, sign, and alternate-form prefix.
 */
static void emit_int_field(const char *digits, int n_digits,
                           char sign, const char *prefix,
                           const fmt_flags_t *f)
{
    int prefix_len = 0;
    while (prefix[prefix_len])
        prefix_len++;

    int sign_len = sign ? 1 : 0;

    int zero_count = 0;
    if (f->precision >= 0 && f->precision > n_digits)
    {
        zero_count = f->precision - n_digits;
    }
    else if (f->precision < 0 && f->zero_pad && !f->left_align)
    {
        int content = sign_len + prefix_len + n_digits;
        zero_count = f->width > content ? f->width - content : 0;
    }

    int total = sign_len + prefix_len + zero_count + n_digits;
    int pad = f->width > total ? f->width - total : 0;

    if (!f->left_align)
        emit_repeat(' ', pad);
    if (sign)
        emit(sign);
    emit_buf(prefix, (size_t)prefix_len);
    emit_repeat('0', zero_count);
    emit_buf(digits, (size_t)n_digits);
    if (f->left_align)
        emit_repeat(' ', pad);
}

/* ── string field ─────────────────────────────────────────────────────── */

static void emit_str_field(const char *s, int len, const fmt_flags_t *f)
{
    int pad = f->width > len ? f->width - len : 0;
    if (!f->left_align)
        emit_repeat(' ', pad);
    emit_buf(s, (size_t)len);
    if (f->left_align)
        emit_repeat(' ', pad);
}

/* ── main implementation ─────────────────────────────────────────────── */

int kvprintf(const char *fmt, va_list args)
{
    int written = 0;

    while (*fmt)
    {
        if (*fmt != '%')
        {
            emit(*fmt++);
            written++;
            continue;
        }
        fmt++; /* consume '%' */

        /* ── flags ── */
        fmt_flags_t f = {0};
        f.precision = -1;

        int parsing = 1;
        while (parsing)
        {
            switch (*fmt)
            {
            case '-':
                f.left_align = 1;
                fmt++;
                break;
            case '0':
                f.zero_pad = 1;
                fmt++;
                break;
            case '+':
                f.plus_sign = 1;
                fmt++;
                break;
            case ' ':
                f.space_sign = 1;
                fmt++;
                break;
            case '#':
                f.alt_form = 1;
                fmt++;
                break;
            default:
                parsing = 0;
                break;
            }
        }

        /* ── width ── */
        if (*fmt == '*')
        {
            f.width = va_arg(args, int);
            if (f.width < 0)
            {
                f.left_align = 1;
                f.width = -f.width;
            }
            fmt++;
        }
        else
        {
            while (*fmt >= '0' && *fmt <= '9')
                f.width = f.width * 10 + (*fmt++ - '0');
        }

        /* ── precision ── */
        if (*fmt == '.')
        {
            fmt++;
            f.precision = 0;
            if (*fmt == '*')
            {
                f.precision = va_arg(args, int);
                if (f.precision < 0)
                    f.precision = -1;
                fmt++;
            }
            else
            {
                while (*fmt >= '0' && *fmt <= '9')
                    f.precision = f.precision * 10 + (*fmt++ - '0');
            }
        }

        /* ── length modifier ── */
        enum
        {
            LEN_DEFAULT,
            LEN_HH,
            LEN_H,
            LEN_L,
            LEN_Z
        } length;
        length = LEN_DEFAULT;

        if (*fmt == 'h')
        {
            fmt++;
            if (*fmt == 'h')
            {
                length = LEN_HH;
                fmt++;
            }
            else
                length = LEN_H;
        }
        else if (*fmt == 'l')
        {
            length = LEN_L;
            fmt++;
        }
        else if (*fmt == 'z')
        {
            length = LEN_Z;
            fmt++;
        }

        /* ── specifier ── */
        char ibuf[IBUF_SIZE];
        char spec = *fmt++;

        switch (spec)
        {

        /* character */
        case 'c':
        {
            char c = (char)va_arg(args, int);
            int pad = f.width > 1 ? f.width - 1 : 0;
            if (!f.left_align)
                emit_repeat(' ', pad);
            emit(c);
            written++;
            if (f.left_align)
                emit_repeat(' ', pad);
            written += pad;
            break;
        }

        /* string */
        case 's':
        {
            const char *s = va_arg(args, const char *);
            if (!s)
                s = "(null)";
            int len = 0;
            while (s[len])
                len++;
            if (f.precision >= 0 && f.precision < len)
                len = f.precision;
            emit_str_field(s, len, &f);
            written += f.width > len ? f.width : len;
            break;
        }

        /* literal percent */
        case '%':
            emit('%');
            written++;
            break;

        /* signed decimal */
        case 'd':
        case 'i':
        {
            int32_t value;
            switch (length)
            {
            case LEN_HH:
                value = (int8_t)va_arg(args, int);
                break;
            case LEN_H:
                value = (int16_t)va_arg(args, int);
                break;
            case LEN_L:
                value = (int32_t)va_arg(args, long);
                break;
            case LEN_Z:
                value = (int32_t)va_arg(args, size_t);
                break;
            default:
                value = (int32_t)va_arg(args, int);
                break;
            }

            char sign = '\0';
            uint32_t uval;
            if (value < 0)
            {
                sign = '-';
                uval = (uint32_t)(-(value + 1)) + 1u;
            }
            else
            {
                uval = (uint32_t)value;
                if (f.plus_sign)
                    sign = '+';
                else if (f.space_sign)
                    sign = ' ';
            }

            char *digits = fmt_uint32(uval, 10, 0, ibuf);
            int n = (int)((ibuf + IBUF_SIZE - 1) - digits);
            emit_int_field(digits, n, sign, "", &f);
            written += f.width > 0 ? f.width : n + (sign ? 1 : 0);
            break;
        }

        /* unsigned decimal */
        case 'u':
        {
            uint32_t value;
            switch (length)
            {
            case LEN_HH:
                value = (uint8_t)va_arg(args, unsigned);
                break;
            case LEN_H:
                value = (uint16_t)va_arg(args, unsigned);
                break;
            case LEN_L:
                value = (uint32_t)va_arg(args, unsigned long);
                break;
            case LEN_Z:
                value = (uint32_t)va_arg(args, size_t);
                break;
            default:
                value = (uint32_t)va_arg(args, unsigned);
                break;
            }
            char *digits = fmt_uint32(value, 10, 0, ibuf);
            int n = (int)((ibuf + IBUF_SIZE - 1) - digits);
            emit_int_field(digits, n, '\0', "", &f);
            written += f.width > n ? f.width : n;
            break;
        }

        /* octal */
        case 'o':
        {
            uint32_t value;
            switch (length)
            {
            case LEN_L:
                value = (uint32_t)va_arg(args, unsigned long);
                break;
            default:
                value = (uint32_t)va_arg(args, unsigned);
                break;
            }
            char *digits = fmt_uint32(value, 8, 0, ibuf);
            int n = (int)((ibuf + IBUF_SIZE - 1) - digits);
            const char *prefix = (f.alt_form && value != 0) ? "0" : "";
            emit_int_field(digits, n, '\0', prefix, &f);
            written += f.width > n ? f.width : n;
            break;
        }

        /* hex lower / upper */
        case 'x':
        case 'X':
        {
            uint32_t value;
            switch (length)
            {
            case LEN_L:
                value = (uint32_t)va_arg(args, unsigned long);
                break;
            default:
                value = (uint32_t)va_arg(args, unsigned);
                break;
            }
            int upper = (spec == 'X');
            char *digits = fmt_uint32(value, 16, upper, ibuf);
            int n = (int)((ibuf + IBUF_SIZE - 1) - digits);
            const char *prefix = "";
            if (f.alt_form && value != 0)
                prefix = upper ? "0X" : "0x";
            emit_int_field(digits, n, '\0', prefix, &f);
            written += f.width > n ? f.width : n;
            break;
        }

        /* binary (non-standard, handy for kernel flag inspection) */
        case 'b':
        {
            uint32_t value;
            switch (length)
            {
            case LEN_L:
                value = (uint32_t)va_arg(args, unsigned long);
                break;
            default:
                value = (uint32_t)va_arg(args, unsigned);
                break;
            }
            char *digits = fmt_uint32(value, 2, 0, ibuf);
            int n = (int)((ibuf + IBUF_SIZE - 1) - digits);
            const char *prefix = f.alt_form ? "0b" : "";
            emit_int_field(digits, n, '\0', prefix, &f);
            written += f.width > n ? f.width : n;
            break;
        }

        /* pointer */
        case 'p':
        {
            uint32_t value = (uint32_t)(uintptr_t)va_arg(args, void *);
            char *digits = fmt_uint32(value, 16, 0, ibuf);
            int n = (int)((ibuf + IBUF_SIZE - 1) - digits);
            emit_int_field(digits, n, '\0', "0x", &f);
            written += f.width > n + 2 ? f.width : n + 2;
            break;
        }

        /* unknown: echo literally */
        default:
            emit('%');
            emit(spec);
            written += 2;
            break;
        }
    }

    return written;
}

int kprintf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int n = kvprintf(fmt, args);
    va_end(args);
    return n;
}
