/*
 * EOS - Experimental Operating System
 * Qemu logging functions
 */

#include <kernel/io/qemu_log.h>
#include <kernel/io/ports.h>

#include <kernel/libk/string.h>

void qemu_putstring(const char *data)
{
    for (size_t i = 0; i < strlen(data); i++) {
        com1_write_char(data[i]);
    }
}

void qemu_putuint(int i)
{
    unsigned int n, d = 1000000000;
    char str[255];
    unsigned int dec_index = 0;

    while ((i / d == 0) && (d >= 10)) {
        d /= 10;
    }
    n = i;

    while (d >= 10) {
        str[dec_index++] = ((char)((int)'0' + n / d));
        n = n % d;
        d /= 10;
    }

    str[dec_index++] = ((char)((int)'0' + n));
    str[dec_index] = 0;
    qemu_putstring(str);
}

void qemu_putint(int i)
{
    if (i >= 0) {
        qemu_putuint(i);
    } else {
        com1_write_char('-');
        qemu_putuint(-i);
    }
}

void qemu_puthex(uint32_t i)
{
    const unsigned char hex[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
    unsigned int n, d = 0x10000000;

    qemu_putstring("0x");

    while ((i / d == 0) && (d >= 0x10)) {
        d /= 0x10;
    }
    n = i;

    while (d >= 0xF) {
        com1_write_char(hex[n / d]);
        n = n % d;
        d /= 0x10;
    }
    com1_write_char(hex[n]);
}

void qemu_print(char *format, va_list args)
{
    int i = 0;
    char *string;

    while (format[i]) {
        if (format[i] == '%') {
            i++;
            switch (format[i]) {
            case 's':
                string = va_arg(args, char *);
                qemu_putstring(string);
                break;
            case 'c':
                // To-Do: fix this! "warning: cast to pointer from integer of different size"
                com1_write_char(va_arg(args, int));
                break;
            case 'd':
                qemu_putint(va_arg(args, int));
                break;
            case 'i':
                qemu_putint(va_arg(args, int));
                break;
            case 'u':
                qemu_putuint(va_arg(args, unsigned int));
                break;
            case 'x':
                qemu_puthex(va_arg(args, uint32_t));
                break;
            default:
                com1_write_char(format[i]);
            }
        } else {
            com1_write_char(format[i]);
        }
        i++;
    }
}

void qemu_printf(char *text, ...)
{
    va_list args;
    // find the first argument
    va_start(args, text);
    // pass print the output handle the format text and the first argument
    qemu_print(text, args);
}
