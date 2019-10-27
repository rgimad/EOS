/*
*    EOS - Experimental Operating System
*    System calls module header
*/
#ifndef _SYSCALL_H
#define _SYSCALL_H

#include <kernel/interrupts.h>

#define SC_CODE_puts 0
//#define SC_CODE_gets 1


void syscall_handler(struct regs *r);


#endif