/*
 * EOS - Experimental Operating System
 * Interrupt management
 */

#include <kernel/interrupts.h>
#include <kernel/io/ports.h>
#include <kernel/idt.h>
#include <kernel/tty.h>

#include <kernel/io/qemu_log.h>

static interrupt_handler_t interrupt_handlers[IDT_NUM_ENTRIES];

bool register_interrupt_handler(uint32_t idt_index, interrupt_handler_t handler)
{
    if (idt_index >= IDT_NUM_ENTRIES) {
        return false;
    }

    if (interrupt_handlers[idt_index] != NULL) {
        return false;
    }

    interrupt_handlers[idt_index] = handler;
    return true;
}

void fault_handler(struct regs *r)
{
    uint32_t cr2_value;
    asm volatile("movl %%cr2, %0"
                 : "=r"(cr2_value));
    qemu_printf("System Exception. System Halted!\n"
        "    cr2 = %#08X\n"
        "    r->idt_index = %u\n"
        "    eax = %#08X\n"
        "    ebx = %#08X\n"
        "    ecx = %#08X\n"
        "    edx = %#08X\n"
        "    esp = %#08X\n"
        "    ebp = %#08X\n"
        "    eip = %#08X\n",
        cr2_value,
        r->idt_index,
        r->eax, r->ebx, r->ecx, r->edx, r->esp, r->ebp, r->eip);
    for (;;)
        ;
}

void irq_handler(struct regs *r)
{
    // qemu_printf("INT %u\n", r->idt_index);
    //if (r->idt_index != 32) tty_printf("idt_index = %d\n", r->idt_index);
    // Blank function pointer
    void (*handler)(struct regs * r);

    // If there's a custom handler to handle the IRQ, handle it
    handler = interrupt_handlers[r->idt_index];
    if (handler) {
        handler(r);
    }

    // If the IDT entry that was invoked was greater than 40, sends an EOI
    // to the slave controller
    if (r->idt_index >= 40) {
        outb(0xA0, 0x20);
    }

    // Sends an EOI to the master interrupt controller
    outb(0x20, 0x20);
}

void run_interrupt_handler(struct regs *r)
{
    // tty_printf("int %x\n", r->idt_index);
    size_t idt_index = r->idt_index;
    if (idt_index < 32) {
        fault_handler(r);
        return;
    }

    if (idt_index >= 32 && idt_index <= 47) {
        irq_handler(r);
        return;
    }

    if (interrupt_handlers[r->idt_index] != NULL) {
        interrupt_handlers[r->idt_index](r);
    }
}
