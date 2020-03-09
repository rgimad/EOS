#
#    EOS - Experimental Operating System
#    Context (register data) switching functions
#

/*
in EOS:

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

in EduOS:

typedef struct context {
    uint32_t eax; // 0
    uint32_t ecx; // 4
    uint32_t edx; // 8
    uint32_t ebx; // 12
    uint32_t esp; // 16
    uint32_t ebp; // 20
    uint32_t esi; // 24
    uint32_t edi; // 28
    uint32_t eflags; //32
    uint32_t cr3; // 36
    uint32_t eip; //40
}context_t;

*/


#  C signature: void kernel_regs_switch(context_t *ctx);
.global kernel_regs_switch
kernel_regs_switch: # switch to kernelmode thread.
    # Now load general registers
    # +4 because of skip return address, and get the pointer to context_t struct
    movl 4(%esp), %ebp   # now ebp contains pointer to received context_t structure
    movl 4(%ebp), %ebx   # ebx = context->ebx ...
    movl 8(%ebp), %ecx
    movl 12(%ebp), %edx
    movl 24(%ebp), %esi
    movl 28(%ebp), %edi

    # load eflags
    movl 32(%ebp), %eax
    pushl %eax
    popfl
    
    # right now, eax, ebp, esp are not restored yet

    # TODO what we need to do to switch to kernel mode? To user mode?

    # Enter usermode from here(make sure the registers are restored correctly for the user process !) TODO what is it????? 
    # mov ax, 0x10
    # mov ds, ax
    # mov es, ax
    # mov fs, ax
    # mov gs, ax
    
    push $0x10 # TODO what is 0x10 and why we push it?

    # Push threads esp
    movl 16(%ebp), %eax
    push %eax
    pushfl
    push $0x08 # TODO what is it ??????????
    # push eip
    movl 40(%ebp), %eax
    push %eax
    # Enter usermode from here(make sure the registers are restored correctly for the user process !) TODO what is it?????

    # restore eax
    movl (%ebp), %eax

    # now, restore ebp
    movl 20(%ebp), %ebp
    sti
    iret
