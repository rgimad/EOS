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
void idt_install();

//inline void enable_interrupts(void) { asm volatile("sti"); }
//inline void disable_interrupts(void) { asm volatile("cli"); }

void interrupt_enable_all();
void interrupt_disable_all();

#endif // _KERNEL_IDT_H_
