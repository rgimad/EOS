/*
*    EOS - Experimental Operating System
*    Thread header
*/
#ifndef _THREAD_H
#define _THREAD_H

#include <stddef.h>
#include <stdint.h>

//Limits
#define THREAD_KSTACK_SIZE 4096
#define THREAD_USTACK_SIZE (4 * 4096)

// Possible thread states
#define THREAD_READY       0
#define THREAD_BLOCKED     1
#define THREAD_RUNNING     2
#define THREAD_TERMINATED  4

#define THREAD_KERNEL_MODE 11
#define THREAD_USER_MODE   12


typedef struct context {
	uint32_t eax; // 0
    uint32_t ebx; // 4
    uint32_t ecx; // 8
    uint32_t edx; // 12
    uint32_t esp; // 16
    uint32_t ebp; // 20
    uint32_t esi; // 24
    uint32_t edi; // 28
    uint32_t eflags; //32
    uint32_t cr3; // 36
    uint32_t eip; //40
} context_t;


typedef struct thread_t
{
	uint32_t tid; // thread id
	uint32_t state; // thread's state
	context_t regs; //thread's context
    uint32_t privileges; // kernel mode or user mode

	listnode_t *self_item;// pointer to this thread's list node
	process_t *process; // process which this thread belongs to

	void *kernel_stack; // thread's kernel stack top
	void *user_stack; // thread's user stack top

	void* entry_point; // thread's code entry point

	uint32_t time;// time of execution

} __attribute__((packed)) thread_t;


#endif //_THREAD_H