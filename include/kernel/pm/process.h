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
	uint32_t pid; // 0
	uint32_t state; // 4
	char *cmdline; // 8

	listnode_t *self_item; // 12, pointer to this process's list node

	size_t threads_count; // 16
	list_t *thread_list; // 20

	void *page_dir; // 24 physical address of page directory structure that was created in kernel heap

	void *heap_begin;
	void *heap_end;
	uint32_t heap_memory_used; // how many memory was used
	int heap_allocs_num; // how many allocations now

	//void *stack_begin;
	//void *stack_end;

	void *parent_proc_thread;
	char name[128];

} __attribute__((packed)) process_t;

// Process methods

process_t* create_process(char *filepath);

#endif //_PROCESS_H