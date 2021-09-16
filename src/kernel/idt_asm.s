# Loads the IDT defined in '_idtp' into the processor.
# This is declared in C as 'extern void idt_load();'
.global idt_load

idt_load:
    mov 4(%esp), %eax
    lidt (%eax)
    ret
