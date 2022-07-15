#include <kernel/tty.h>
#include <kernel/io/qemu_log.h>

void __assert_func(const char *file, int line, const char *func, const char *failedexpr)
{
    tty_printf("Assertion failed in %s:%d (%s): %s\n", file, line, func, failedexpr);
    qemu_printf("Assertion failed in %s:%d (%s): %s\n", file, line, func, failedexpr);
}