/*
*    EOS - Experimental Operating System
*    Scheduler header
*/
#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include <stdint.h>
#include <stdbool.h>
#include <libk/list.h>
#include <kernel/interrupts.h>//for struct regs
#include <kernel/pm/process.h>
#include <kernel/pm/thread.h>

extern uint32_t pid_counter;
extern uint32_t tid_counter;
extern bool scheduler_enabled;
extern list_t *process_list;
extern process_t *kernel_process; // pointer to kernel process structure
extern thread_t *kernel_main_thread; // pointer to kernel's main thread structure

void scheduler_init();

void kthread_grafdemo();

void schedule(struct regs *r); // scheduler itself. This function is called at every timer (PIT) tick

//extern void kernel_regs_switch(context_t *ctx);

extern void task_switch(); // implemented in context_switch_asm.s

#endif // _SCHEDULER_H