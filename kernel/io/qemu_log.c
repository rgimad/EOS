/*
 * EOS - Experimental Operating System
 * Qemu logging functions
 */

#include <kernel/io/qemu_log.h>

void qemu_putstring(const char *data)
{
    for (size_t i = 0; i < strlen(data); i++) {
        com1_write_char(data[i]);
    }
}

void qemu_printf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vsnprintf(sprintf_buffer, sizeof(sprintf_buffer), format, args);
    qemu_putstring(sprintf_buffer);
}
