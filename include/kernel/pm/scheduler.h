/*
*    EOS - Experimental Operating System
*    Scheduler header
*/
#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include <libk/list.h>

extern uint32_t pid_counter;
extern uint32_t tid_counter;
extern bool scheduler_enabled;

extern list_t *process_list;

void scheduler_init();

void schedule(); // scheduler itself. This function is called at every timer (PIT) tick

#endif // _SCHEDULER_H