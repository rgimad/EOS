/*
*    EOS - Experimental Operating System
*    Kernel main source file
*/

#include <kernel/kernel.h>

//then include kernel subsystem's headers
#include <kernel/vga.h>
#include <kernel/tty.h>
#include <kernel/ksh.h>
#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/interrupts.h>
#include <kernel/cpu_detect.h>

//devices
#include <kernel/devices/timer.h>
#include <kernel/devices/keyboard.h>

//memory manager
#include <kernel/mm/phys_memory.h>
#include <kernel/mm/virt_memory.h>
#include <kernel/mm/kheap.h>

//graphics
#include <kernel/graphics/vesafb.h>



int kernel_init(struct multiboot_info *mboot_info)
{

	//higher_half_test();

	//tty_printf("                  ");tty_printf(EOS_VERSION_STRING);tty_printf("\n\n");
	//tty_printf("                  ");tty_putstring_color(EOS_VERSION_STRING, VESA_GREEN);tty_printf("\n");

	//tty_printf("Terminal is ready.\n");

	//install Global Descriptor Table
	//tty_printf("Installing GDT...\n");
    gdt_install();

    //install Interrupt Descriptor Table
    //tty_printf("Installing IDT...\n\n");
    idt_install();

    //tty_printf("Some multiboot info:\nMagic number = %x\n", magic_number);
    //tty_printf("\nRAM available = %d MB\n", (mboot_info->mem_lower + mboot_info->mem_upper)/1024);
	//tty_printf("\n");
	
    //parse_memory_map((memory_map_entry*)mboot_info->mmap_addr, mboot_info->mmap_length);
    pmm_init(mboot_info);
    //pmm_test();

    //tty_printf("pde0 = %u\n", *(page_dir_entry*)(0xFFFFF000));
    //tty_printf("pde_%d = %u\n", (0xC0000000 >> 22), *(page_dir_entry*)(0xFFFFF000 + (0xC0000000 >> 22)*4));
    
    vmm_init();
    //vmm_test();

    init_vbe(mboot_info);
	tty_init();

    kheap_init();
    //kheap_test();

    tty_printf("\n\n");

    //draw_vertical_line(50, 50, 100, 0xFFFFFF);
    //draw_fill(50, 50, 100, 100, 0x0000AA);

    /*draw_fill(0, 500, framebuffer_width, framebuffer_height - 500, 0x0000AA);
    int arr[10] = {0x00AA00, 0x00AAAA, 0xAA0000, 0xAA00AA, 0xAA5500, 0xAAAAAA, 0x555555, 0x5555FF, 0x55FF55, 0x55FFFF};
    int i;
    for (i = 0; i < 30; i++)
    	draw_square(30 + 7*i, 30 + 7*i, 200, 300, arr[i % 10]);

    char chr;
    for (chr = 32; chr <= '~'; chr++)
    {
    	draw_vga_character(chr, 500 + ((chr - 32) % 10)*20, 50 + ((chr - 32)/10)*20, 0x00AA00, 0x0000AA, 0);
    }*/


    //tty_printf("mboot_info = %x\n", mboot_info);

    //Detecting CPU
    //tty_printf("Detecting CPU...\n");
    //int tmp;
    //tmp = detect_cpu();

	//disable interrupts while kernel init
	//tty_printf("\nDisabling all interrupts...\n");
	interrupt_disable_all();

	//tty_printf("Installing Timer...\n");
	timer_install();

	//tty_printf("Installing keyboard...\n");
	keyboard_install();

	//enable interrutps
	//tty_printf("Enabling all interrupts...\n\n");
	interrupt_enable_all();

	return 1;
}

void higher_half_test()
{
	int eip, esp;
    //asm("movl %%ebx,%0" : "=r"(eip_val));
    tty_printf("Hello from higher half! ");
    asm volatile("1: lea 1b, %0;": "=a"(eip));
    tty_printf("EIP = %x  ", eip);
	asm("movl %%esp,%0" : "=r"(esp));
	tty_printf("ESP = %x  \n", esp);
}

void kernel_main(int magic_number, struct multiboot_info *mboot_info) //Arguments are passed by _start in boot.s
{
	//initilize the kernel
	multiboot_info mboot_info_copy = *mboot_info;
	kernel_init(&mboot_info_copy);

	// init the kernel debug shell (ksh)
	ksh_init();

	// run ksh main function
	ksh_main();

	for(;;) asm("hlt");
	//kernel_idle();
	
}

