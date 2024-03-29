/*
 * EOS - Experimental Operating System
 * IDT setup functions
 */

#include <kernel/idt.h>
#include <kernel/tty.h>
#include <kernel/io/ports.h>

#include <kernel/libk/string.h>

#include <kernel/devices/timer.h>

#define SET_IDT_ENTRY(idx) \
    set_idt_entry(idx, (uint32_t) &interrupt_handler_##idx,\
                  0x08, 0x8E);

#define DECLARE_INTERRUPT_HANDLER(i) void interrupt_handler_##i(void)

void interrupt_enable_all(void) {
    asm volatile("sti");
}

void interrupt_disable_all(void) {
    asm volatile("cli");
}

// Defines an IDT entry
struct idt_entry {
    uint16_t handler_lo;
    uint16_t sel;
    uint8_t always0;
    uint8_t flags;
    uint16_t handler_hi;
} __attribute__((packed));
typedef struct idt_entry idt_entry_t;

struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));
typedef struct idt_ptr idt_ptr_t;

// Declare an IDT of 256 entries
idt_entry_t idt[IDT_NUM_ENTRIES];
idt_ptr_t idtp;

// Function arch/i386/idt.S, loads IDT from a pointer to an idt_ptr
extern void idt_load(struct idt_ptr *idt_ptr_addr);

/* ISRs */
//void interrupt_handler_0(void) {};
DECLARE_INTERRUPT_HANDLER(0);
DECLARE_INTERRUPT_HANDLER(1);
DECLARE_INTERRUPT_HANDLER(2);
DECLARE_INTERRUPT_HANDLER(3);
DECLARE_INTERRUPT_HANDLER(4);
DECLARE_INTERRUPT_HANDLER(5);
DECLARE_INTERRUPT_HANDLER(6);
DECLARE_INTERRUPT_HANDLER(7);
DECLARE_INTERRUPT_HANDLER(8);
DECLARE_INTERRUPT_HANDLER(9);
DECLARE_INTERRUPT_HANDLER(10);
DECLARE_INTERRUPT_HANDLER(11);
DECLARE_INTERRUPT_HANDLER(12);
DECLARE_INTERRUPT_HANDLER(13);
DECLARE_INTERRUPT_HANDLER(14);
DECLARE_INTERRUPT_HANDLER(15);
DECLARE_INTERRUPT_HANDLER(16);
DECLARE_INTERRUPT_HANDLER(17);
DECLARE_INTERRUPT_HANDLER(18);
DECLARE_INTERRUPT_HANDLER(19);

DECLARE_INTERRUPT_HANDLER(20);
DECLARE_INTERRUPT_HANDLER(21);
DECLARE_INTERRUPT_HANDLER(22);
DECLARE_INTERRUPT_HANDLER(23);
DECLARE_INTERRUPT_HANDLER(24);
DECLARE_INTERRUPT_HANDLER(25);
DECLARE_INTERRUPT_HANDLER(26);
DECLARE_INTERRUPT_HANDLER(27);
DECLARE_INTERRUPT_HANDLER(28);
DECLARE_INTERRUPT_HANDLER(29);
DECLARE_INTERRUPT_HANDLER(30);
DECLARE_INTERRUPT_HANDLER(31);

/* IRQs */
DECLARE_INTERRUPT_HANDLER(32);
DECLARE_INTERRUPT_HANDLER(33);
DECLARE_INTERRUPT_HANDLER(34);
DECLARE_INTERRUPT_HANDLER(35);
DECLARE_INTERRUPT_HANDLER(36);
DECLARE_INTERRUPT_HANDLER(37);
DECLARE_INTERRUPT_HANDLER(38);
DECLARE_INTERRUPT_HANDLER(39);
DECLARE_INTERRUPT_HANDLER(40);
DECLARE_INTERRUPT_HANDLER(41);
DECLARE_INTERRUPT_HANDLER(42);
DECLARE_INTERRUPT_HANDLER(43);
DECLARE_INTERRUPT_HANDLER(44);
DECLARE_INTERRUPT_HANDLER(45);
DECLARE_INTERRUPT_HANDLER(46);
DECLARE_INTERRUPT_HANDLER(47);

// Eto fignya ya dobavil a potom zakommentil
/*DECLARE_INTERRUPT_HANDLER(48);
DECLARE_INTERRUPT_HANDLER(49);
DECLARE_INTERRUPT_HANDLER(50);
DECLARE_INTERRUPT_HANDLER(51);
DECLARE_INTERRUPT_HANDLER(52);
DECLARE_INTERRUPT_HANDLER(53);
DECLARE_INTERRUPT_HANDLER(54);
DECLARE_INTERRUPT_HANDLER(55);
DECLARE_INTERRUPT_HANDLER(56);
DECLARE_INTERRUPT_HANDLER(57);
DECLARE_INTERRUPT_HANDLER(58);
DECLARE_INTERRUPT_HANDLER(59);
DECLARE_INTERRUPT_HANDLER(60);
DECLARE_INTERRUPT_HANDLER(61);
DECLARE_INTERRUPT_HANDLER(62);
DECLARE_INTERRUPT_HANDLER(63);*/

DECLARE_INTERRUPT_HANDLER(64); // for int 0x40 (KOS syscalls) handler

/*DECLARE_INTERRUPT_HANDLER(65);
DECLARE_INTERRUPT_HANDLER(66);
DECLARE_INTERRUPT_HANDLER(67);
DECLARE_INTERRUPT_HANDLER(68);
DECLARE_INTERRUPT_HANDLER(69);
DECLARE_INTERRUPT_HANDLER(70);
DECLARE_INTERRUPT_HANDLER(71);
DECLARE_INTERRUPT_HANDLER(72);
DECLARE_INTERRUPT_HANDLER(73);
DECLARE_INTERRUPT_HANDLER(74);
DECLARE_INTERRUPT_HANDLER(75);
DECLARE_INTERRUPT_HANDLER(76);
DECLARE_INTERRUPT_HANDLER(77);
DECLARE_INTERRUPT_HANDLER(78);
DECLARE_INTERRUPT_HANDLER(79);
DECLARE_INTERRUPT_HANDLER(80);
DECLARE_INTERRUPT_HANDLER(81);
DECLARE_INTERRUPT_HANDLER(82);
DECLARE_INTERRUPT_HANDLER(83);
DECLARE_INTERRUPT_HANDLER(84);
DECLARE_INTERRUPT_HANDLER(85);
DECLARE_INTERRUPT_HANDLER(86);
DECLARE_INTERRUPT_HANDLER(87);
DECLARE_INTERRUPT_HANDLER(88);
DECLARE_INTERRUPT_HANDLER(89);
DECLARE_INTERRUPT_HANDLER(90);
DECLARE_INTERRUPT_HANDLER(91);
DECLARE_INTERRUPT_HANDLER(92);
DECLARE_INTERRUPT_HANDLER(93);
DECLARE_INTERRUPT_HANDLER(94);
DECLARE_INTERRUPT_HANDLER(95);
DECLARE_INTERRUPT_HANDLER(96);
DECLARE_INTERRUPT_HANDLER(97);
DECLARE_INTERRUPT_HANDLER(98);
DECLARE_INTERRUPT_HANDLER(99);
DECLARE_INTERRUPT_HANDLER(100);
DECLARE_INTERRUPT_HANDLER(101);
DECLARE_INTERRUPT_HANDLER(102);
DECLARE_INTERRUPT_HANDLER(103);
DECLARE_INTERRUPT_HANDLER(104);
DECLARE_INTERRUPT_HANDLER(105);
DECLARE_INTERRUPT_HANDLER(106);
DECLARE_INTERRUPT_HANDLER(107);
DECLARE_INTERRUPT_HANDLER(108);
DECLARE_INTERRUPT_HANDLER(109);
DECLARE_INTERRUPT_HANDLER(110);
DECLARE_INTERRUPT_HANDLER(111);
DECLARE_INTERRUPT_HANDLER(112);
DECLARE_INTERRUPT_HANDLER(113);
DECLARE_INTERRUPT_HANDLER(114);
DECLARE_INTERRUPT_HANDLER(115);
DECLARE_INTERRUPT_HANDLER(116);
DECLARE_INTERRUPT_HANDLER(117);
DECLARE_INTERRUPT_HANDLER(118);
DECLARE_INTERRUPT_HANDLER(119);
DECLARE_INTERRUPT_HANDLER(120);
DECLARE_INTERRUPT_HANDLER(121);
DECLARE_INTERRUPT_HANDLER(122);
DECLARE_INTERRUPT_HANDLER(123);
DECLARE_INTERRUPT_HANDLER(124);
DECLARE_INTERRUPT_HANDLER(125);
DECLARE_INTERRUPT_HANDLER(126);
DECLARE_INTERRUPT_HANDLER(127);*/
DECLARE_INTERRUPT_HANDLER(128); // for syscalls

void set_idt_entry(uint8_t num, uint64_t handler, uint16_t sel, uint8_t flags) {
    idt[num].handler_lo = handler & 0xFFFF;
    idt[num].handler_hi = (handler >> 16) & 0xFFFF;
    idt[num].always0 = 0;
    idt[num].flags = flags;
    idt[num].sel = sel;
}

void irq_set_mask(uint8_t irq_line) {
    uint16_t port;
    uint8_t value;

    if (irq_line < 8) {
        port = 0x21; // master pic data port
    } else {
        port = 0xA1; // slave pic data port
        irq_line -= 8;
    }

    value = inb(port) | (1 << irq_line);
    outb(port, value);
}

void irq_clear_mask(uint8_t irq_line) {
    uint16_t port;
    uint8_t value;

    if (irq_line < 8) {
        port = 0x21; // master pic data port
    } else {
        port = 0xA1; // slave pic data port
        irq_line -= 8;
    }

    value = inb(port) & ~(1 << irq_line);
    outb(port, value);
}

void remap_pics(void) {
    // 0x20 - master pic command port
    // 0x21 - master pic data port
    // 0xA0 - slave pic command port
    // 0xA1 - slave pic data port
    outb(0x20, 0x11); // start the initialization sequence (in cascade mode)
    outb(0xA0, 0x11);
    outb(0x80, 0); // small delay for pic remapping, for old hardware

    outb(0x21, 0x20); // ICW2: Master PIC vector offset
    outb(0xA1, 0x28); // ICW2: Slave PIC vector offset
    outb(0x80, 0);

    outb(0x21, 0x04); // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
    outb(0xA1, 0x02); // ICW3: tell Slave PIC its cascade identity (0000 0010)
    outb(0x80, 0);

    outb(0x21, 0x01); // 8086/88 (MCS-80/85) mode
    outb(0xA1, 0x01);
    outb(0x80, 0);
}

// Installs the IDT
void idt_install(void) {
    remap_pics();

    // Sets the special IDT pointer up
    idtp.limit = (sizeof(struct idt_entry) * IDT_NUM_ENTRIES) - 1;
    idtp.base = (uint32_t) &idt;

    // tty_printf("idtp.base = %x, idtp.limit = %x\n", idtp.base, idtp.limit);

    // Clear out the entire IDT, initializing it to zeros
    memset(&idt, 0, sizeof(struct idt_entry) * IDT_NUM_ENTRIES);

    idt_load(&idtp);

    // ISRs
    SET_IDT_ENTRY(0);
    SET_IDT_ENTRY(1);
    SET_IDT_ENTRY(2);
    SET_IDT_ENTRY(3);
    SET_IDT_ENTRY(4);
    SET_IDT_ENTRY(5);
    SET_IDT_ENTRY(6);
    SET_IDT_ENTRY(7);
    SET_IDT_ENTRY(8);
    SET_IDT_ENTRY(9);
    SET_IDT_ENTRY(10);
    SET_IDT_ENTRY(11);
    SET_IDT_ENTRY(12);
    SET_IDT_ENTRY(13);
    SET_IDT_ENTRY(14);
    SET_IDT_ENTRY(15);
    SET_IDT_ENTRY(16);
    SET_IDT_ENTRY(17);
    SET_IDT_ENTRY(18);
    SET_IDT_ENTRY(19);
    SET_IDT_ENTRY(20);
    SET_IDT_ENTRY(21);
    SET_IDT_ENTRY(22);
    SET_IDT_ENTRY(23);
    SET_IDT_ENTRY(24);
    SET_IDT_ENTRY(25);
    SET_IDT_ENTRY(26);
    SET_IDT_ENTRY(27);
    SET_IDT_ENTRY(28);
    SET_IDT_ENTRY(29);
    SET_IDT_ENTRY(30);
    SET_IDT_ENTRY(31);

    SET_IDT_ENTRY(32);
    // Install scheduler by timer interrupt
    // set_idt_entry(TIMER_IDT_INDEX, (uint32_t) &task_switch, 0x08, 0x8E);
    // timer_set_frequency(TICKS_PER_SECOND);

    SET_IDT_ENTRY(33);
    SET_IDT_ENTRY(34);
    SET_IDT_ENTRY(35);
    SET_IDT_ENTRY(36);
    SET_IDT_ENTRY(37);
    SET_IDT_ENTRY(38);
    SET_IDT_ENTRY(39);
    SET_IDT_ENTRY(40);
    SET_IDT_ENTRY(41);
    SET_IDT_ENTRY(42);
    SET_IDT_ENTRY(43);
    SET_IDT_ENTRY(44);
    SET_IDT_ENTRY(45);
    SET_IDT_ENTRY(46);
    SET_IDT_ENTRY(47);

    /*SET_IDT_ENTRY(48);
    SET_IDT_ENTRY(49);
    SET_IDT_ENTRY(50);
    SET_IDT_ENTRY(51);
    SET_IDT_ENTRY(52);
    SET_IDT_ENTRY(53);
    SET_IDT_ENTRY(54);
    SET_IDT_ENTRY(55);
    SET_IDT_ENTRY(56);
    SET_IDT_ENTRY(57);
    SET_IDT_ENTRY(58);
    SET_IDT_ENTRY(59);
    SET_IDT_ENTRY(60);
    SET_IDT_ENTRY(61);
    SET_IDT_ENTRY(62);
    SET_IDT_ENTRY(63);*/

    SET_IDT_ENTRY(64); // for int 0x40 (KOS syscalls) handler

    /* SET_IDT_ENTRY(65);
    SET_IDT_ENTRY(66);
    SET_IDT_ENTRY(67);
    SET_IDT_ENTRY(68);
    SET_IDT_ENTRY(69);
    SET_IDT_ENTRY(70);
    SET_IDT_ENTRY(71);
    SET_IDT_ENTRY(72);
    SET_IDT_ENTRY(73);
    SET_IDT_ENTRY(74);
    SET_IDT_ENTRY(75);
    SET_IDT_ENTRY(76);
    SET_IDT_ENTRY(77);
    SET_IDT_ENTRY(78);
    SET_IDT_ENTRY(79);
    SET_IDT_ENTRY(80);
    SET_IDT_ENTRY(81);
    SET_IDT_ENTRY(82);
    SET_IDT_ENTRY(83);
    SET_IDT_ENTRY(84);
    SET_IDT_ENTRY(85);
    SET_IDT_ENTRY(86);
    SET_IDT_ENTRY(87);
    SET_IDT_ENTRY(88);
    SET_IDT_ENTRY(89);
    SET_IDT_ENTRY(90);
    SET_IDT_ENTRY(91);
    SET_IDT_ENTRY(92);
    SET_IDT_ENTRY(93);
    SET_IDT_ENTRY(94);
    SET_IDT_ENTRY(95);
    SET_IDT_ENTRY(96);
    SET_IDT_ENTRY(97);
    SET_IDT_ENTRY(98);
    SET_IDT_ENTRY(99);
    SET_IDT_ENTRY(100);
    SET_IDT_ENTRY(101);
    SET_IDT_ENTRY(102);
    SET_IDT_ENTRY(103);
    SET_IDT_ENTRY(104);
    SET_IDT_ENTRY(105);
    SET_IDT_ENTRY(106);
    SET_IDT_ENTRY(107);
    SET_IDT_ENTRY(108);
    SET_IDT_ENTRY(109);
    SET_IDT_ENTRY(110);
    SET_IDT_ENTRY(111);
    SET_IDT_ENTRY(112);
    SET_IDT_ENTRY(113);
    SET_IDT_ENTRY(114);
    SET_IDT_ENTRY(115);
    SET_IDT_ENTRY(116);
    SET_IDT_ENTRY(117);
    SET_IDT_ENTRY(118);
    SET_IDT_ENTRY(119);
    SET_IDT_ENTRY(120);
    SET_IDT_ENTRY(121);
    SET_IDT_ENTRY(122);
    SET_IDT_ENTRY(123);
    SET_IDT_ENTRY(124);
    SET_IDT_ENTRY(125);
    SET_IDT_ENTRY(126);
    SET_IDT_ENTRY(127);*/
    SET_IDT_ENTRY(128); // Need for system calls - int 0x80 , 0x80 = 128 in decimal

    irq_set_mask(0); // mask PIT
    // unmask others
    for (int i = 1; i < 16; ++i) {
        irq_clear_mask(i);
    }
}
