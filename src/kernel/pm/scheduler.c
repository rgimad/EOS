/*
*    EOS - Experimental Operating System
*    Scheduler implementation
*/
#include <kernel/pm/scheduler.h>
#include <kernel/mm/kheap.h>
#include <libk/string.h>

uint32_t pid_counter = 0; // last process id value. New process will be pid_counter++
uint32_t tid_counter = 0; // last thread id value. New thread will be tid_counter++
bool scheduler_enabled = false; // enable or disable the scheduler

procces_t *kernel_process; // pointer to kernel process structure
thread_t *kernel_main_thread; // pointer to kernel's main thread structure

list_t *process_list; // this list contains all the processes runned in the system
// we dont need to have a global thread list beacuse each process already has its own thread list

procces_t *current_process; // pointer to current running process structure
thread_t *current_thread; // pointer to current running thread structure


void scheduler_init()
{
	//uint32_t esp = 0;
    //asm volatile("mov %%esp, %0":"=a"(esp)); 
    asm volatile ("cli");

    process_list = list_create();

    kernel_process = (procces_t*)kheap_malloc(sizeof(procces_t));
    memset(kernel_process, 0, sizeof(process_t));

    kernel_process->pid = pid_counter++;
    //kernel_process->state = ??????;
    strcpy(kernel_process->name, "kernel.elf");
    kernel_process->self_item = kernel_process;
    kernel_process->threads_count = 1;
    kernel_process->thread_list = list_create();
    //kernel_process->page_dir = ????????????????????;
    


}