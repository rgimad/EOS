/*
*    EOS - Experimental Operating System
*    Interrupt management
*/
#include <kernel/interrupts.h>
#include <kernel/io/ports.h>
#include <kernel/idt.h>
#include <kernel/tty.h>

#include <kernel/io/qemu_log.h>


static interrupt_handler_t interrupt_handlers[IDT_NUM_ENTRIES];

bool register_interrupt_handler(uint32_t idt_index, interrupt_handler_t handler)
{
    if (idt_index >= IDT_NUM_ENTRIES)
    {
        return false;
    }

    if (interrupt_handlers[idt_index] != NULL)
    {
        return false;
    }

    interrupt_handlers[idt_index] = handler;
    return true;
}

void fault_handler(struct regs *r)
{
    //void * linearAddress;
    // retrieve the linear address of the page fault stored in CR2
    //ASM( "movl %%cr2, %0" : "=r" (linearAddress) );
    //asm volatile( "movl %cr2, %eax");
    //for (;;);
    uint32_t adr;
    asm volatile( "movl %%cr2, %0" : "=r" (adr) );
    qemu_printf("System Exception. System Halted! cr2 = %x  r->idt_index = %x eax = %x  ebx = %x  ecx = %x  edx = %x  esp = %x  ebp = %x  eip = %x\n", adr, r->idt_index, r->eax, r->ebx, r->ecx, r->edx, r->esp, r->ebp, r->eip);
    for (;;);
}

void irq_handler(struct regs *r)
{
    //if (r->idt_index != 32) tty_printf("idt_index = %d\n", r->idt_index);
    // Blank function pointer
    void (*handler)(struct regs * r);

    // If there's a custom handler to handle the IRQ, handle it
    handler = interrupt_handlers[r->idt_index];
    if (handler)
    {
        handler(r);
    }

    // If the IDT entry that was invoked was greater than 40, sends an EOI
    // to the slave controller
    if (r->idt_index >= 40)
    {
        outb(0xA0, 0x20);
    }

    // Sends an EOI to the master interrupt controller
    outb(0x20, 0x20);
}

void run_interrupt_handler(struct regs* r)
{
    size_t idt_index = r->idt_index;
    if (idt_index < 32)
    {
        fault_handler(r);
        return;
    }

    if (idt_index >= 32 && idt_index <= 47)
    {
        irq_handler(r);
        return;
    }

    if (interrupt_handlers[r->idt_index] != NULL)
    {
        interrupt_handlers[r->idt_index](r);
    }
}
