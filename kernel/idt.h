/*
 * EOS - Experimental Operating System
 * IDT header
 */

#ifndef _KERNEL_IDT_H_
#define _KERNEL_IDT_H_

#include <stdint.h>

//#include <asm.h>

#define IDT_NUM_ENTRIES 256

// Sets up the IDT, should be called on early initialization
void idt_install(void);

void interrupt_enable_all(void);
void interrupt_disable_all(void);

#endif // _KERNEL_IDT_H_
