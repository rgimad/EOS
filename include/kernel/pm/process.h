/*
*    EOS - Experimental Operating System
*    Process header
*/
#ifndef _PROCESS_H
#define _PROCESS_H

#include <stddef.h>
#include <stdint.h>
#include <libk/list.h>

//Limits
//#define PROCESS_MSG_BUFF_SIZE 16

// Possible process states
#define PROCESS_UNINTERRUPTIBLE 1
#define PROCESS_INTERRUPTIBLE 2
#define PROCESS_RUNNING 3
#define PROCESS_KILLING 4
//maybe need more states?

// Process structure
typedef	struct process_t
{
	uint32_t pid;
	uint32_t state;
	char name[256];
	char *cmdline;

	listnode_t *self_item; // pointer to this process's list node

	size_t threads_count;
	list_t *thread_list;

	void *page_dir; // physical address of page directory structure that was created in kernel heap

	void *heap_begin;
	void *heap_end;
	//uint32_t heap_size;

	//void *stack_begin;
	//void *stack_end;

	thread_t *parent_proc_thread;

} __attribute__((packed)) process_t;

#endif //_PROCESS_H