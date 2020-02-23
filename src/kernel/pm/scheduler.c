/*
*    EOS - Experimental Operating System
*    Scheduler implementation
*/
#include <kernel/pm/scheduler.h>
#include <kernel/kernel.h>
#include <kernel/mm/virt_memory.h>
#include <kernel/mm/kheap.h>
#include <libk/string.h>

uint32_t pid_counter = 0; // last process id value. New process will be pid_counter++
uint32_t tid_counter = 0; // last thread id value. New thread will be tid_counter++
bool scheduler_enabled = false; // enable or disable the scheduler

process_t *kernel_process; // pointer to kernel process structure
thread_t *kernel_main_thread; // pointer to kernel's main thread structure

list_t *process_list; // this list contains all the processes runned in the system
// we dont need to have a global thread list beacuse each process already has its own thread list

process_t *current_process; // pointer to current running process structure
thread_t *current_thread; // pointer to current running thread structure


void scheduler_init()
{
	//uint32_t esp = 0;
    //asm volatile("mov %%esp, %0":"=a"(esp)); 
    asm volatile ("cli");

    process_list = list_create();

    kernel_process = (process_t*)kheap_malloc(sizeof(process_t));
    memset(kernel_process, 0, sizeof(process_t));

    kernel_process->pid = pid_counter++;
    kernel_process->state = PROCESS_INTERRUPTIBLE; // ????????
    strcpy(kernel_process->name, "kernel.elf");
    kernel_process->threads_count = 1;
    kernel_process->thread_list = list_create();
    kernel_process->page_dir = kernel_page_dir; // kernel_page_dir - pointer (physical) to kernel page dircetory structure it will be used for context switch (move its value to cr3)
    kernel_process->self_item = list_push(process_list, kernel_process);
    kernel_process->parent_proc_thread = NULL;


    //kernel_process created, now we create main kernel thread structure
    thread_t *kernel_main_thread = (thread_t*)kheap_malloc(sizeof(thread_t));
    memset(kernel_main_thread, 0, sizeof(thread_t));

    kernel_main_thread->tid = tid_counter++;
    kernel_main_thread->state = THREAD_READY; // thread is ready, not blocked beacuse thread become blocked only if it waits for some resource like file or infut etc.
    kernel_main_thread->privileges = THREAD_KERNEL_MODE;
    kernel_main_thread->process = kernel_process; // parent process is kernel
    kernel_main_thread->kernel_stack = kernel_stack_top_vaddr;
    kernel_main_thread->kernel_stack_size = 65536; // 64kib stack for main kernel thread was reserved in boot.s
    kernel_main_thread->self_item = list_push(kernel_process->thread_list, kernel_main_thread);
    //kernel_main_thread->entry_point = ???????;
    // TODO set kernel_main_thread regs - eip, esp , etc. !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    asm volatile ("sti");
}

void schedule()
{
    //
}