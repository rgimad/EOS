#ifndef LIBK_SPRINTF_H
#define LIBK_SPRINTF_H

#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include "string.h"
#include "ctype.h"
#include "cdefs.h"

extern char sprintf_buffer[512];

int sprintf(char *s, const char *fmt, ...) __format(printf, 2, 3);
int vsprintf(char *s, const char *fmt, va_list va);
int snprintf(char *s, size_t n, const char *fmt, ...) __format(printf, 3, 4);
int vsnprintf(char *s, size_t n, const char *fmt, va_list va);

#endif
