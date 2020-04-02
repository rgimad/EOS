#
#    EOS - Experimental Operating System
#    Context switching functions
#
#  --------------------------------------------------------------------------

.extern		current_thread
.extern     current_process
.extern     process_list
.extern		kernel_tss  # is its structure correct for code below ??
.extern     scheduler_enabled

.extern list_peek_front

.extern com1_write_char


# ---------------------------------------------------------------------------
#       void task_switch(void)
# ---------------------------------------------------------------------------
.global		task_switch

task_switch:

	push	%ebp 				# C-function prologue
    
    pushf						# Save EFLAGS in thread stack
    cli							# Disable all interrupts

    cmp $0, scheduler_enabled
    je restore_context         #  if (!scheduler_enabled) goto restore_context;
    
    # edx = current_thread
    mov	current_thread, %edx

    # Save current task's ESP
    mov	%esp, 12(%edx)         #  current_thread->esp = esp;

    # Get next thread to eax:

    mov 16(%edx), %eax         #  eax = current_thread->self_item
    mov 4(%eax), %eax          #  eax = current_thread->self_item->next

    test %eax, %eax            #  compare if eax is 0
    jz get_thread_from_other_process

    get_thread_from_current_process:
        mov 8(%eax), %eax      # eax = eax->val so now eax = current_thread->self_item->next->val
        jmp endif1
    
    get_thread_from_other_process:
        mov current_process, %ebx  # ebx = current_process
        mov 12(%ebx), %ebx     # ebx = current_process->self_item
        mov 4(%ebx), %ebx      # ebx = current_process->self_item->next
        test %ebx, %ebx
        jz get_thread_from_first_process

        get_thread_from_next_process:
            mov 8(%ebx), %ebx  # ebx = current_process->self_item->next->val
            mov 20(%ebx), %ebx # ebx = ((process_t*)(current_process->self_item->next->val))->thread_list
            push %ebx
            call list_peek_front
            pop %ebx
            # now eax = (thread_t*)list_peek_front( ((process_t*)(current_process->self_item->next->val))->thread_list )
            jmp endif1
        
        get_thread_from_first_process:
            mov process_list, %ebx
            push %ebx
            call list_peek_front
            pop %ebx
            mov 20(%eax), %eax # eax = ((process_t*)list_peek_front(process_list))->thread_list
            mov %eax, %ebx
            push %ebx
            call list_peek_front
            pop %ebx
            # now eax = (thread_t*)list_peek_front( ((process_t*)list_peek_front(process_list))->thread_list )

    endif1: 

    mov	current_thread, %edx
    cmp %eax, %edx
    je restore_context # if (next_thread == current_thread) return;      

    perform_switch:
    # following 3 lines cause esp corruption - why???
    # pushl $49
    # call com1_write_char
    # add 0x4, %esp
    
    # below assume that eax is pointer to next thread structure
    
    # Set page directory

    mov current_process, %ebx   # current_thread->process --> EBX
    mov 24(%ebx), %ecx          # process->page_dir --> ECX 
    mov %ecx, %cr3              # reload CR3 
    
    # Set new ESP 
    mov 12(%eax), %esp			
    
    # Set TSS kernel stack
    
    # mov 24(%edx), %eax			# current_thread->stack_top --> EAX
    # mov $kernel_tss, %edx		# tss address --> EDX   40??
    # mov %eax, 4(%edx)			#  EAX --> tss.esp		
    
    restore_context:
        popf					#  Restore EFLAGS from new task stack
        
        pop		%ebp	        # C-function epilog    
        
        ret    # TODO ! now it jumps directly to newly created khread, but is must return to the end of irq_handler which sends EOI and return to the end of run_interrupt_handler which returns to the end of default_interrupt_handler which will restore registers from next_kthread stack and jump to its saved eip

# ------------------------------------------------------------------------------------

.global     start_scheduler
start_scheduler:       
    mov     $1, %eax
    xchg    %eax, scheduler_enabled    

    ret

.global     stop_scheduler
stop_scheduler:
    xor     %eax, %eax
    xchg    %eax, scheduler_enabled   
    
    ret

# fmt1:
#    .string "task_switch called\n"
           
