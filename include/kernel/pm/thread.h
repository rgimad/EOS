/*
*    EOS - Experimental Operating System
*    Thread header
*/
#ifndef _THREAD_H
#define _THREAD_H

#include <stddef.h>
#include <stdint.h>
#include <kernel/pm/process.h>

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
	uint32_t tid; // 0, thread id
	uint32_t state; // 4, thread's state

    uint32_t privileges; // 8, kernel mode or user mode

    uint32_t esp; // 12, thread's esp value

	listnode_t *self_item;// 16, pointer to this thread's list node
	process_t *process; // 20, process which this thread belongs to

	void *kernel_stack; // 24, thread's kernel stack top
	void *user_stack; // 28, thread's user stack top

    uint32_t kernel_stack_size; // 32
    uint32_t user_stack_size; // 36

	void* entry_point; // 40, thread's code entry point

	uint32_t time;// time of execution

    context_t registers; //thread's context
    char name[128]; // name of thread. //was 256

} __attribute__((packed)) thread_t;

// Thread functions

thread_t* create_user_thread(process_t *parent_proc, void *entry_point); // create a new thread within a usermode process
thread_t* create_kernel_thread(void *entry_point); // create a new thread within kernel_process


#endif //_THREAD_H