.macro no_error_code_handler idt_index
.global interrupt_handler_\idt_index
interrupt_handler_\idt_index:
    cli
    push $0
    push $\idt_index
    jmp  default_interrupt_handler
.endm

.macro error_code_handler idt_index
.global interrupt_handler_\idt_index
interrupt_handler_\idt_index:
    cli
    push $\idt_index
    jmp  default_interrupt_handler
.endm

.extern irq_handler

default_interrupt_handler:
    pusha
    push %ds
    push %es
    push %fs
    push %gs
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %esp, %eax  # Push us the stack
    push %eax
    call run_interrupt_handler # A special call, preserves the 'eip' register
    pop %eax
    pop %gs
    pop %fs
    pop %es
    pop %ds
    popa
    add $8, %esp   # Cleans up the pushed error code and pushed ISR number
    sti
    iret           # pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP!

# ISRs
no_error_code_handler 0
no_error_code_handler 1
no_error_code_handler 2
no_error_code_handler 3
no_error_code_handler 4
no_error_code_handler 5
no_error_code_handler 6
no_error_code_handler 7
error_code_handler 8
no_error_code_handler 9
error_code_handler 10
error_code_handler 11
error_code_handler 12
error_code_handler 13
error_code_handler 14
no_error_code_handler 15
no_error_code_handler 16
error_code_handler 17
no_error_code_handler 18
no_error_code_handler 19

no_error_code_handler 20
no_error_code_handler 21
no_error_code_handler 22
no_error_code_handler 23
no_error_code_handler 24
no_error_code_handler 25
no_error_code_handler 26
no_error_code_handler 27
no_error_code_handler 28
no_error_code_handler 29
no_error_code_handler 30
no_error_code_handler 31



# IRQs why starts not from 0??
no_error_code_handler 32
no_error_code_handler 33
no_error_code_handler 34
no_error_code_handler 35
no_error_code_handler 36
no_error_code_handler 37
no_error_code_handler 38
no_error_code_handler 39
no_error_code_handler 40
no_error_code_handler 41
no_error_code_handler 42
no_error_code_handler 43
no_error_code_handler 44
no_error_code_handler 45
no_error_code_handler 46
no_error_code_handler 47