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

void scheduler_init();

#endif // _SCHEDULER_H