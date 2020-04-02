/*
*    EOS - Experimental Operating System
*    Scheduler implementation
*/
#include <kernel/pm/scheduler.h>
#include <kernel/kernel.h>
#include <kernel/mm/virt_memory.h>
#include <kernel/mm/kheap.h>
#include <libk/string.h>
#include <kernel/io/qemu_log.h>

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
    strcpy(kernel_main_thread->name, "kernel_main_thread");
    kernel_main_thread->privileges = THREAD_KERNEL_MODE;
    kernel_main_thread->process = kernel_process; // parent process is kernel
    kernel_main_thread->kernel_stack = kernel_stack_top_vaddr;
    kernel_main_thread->kernel_stack_size = 65536; // 64kib stack for main kernel thread was reserved in boot.s
    kernel_main_thread->self_item = list_push(kernel_process->thread_list, kernel_main_thread);
    //kernel_main_thread->entry_point = ???????;

    current_process = kernel_process;
    current_thread = kernel_main_thread;

    // !!!!
    scheduler_enabled = false;


    asm volatile ("sti");

}

// !!!! UNUSED
void schedule(struct regs *r)//TODO schedule must receive context of the interrupted current thread
{
    //qemu_printf("interrupted context eax = %x\n", r->eax);
    //asm volatile ("cli");
    if (!scheduler_enabled)
    {
        //asm volatile ("sti");
        return;
    }
    
    //qemu_printf("schedule() called!");
    thread_t *next_thread; // pointer to thread to which we are going to switch
    if (current_thread->self_item->next) // if in the thread list of current process remains some other thread
    {
        next_thread = (thread_t*)current_thread->self_item->next->val;
    } else // else next thread to be run will be from other process
    {
        if (current_process->self_item->next) // if the current process was not last in the list
        {
            // next thread will be first thread of current_process's next
            next_thread = (thread_t*)list_peek_front( ((process_t*)(current_process->self_item->next->val))->thread_list );
        } else
        {
            // next thread will be first thread of first process in the process_list 
            next_thread = (thread_t*)list_peek_front( ((process_t*)list_peek_front(process_list))->thread_list );
        }
    }
    //qemu_printf("next_thread = %s, current_thread = %s\n", next_thread->name, current_thread->name);
    if (next_thread == current_thread) // if there are only one thread (main kernel thread)
    {
        //qemu_printf("only one thread\n");
        //asm volatile ("sti");
        return;
    }
    // save context of interrupted current thread that we have received, to current_thread structure
	current_thread->registers.eax = r->eax;
    current_thread->registers.ebx = r->ebx;
    current_thread->registers.ecx = r->ecx;
    current_thread->registers.edx = r->edx;
    current_thread->registers.esp = r->esp;
    current_thread->registers.ebp = r->ebp;
    current_thread->registers.esi = r->esi;
    current_thread->registers.edi = r->edi;
    current_thread->registers.eflags = r->eflags;
    qemu_printf("r->eflags = %x\n", r->eflags);
    uint32_t cur_cr3_val;
    asm volatile("mov %%cr3, %0" : "=r"(cur_cr3_val));
    current_thread->registers.cr3 = cur_cr3_val;
    current_thread->registers.eip = r->eip;

    // switch current_thread to next_thread and switch current_process to next_thread's parent process
    current_thread = next_thread;
    current_process = next_thread->process;

    // TODO switch page directory to next_thread's

    // now we write saved context of next_thread to the actual cpu registers
    qemu_printf("switching kernelmode thread context to  eax = %x  ebx = %x  ecx = %x  edx = %x  esp = %x  ebp = %x  eip = %x  \n", current_thread->registers.eax, current_thread->registers.ebx, current_thread->registers.ecx, current_thread->registers.edx, current_thread->registers.esp, current_thread->registers.ebp, current_thread->registers.eip);


    /* // If the IDT entry that was invoked was greater than 40, sends an EOI
    // to the slave controller
    qemu_printf("r->idx_index = %d   r->eip = %x\n", r->idt_index, r->eip);
    if (r->idt_index >= 40)
    {
        outb(0xA0, 0x20);
    }
    // Sends an EOI to the master interrupt controller
    outb(0x20, 0x20);  */

    //asm volatile ("cli");
    
    // now there is NO kernel_regs_switch
    //kernel_regs_switch(&(next_thread->registers));
}