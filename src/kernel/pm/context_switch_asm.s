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
    cli
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
    push %eax
    popf
    
    # right now, eax, ebp, esp are not restored yet

    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    # push ss selector
    push $0x10

    # Push threads esp
    movl 16(%ebp), %eax
    push %eax

    # push eflags
    pushf
    
    # push cs selector
    push $0x08

    # push eip
    movl 40(%ebp), %eax
    push %eax

    # restore eax
    movl (%ebp), %eax

    # now, restore ebp
    movl 20(%ebp), %ebp

    # sending EOI (end of interrupt) signal to pic
    push %ax
    mov $0x20, %ax
    outb %ax, $0x20
    pop %ax

    #sti
    iret  # pops 5 things at once: CS, EIP, EFLAGS, ESP and SS!