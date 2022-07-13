#include <stdarg.h>

#define __dllexport __attribute__((dllexport))

__dllexport void putchar(char c) {
    __asm__ __volatile__("int $0x40" ::"a"(63), "b"(1), "c"(c));
}

void putstring(const char *s) {
    unsigned i = 0;
    while (*(s + i)) {
        __asm__  __volatile__("int $0x40" ::"a"(63), "b"(1), "c"(*(s + i)));
        i++;
    }
}

__dllexport void puts(const char* s) {
   putstring(s);
   putchar('\n');
}

void putuint(int i) {
    unsigned int n, d = 1000000000;
    char str[255];
    unsigned int dec_index = 0;

    while ((i / d == 0) && (d >= 10)) {
        d /= 10;
    }
    n = i;

    while (d >= 10) {
        str[dec_index++] = ((char) ((int) '0' + n/d));
        n = n % d;
        d /= 10;
    }

    str[dec_index++] = ((char) ((int) '0' + n));
    str[dec_index] = 0;
    putstring(str);
}

void putint(int i) {
    if (i >= 0) {
        putuint(i);
    } else {
        putchar('-');
        putuint(-i);
    }
}

void puthex(unsigned i) {
    const unsigned char hex[16]  =  { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
    unsigned int n, d = 0x10000000;

    putstring("0x");

    while ((i / d == 0) && (d >= 0x10)) {
        d /= 0x10;
    }
    n = i;

    while (d >= 0xF) {
        putchar(hex[n / d]);
        n = n % d;
        d /= 0x10;
    }

    putchar(hex[n]);
}

void print(char *format, va_list args) {
    int i = 0;
    char *string;

    while (format[i]) {
        if (format[i] == '%') {
            i++;
            switch (format[i]) {
            case 's':
                string = va_arg(args, char*);
                putstring(string);
                break;
            case 'c':
                // TODO: Fix this! "warning: cast to pointer from integer of different size"
                putchar(va_arg(args, int));
                break;
            case 'd':
                putint(va_arg(args, int));
                break;
            case 'i':
                putint(va_arg(args, int));
                break;
            case 'u':
                putuint(va_arg(args, unsigned int));
                break;
            case 'x':
                puthex(va_arg(args, unsigned int));
                break;
            default:
                putchar(format[i]);
            }
        } else {
            putchar(format[i]);
        }
        i++;
    }
}

__dllexport void printf(char *text, ...) {
    va_list args;
    // Find the first argument
    va_start(args, text);
    // Pass print the output handle the format text and the first argument
    print(text, args);
}

int DllMain(void) {
    return 0;
}