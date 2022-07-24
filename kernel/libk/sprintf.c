/*
 * Copyright (C) 2018, Joe Davis <me@jo.ie>
 * Copyright (C) 2020, 2021, 2022, Kirill GPRB
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <kernel/libk/sprintf.h>

struct buffer {
    char *data;
    size_t size;
    size_t pos;
};

char sprintf_buffer[512];

#define FLAGS_ZEROPAD   (1 << 0)
#define FLAGS_LEFT      (1 << 1)
#define FLAGS_PLUS      (1 << 2)
#define FLAGS_SPACE     (1 << 3)
#define FLAGS_HASH      (1 << 4)
#define FLAGS_SIGNED    (1 << 5)
#define FLAGS_UPPERCASE (1 << 6)

#define ILEN_CHAR       0 /* hh */
#define ILEN_SHORT      1 /* h  */
#define ILEN_INT        2
#define ILEN_LONG       3 /* l  */
#define ILEN_DLONG      4 /* ll */
#define ILEN_INTMAX_T   5 /* j  */
#define ILEN_SIZE_T     6 /* z  */
#define ILEN_PTRDIFF_T  7 /* t  */

#define va_arg_ilen(ap, ilen, iv)                   \
    switch(ilen) {                                  \
        case ILEN_CHAR:                             \
            iv = va_arg(ap, unsigned int);          \
            break;                                  \
        case ILEN_SHORT:                            \
            iv = va_arg(ap, unsigned int);          \
            break;                                  \
        case ILEN_INT:                              \
            iv = va_arg(ap, unsigned int);          \
            break;                                  \
        case ILEN_LONG:                             \
            iv = va_arg(ap, unsigned long);         \
            break;                                  \
        case ILEN_DLONG:                            \
            iv = va_arg(ap, unsigned long long);    \
            break;                                  \
        case ILEN_INTMAX_T:                         \
            iv = va_arg(ap, uintmax_t);             \
            break;                                  \
        case ILEN_SIZE_T:                           \
            iv = va_arg(ap, size_t);                \
            break;                                  \
        case ILEN_PTRDIFF_T:                        \
            iv = va_arg(ap, ptrdiff_t);             \
            break;                                  \
        default:                                    \
            iv = 0;                                 \
            break;                                  \
    }

#define de_sign(iv, sign, type) \
    if((type)iv < 0) {          \
        iv = -(type)iv;         \
        sign = '-';             \
    }

static inline int is_flag(int c)
{
    return (c == '0' || c == '-' || c == '+' || c == ' ' || c == '#');
}

static inline size_t fmt_atoz(const char **fmt)
{
    size_t value = 0;
    while(isdigit(**fmt)) {
        value = (value * 10) + (**fmt - '0');
        (*fmt)++;
    }

    return value;
}

static inline void buffer_write(struct buffer *buf, char c)
{
    if(buf->pos < buf->size)
        buf->data[buf->pos] = c;
    buf->pos++;
}

#define MAX_BASE 16

uint64_t __udivmoddi4(uint64_t num, uint64_t den, uint64_t *rem_ptr) {
    uint64_t quot = 0, qbit = 1;
    if (den == 0) {
        // Divide by zero to trigger error
        return 1/((unsigned)den); 
    }
    // Left-justify denominator and count shift
    while ((int64_t)den >= 0) {
        den <<= 1;
        qbit <<= 1;
    }
    while (qbit) {
        if ( den <= num ) {
            num -= den;
            quot += qbit;
        }
        den >>= 1;
        qbit >>= 1;
    }
    if (rem_ptr) {
        *rem_ptr = num;
    }
    return quot;
}

static void buffer_write_int(struct buffer *buf, uintmax_t iv, int flags, size_t width, size_t precision, int ilen, int base)
{
    const char *digits = (flags & FLAGS_UPPERCASE) ? "0123456789ABCDEF" : "0123456789abcdef";
    char ibuf_data[96] = { 0 };
    struct buffer ibuf = { .data = ibuf_data, .size = sizeof(ibuf_data), .pos = 0 };
    char sign = 0, pad = (flags & FLAGS_ZEROPAD) ? '0' : ' ';
    size_t ibuf_pos;

    if(base > MAX_BASE)
        base = MAX_BASE;

    if(flags & FLAGS_SIGNED) {
        switch(ilen) {
            case ILEN_CHAR:
                de_sign(iv, sign, signed char);
                break;
            case ILEN_SHORT:
                de_sign(iv, sign, signed short);
                break;
            case ILEN_INT:
                de_sign(iv, sign, signed int);
                break;
            case ILEN_LONG:
                de_sign(iv, sign, signed long);
                break;
            case ILEN_DLONG:
                de_sign(iv, sign, signed long long);
                break;
            case ILEN_INTMAX_T:
                de_sign(iv, sign, intmax_t);
                break;
            case ILEN_PTRDIFF_T:
                de_sign(iv, sign, ptrdiff_t);
                break;
        }
    }

    if(sign == 0) {
        if(flags & FLAGS_SPACE)
            sign = ' ';
        else if(flags & FLAGS_PLUS)
            sign = '+';
    }

    do {
        // TODO: use native division for 64bit archs
        uint64_t rem, quot;
        quot = __udivmoddi4(iv, base, &rem);
        buffer_write(&ibuf, digits[(size_t)rem]);
        iv = quot;
        if(precision)
            precision--;
    } while(iv > 0 || precision != 0);

    if(sign)
        buffer_write(&ibuf, sign);

    if(flags & FLAGS_HASH) {
        if(!(flags & FLAGS_ZEROPAD)) {
            switch(base) {
                case 2:
                    buffer_write(&ibuf, 'b');
                    buffer_write(&ibuf, '0');
                    break;
                case 16:
                    buffer_write(&ibuf, 'x');
                    buffer_write(&ibuf, '0');
                    break;
            }
        }

        if(base == 8)
            buffer_write(&ibuf, '0');
    }

    ibuf_pos = ibuf.pos;
    if(!(flags & FLAGS_LEFT)) {
        while(ibuf.pos < width)
            buffer_write(&ibuf, pad);

        if((flags & FLAGS_HASH) && (flags & FLAGS_ZEROPAD)) {
            switch(base) {
                case 2:
                    buffer_write(&ibuf, 'b');
                    buffer_write(&ibuf, '0');
                    break;
                case 16:
                    buffer_write(&ibuf, 'x');
                    buffer_write(&ibuf, '0');
                    break;
            }
        }
    }

    if(ibuf.pos >= ibuf.size)
        ibuf.pos = ibuf.size;
    
    while(ibuf.pos--)
        buffer_write(buf, ibuf.data[ibuf.pos]);

    if(flags & FLAGS_LEFT) {
        while(ibuf_pos < width) {
            buffer_write(buf, ' ');
            ibuf_pos++;
        }
    }
}

static void buffer_write_str(struct buffer *buf, const char *str, int flags, size_t width, size_t precision)
{
    size_t i, length = precision ? strnlen(str, precision) : strlen(str);

    if(!(flags & FLAGS_LEFT)) {
        for(i = length; i < width; i++)
            buffer_write(buf, ' ');
    }

    while(*str)
        buffer_write(buf, *str++);

    if(flags & FLAGS_LEFT) {
        for(i = length; i < width; i++)
            buffer_write(buf, ' ');
    }
}

int sprintf(char *s, const char *fmt, ...)
{
    int p;
    va_list ap;
    va_start(ap, fmt);
    p = vsnprintf(s, (size_t)-1, fmt, ap);
    va_end(ap);
    return p;
}

int vsprintf(char *s, const char *fmt, va_list ap)
{
    return vsnprintf(s, (size_t)-1, fmt, ap);
}

int snprintf(char *s, size_t n, const char *fmt, ...)
{
    int p;
    va_list ap;
    va_start(ap, fmt);
    p = vsnprintf(s, n, fmt, ap);
    va_end(ap);
    return p;
}

int vsnprintf(char *s, size_t n, const char *fmt, va_list ap)
{
    struct buffer buf = {.data = s, .size = n, .pos = 0};

    int flags, ilen;
    size_t width, precision;
    uintmax_t iv;

    while(*fmt) {
        if(*fmt != '%' || *++fmt == '%') {
            buffer_write(&buf, *fmt++);
            continue;
        }

        flags = 0;
        while(is_flag(*fmt)) {
            switch(*fmt++) {
                case '0':   flags |= FLAGS_ZEROPAD; break;
                case '-':   flags |= FLAGS_LEFT;    break;
                case '+':   flags |= FLAGS_PLUS;    break;
                case ' ':   flags |= FLAGS_SPACE;   break;
                case '#':   flags |= FLAGS_HASH;    break;
            }
        }

        if((flags & FLAGS_ZEROPAD) && (flags & FLAGS_LEFT))
            flags &= ~FLAGS_ZEROPAD;

        width = fmt_atoz(&fmt);
        precision = 0;

        if(*fmt == '.') {
            fmt++;
            precision = fmt_atoz(&fmt);
        }

        ilen = ILEN_INT;
        switch(*fmt) {
            case 'h':   ilen = (*++fmt == 'h') ? ILEN_CHAR : ILEN_SHORT;    break;
            case 'l':   ilen = (*++fmt == 'l') ? ILEN_DLONG : ILEN_LONG;    break;
            case 'j':   ilen = ILEN_INTMAX_T;                               break;
            case 'z':   ilen = ILEN_SIZE_T;                                 break;
            case 't':   ilen = ILEN_PTRDIFF_T;                              break;
        }

        if(ilen != ILEN_INT && ilen != ILEN_SHORT && ilen != ILEN_LONG)
            fmt++;

        iv = 0;
        switch (*fmt) {
            case 'd':
            case 'i':
                flags |= FLAGS_SIGNED;
                va_arg_ilen(ap, ilen, iv);
                buffer_write_int(&buf, iv, flags, width, precision, ilen, 10);
                break;

            case 'u':
                va_arg_ilen(ap, ilen, iv);
                buffer_write_int(&buf, iv, flags, width, precision, ilen, 10);
                break;
            
            case 'x':
                va_arg_ilen(ap, ilen, iv);
                buffer_write_int(&buf, iv, flags, width, precision, ilen, 16);
                break;

            case 'X':
                flags |= FLAGS_UPPERCASE;
                va_arg_ilen(ap, ilen, iv);
                buffer_write_int(&buf, iv, flags, width, precision, ilen, 16);
                break;
            
            case 'o':
                va_arg_ilen(ap, ilen, iv);
                buffer_write_int(&buf, iv, flags, width, precision, ilen, 8);
                break;

            case 'b':
                va_arg_ilen(ap, ilen, iv);
                buffer_write_int(&buf, iv, flags, width, precision, ilen, 2);
                break;

            case 'c':
                buffer_write(&buf, (char)va_arg(ap, int));
                break;
            
            case 's':
                buffer_write_str(&buf, va_arg(ap, const char *), flags, width, precision);
                break;
            
            case 'p':
                flags |= FLAGS_HASH;
                flags |= FLAGS_UPPERCASE;
                flags |= FLAGS_ZEROPAD;
                buffer_write_int(&buf, (uintptr_t)va_arg(ap, void *), flags, sizeof(uintptr_t) * 2, precision, ILEN_INTMAX_T, 16);
                break;
        }

        fmt++;
    }

    buffer_write(&buf, 0);
    s[n - 1] = 0;
    return (buf.pos - 1);
}