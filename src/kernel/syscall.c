/*
*    EOS - Experimental Operating System
*    System calls module
*/
#include <kernel/syscall.h>
//#include <kernel/interrupts.h>

#include <kernel/tty.h>

#include <libk/string.h>


uint32_t sc_puts(char *str)
{
	tty_printf("%s", str);
	return (uint32_t)0;
}


void syscall_init()
{
	register_interrupt_handler(SYSCALL_IDT_INDEX, &syscall_handler);
}

void syscall_handler(struct regs *r)
{
	//uint32_t _eax, _ebx, _ecx, _edx;
	uint32_t result = -1;
	//asm volatile("cli" : "=a"(_eax), "=b"(_ebx), "=c"(_ecx), "=d"(_edx));
	asm("cli;");
	//tty_printf("\n eax %d ebx %d ecx %d edx %d\n", r->eax, r->ebx, r->ecx, r->edx);
	uint32_t* argptr = (uint32_t*)(r->ebx);
	switch(r->eax)
	{
		case SC_CODE_puts: result = sc_puts((char*)(argptr[0])); break;
		default: //panic("Invalid syscall");
			tty_printf("Invalid syscall #%x\n", r->eax);
			asm("cli;hlt;");
	}

	r->eax = result;
	asm volatile("sti;" : : "a"(r->eax), "b"(r->ebx), "c"(r->ecx), "d"(r->edx));
}