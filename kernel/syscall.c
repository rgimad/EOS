/*
 * EOS - Experimental Operating System
 * System calls module
 */

#include <kernel/syscall.h>

#include <kernel/tty.h>
#include <kernel/devices/keyboard.h>
#include <kernel/graphics/vesafb.h>

#include <kernel/libk/string.h>

uint32_t sc_puts(char *str) {
    tty_printf("%s", str);
    return (uint32_t)0;
}

// uint32_t sc_gets(char *str, int cnt) {
//     keyboard_gets(str, cnt);
// }

void syscall_init() {
    register_interrupt_handler(SYSCALL_IDT_INDEX, &syscall_handler);
    register_interrupt_handler(0x40, &syscall_i40_handler);
}

void syscall_handler(struct regs *r) {
    //tty_printf("welcome to syscall handler!\n");
    //uint32_t _eax, _ebx, _ecx, _edx;
    uint32_t result = -1;
    //asm volatile("cli" : "=a"(_eax), "=b"(_ebx), "=c"(_ecx), "=d"(_edx));

    //asm("cli;");

    //tty_printf("\n eax %d ebx %d ecx %d edx %d\n", r->eax, r->ebx, r->ecx, r->edx);
    uint32_t* argptr = (uint32_t*) (r->ebx);
    //void* argptr = (void*)(r->ebx);

    //tty_printf("argptr = %x\n", argptr);  // DEBUG OUTPUT

    switch (r->eax) {
    case SC_CODE_puts:
        tty_printf("str = %x\n", (char*) (argptr[0]));
        result = sc_puts((char*) (argptr[0]));
        break;
    //case SC_CODE_gets: tty_printf("str = %x, num = %d\n", (char*)(argptr[0]), (int)(argptr[1])); result = sc_gets((char*)(argptr[0]), (int)(argptr[1])); break;
    case SC_CODE_draw_square:
        draw_square(((int*) argptr)[0], ((int*) argptr)[1], ((int*) argptr)[2], ((int*) argptr)[3], ((uint32_t*) argptr)[4]);
        break;
    default: //panic("Invalid syscall");
        tty_printf("Invalid syscall #%x\n", r->eax);
        //asm("cli;hlt;");
    }

    r->eax = result;

    //asm volatile("sti;" : : "a"(r->eax), "b"(r->ebx), "c"(r->ecx), "d"(r->edx));
}

void syscall_i40_handler(struct regs *r) {
    if (r->eax == 63) {
        if (r->ebx == 1) {
            tty_printf("%c", (char)(r->ecx & 0xFF));
        }
    }
}

