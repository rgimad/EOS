/*
*    EOS - Experimental Operating System
*    Interrupt manager header file
*/
#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_

#include <stdbool.h>
#include <stdint.h>

#define TIMER_IDT_INDEX 32
#define KEYBOARD_IDT_INDEX 33
#define SYSCALL_IDT_INDEX 128

// Holds the registers at the time of the interrupt
struct regs {
  uint32_t gs, fs, es, ds;                          // pushed the segs last
  uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;  // pushed by pusha
  uint32_t idt_index, err_code;                     // manually pushed
  uint32_t eip, cs, eflags, useresp, ss;            // pushed automatically
};

//interrupt_handler_t type is a pointer to a function which receives struct regs *r and returns nothing(void).
//so an interrupt handler has to receive cpu registers state.
typedef void (*interrupt_handler_t)(struct regs* r);

bool register_interrupt_handler(uint32_t idt_index, interrupt_handler_t handler);

#endif /* INTERRUPT_H */