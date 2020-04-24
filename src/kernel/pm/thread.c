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
#include <libk/string.h>


thread_t* create_user_thread(process_t *parent_proc, void *entry_point)
{
    //asm volatile ("cli");

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

    //asm volatile ("sti");

    return new_uthread;
}



thread_t* create_kernel_thread(void *entry_point) // TODO somwhere in this function kernel crashes
{
    asm volatile ("cli");

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
    strcpy(new_kthread->name, "newthread");
    new_kthread->privileges = THREAD_KERNEL_MODE;
    new_kthread->process = kernel_process;
    new_kthread->entry_point = entry_point;
    new_kthread->registers.eip = (uint32_t)entry_point; // set up the instruction pointer to given entry point
    new_kthread->registers.eflags = 0x206; // enable interrupt. TODO: why ?????
    qemu_printf("new_kthread filled with values..\n");

    // allocate and set the top of the kernel stack of new thread
	void *new_kthread_kernel_stack = kheap_malloc(THREAD_KSTACK_SIZE);
	memset(new_kthread_kernel_stack, 0, THREAD_KSTACK_SIZE);
	new_kthread->kernel_stack = new_kthread_kernel_stack + THREAD_KSTACK_SIZE; // + THREAD_KSTACK_SIZE because stack grows downwards, new_kthread_kernel_stack is end of stack
    new_kthread->esp = new_kthread->kernel_stack - 28; // TODO: WHY -28 ?? if i write e.g. -12 system crashes
    
    // Prepare stack for new kernel thread
    uint32_t* stack = (uint32_t *)(new_kthread->esp);
	*--stack = 0x00000202; // eflags
	*--stack = 0x8; // cs , was 0x8
	*--stack = (uint32_t)entry_point; // eip
	*--stack = 0; // eax
	*--stack = 0; // ebx
	*--stack = 0; // ecx
	*--stack = 0; //edx
	*--stack = 0; //esi
	*--stack = 0; //edi
	*--stack = new_kthread->esp; //ebp
	*--stack = 0x10; // ds, was 0x10
	*--stack = 0x10; // es, was 0x10
	*--stack = 0x10; // fs, was 0x10
	*--stack = 0x10; // gs, was 0x10
    new_kthread->esp = stack;


    // create pointer to stack frame
	/*uint32_t* newthread_esp = (uint32_t*)(new_kthread_kernel_stack + THREAD_KSTACK_SIZE);

    uint32_t eflags;// = read_eflags();
    asm volatile ("pushf \n\t"
                            "pop %%eax\n\t"
                            "movl %%eax, %0\n\t"
                            :"=r"(eflags)
                            :
                            :"%eax"
                    );
	eflags |= (1 << 9);

	newthread_esp[-5] = (uint32_t)entry_point;
	newthread_esp[-7] = eflags;*/

    //*(uint32_t*)new_kthread->kernel_stack = (uint32_t)new_kthread->entry_point; // ??
    //new_kthread->kernel_stack -= 4; // ??

    //new_kthread->registers.esp = new_kthread->kernel_stack;

    qemu_printf("Kernel thread with entry point = %x  esp = %x  was succesfully created\n", entry_point, new_kthread->esp);

    asm volatile ("sti");

    return new_kthread;
}