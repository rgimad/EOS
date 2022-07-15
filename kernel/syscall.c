/*
 * EOS - Experimental Operating System
 * System calls module
 */

#include <kernel/syscall.h>

void syscall_init()
{
    register_interrupt_handler(0x40, &syscall_i40_handler);
}

void syscall_i40_handler(struct regs *r)
{
    if (r->eax == 63) {
        if (r->ebx == 1) {
            tty_printf("%c", (char)(r->ecx & 0xFF));
        }
    }
}
