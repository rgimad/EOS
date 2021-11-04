/*
 * EOS - Experimental Operating System
 * Kernel main source file
 */

#include <kernel/kernel.h>

// Then include kernel subsystem's headers
#include <kernel/vga.h>
#include <kernel/tty.h>
#include <kernel/ksh.h>
#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/tss.h>
#include <kernel/interrupts.h>
#include <kernel/cpu_detect.h>

// Devices
#include <kernel/devices/timer.h>
#include <kernel/devices/keyboard.h>
#include <kernel/devices/mouse.h>

// Memory manager
#include <kernel/mm/phys_memory.h>
#include <kernel/mm/virt_memory.h>
#include <kernel/mm/kheap.h>

// File system
#include <kernel/fs/vfs.h>
#include <kernel/fs/initrd.h>

// Syscalls
#include <kernel/syscall.h>

// Process manager
#include <kernel/pm/elf.h>

// Graphics
#include <kernel/graphics/vesafb.h>

uint32_t kernel_stack_top_vaddr;

int kernel_init(struct multiboot_info *mboot_info) {
    tty_init();
    svga_mode_info_t *svga_mode = (svga_mode_info_t*) mboot_info->vbe_mode_info;
    framebuffer_addr = svga_mode->physbase; //vmm_temp_map_page(svga_mode->physbase);
    framebuffer_pitch = svga_mode->pitch;
    framebuffer_bpp = svga_mode->bpp;
    framebuffer_width = svga_mode->screen_width;
    framebuffer_height = svga_mode->screen_height;
    //framebuffer_size = framebuffer_width * framebuffer_height * (framebuffer_bpp / 8);
    framebuffer_size = framebuffer_height * framebuffer_pitch;
    back_framebuffer_addr = framebuffer_addr;

    //higher_half_test();

    //install Global Descriptor Table
    //tty_printf("Installing GDT...\n");
    gdt_install();

    //install Interrupt Descriptor Table
    //tty_printf("Installing IDT...\n\n");
    idt_install();

    //uint32_t initrd_beg = *(uint32_t*) (mboot_info->mods_addr);
    //uint32_t initrd_end = *(uint32_t*) (mboot_info->mods_addr + 4);

    //tty_printf("Some multiboot info:\nMagic number = %x\n", magic_number);
    //tty_printf("\nRAM available = %d MB\n", (mboot_info->mem_lower + mboot_info->mem_upper) / 1024);
    //tty_printf("\n");

    //parse_memory_map((memory_map_entry*) mboot_info->mmap_addr, mboot_info->mmap_length);
    pmm_init(mboot_info);
    //pmm_test();

    uint32_t initrd_beg = *(uint32_t*) (mboot_info->mods_addr);
    uint32_t initrd_end = *(uint32_t*) (mboot_info->mods_addr + 4);

    //tty_printf("pde0 = %u\n", *(page_dir_entry*)(0xFFFFF000));
    //tty_printf("pde_%d = %u\n", (0xC0000000 >> 22), *(page_dir_entry*)(0xFFFFF000 + (0xC0000000 >> 22)*4));

    //uint32_t x = *(uint32_t*) (0xF0FFF000); //doenst cause page fault if in boot.s was identity mapped all the ram :D

    vmm_init();
    //vmm_test();

    kheap_init();
    //kheap_test();

    //tty_init();
    init_vbe(mboot_info);

    /*back_framebuffer_addr = kheap_malloc(framebuffer_size);
    tty_printf("back_framebuffer_addr = %x\n", back_framebuffer_addr);
    tty_printf("init_vbe: [c0800000]->%x\n", page_table_entry_is_writable(GET_PTE(0xC0800000)));
    memset(back_framebuffer_addr, 0, framebuffer_size); //causes page fault at c0800000*/

    tty_printf("framebuffer_addr = %x\n", framebuffer_addr);
    tty_printf("framebuffer_size = %x\n", framebuffer_size);
    //tty_printf("x = %x\n", (framebuffer_pitch * VESA_HEIGHT));
    //tty_printf("VESA_HEIGHT = %d\n", VESA_HEIGHT);
    //tty_printf("\n\n");

    //memcpy(framebuffer_addr, back_framebuffer_addr, framebuffer_size);

    vfs_init();
    initrd_init(initrd_beg, initrd_end);

    //disable interrupts while kernel init
    interrupt_disable_all();

    timer_install();
    keyboard_install();
    mouse_install();

    syscall_init();

    //enable interrutps
    //tty_printf("Enabling all interrupts...\n\n");
    interrupt_enable_all();

    return 1;
}

void higher_half_test() {
    int eip, esp;
    //asm("movl %%ebx,%0" : "=r"(eip_val));
    tty_printf("Hello from higher half! ");

    asm volatile("1: lea 1b, %0;": "=a"(eip));
    tty_printf("EIP = %x  ", eip);

    asm("movl %%esp,%0" : "=r"(esp));
    tty_printf("ESP = %x  \n", esp);
}

void kernel_main(int magic_number, struct multiboot_info *mboot_info) { // Arguments are passed by _start in boot.s
    asm("movl %%esp,%0" : "=r"(kernel_stack_top_vaddr)); // TODO is it unused????

    // Initilize the kernel
    multiboot_info mboot_info_copy = *mboot_info;
    kernel_init(&mboot_info_copy);

    // TODO: 172-174 is it normal to place these lines of code here????????????????????????????????????????????????????????
    // Set TSS stack so that when process return from usermode to kernel mode, the kernel have a ready-to-use stack
    uint32_t esp;
    asm volatile("mov %%esp, %0" : "=r"(esp));
    tss_set_stack(0x10, esp);

    scheduler_init();

    // init the kernel debug shell (ksh)
    ksh_init();

    //elf_info("/initrd/first_program_gas.elf");
    //run_elf_file("/initrd/first_program_gas.elf");

    //run_elf_file("/initrd/reverse.elf");

    // run ksh main function
    ksh_main();

    for (; ; ) {
        asm("hlt");
    }
}
