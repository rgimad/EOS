/*
*    EOS - Experimental Operating System
*    Threads implementation
*/
#include <kernel/pm/thread.h>

#include <kernel/pm/scheduler.h>
#include <kernel/mm/virt_memory.h>
#include <kernel/mm/kheap.h>
#include <kernel/mm/uheap.h>

#include <kernel/io/qemu_log.h>


thread_t* create_user_thread(process_t *parent_proc, void *entry_point)
{
    thread_t *new_uthread = (thread_t*)kheap_malloc(sizeof(thread_t));
    memset(new_uthread, 0, sizeof(thread_t));
    parent_proc->threads_count++;
    new_uthread->self_item = list_push(parent_proc->thread_list, new_uthread);
    new_uthread->tid = tid_counter++;
    new_uthread->state = THREAD_READY;
    new_uthread->privileges = THREAD_USER_MODE;
    new_uthread->process = parent_proc;
    new_uthread->entry_point = entry_point;
    new_uthread->registers.eip = (uint32_t)entry_point; // sset up the instruction pointer to given entry point
    new_uthread->registers.eflags = 0x206; // enable interrupt. TODO: why ?????

    // allocate and set the top of the kernel stack of new thread
	void *new_uthread_kernel_stack = kheap_malloc(THREAD_KSTACK_SIZE);
	memset(new_uthread_kernel_stack, 0, THREAD_KSTACK_SIZE);
	new_uthread->kernel_stack = new_uthread_kernel_stack;

    // switch to parent procs page dir to use the user heap manager to allocate user stack for new_uthread
    vmm_switch_page_directory(parent_proc->page_dir);
    void *new_uthread_user_stack = uheap_malloc(parent_proc, THREAD_USTACK_SIZE);
    memset(new_uthread_user_stack, 0, THREAD_USTACK_SIZE);
    vmm_switch_page_directory(kernel_page_dir);
    new_uthread->user_stack = new_uthread_user_stack;

    return new_uthread;
}



thread_t* create_kernel_thread(void *entry_point) // TODO somwhere in this function kernel crashes
{
    qemu_printf("Starting creating kernel thread with entry point = %x\n", entry_point);
    thread_t *new_kthread = (thread_t*)kheap_malloc(sizeof(thread_t));
    memset(new_kthread, 0, sizeof(thread_t));
    qemu_printf("memory for new_kthread allocated...\n");
    kernel_process->threads_count++;
    qemu_printf("kernel_process->threads_count++; worked successfully...\n");
    new_kthread->self_item = list_push(kernel_process->thread_list, new_kthread);
    qemu_printf("new_kthread pushed to list...\n");
    new_kthread->tid = tid_counter++;
    new_kthread->state = THREAD_READY;
    new_kthread->privileges = THREAD_KERNEL_MODE;
    new_kthread->process = kernel_process;
    new_kthread->entry_point = entry_point;
    new_kthread->registers.eip = (uint32_t)entry_point; // sset up the instruction pointer to given entry point
    new_kthread->registers.eflags = 0x206; // enable interrupt. TODO: why ?????
    qemu_printf("new_kthread filled with values..\n");

    // allocate and set the top of the kernel stack of new thread
	void *new_kthread_kernel_stack = kheap_malloc(THREAD_KSTACK_SIZE);
	memset(new_kthread_kernel_stack, 0, THREAD_KSTACK_SIZE);
	new_kthread->kernel_stack = new_kthread_kernel_stack;

    qemu_printf("Kernel thread with entry point = %x was succesfully created\n", entry_point);
    return new_kthread;
}