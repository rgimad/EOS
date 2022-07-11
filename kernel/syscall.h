/*
 * EOS - Experimental Operating System
 * System calls module header
 */

#ifndef _SYSCALL_H
#define _SYSCALL_H

#include <kernel/tty.h>
#include <kernel/interrupts.h>

void syscall_init();
void syscall_i40_handler(struct regs *r);

#endif
