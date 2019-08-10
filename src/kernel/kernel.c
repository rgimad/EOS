/*
*    EOS - Experimental Operating System
*    Kernel main source file
*/

#include <kernel/kernel.h>
#include <kernel/multiboot.h>

//then include kernel subsystem's headers
#include <kernel/vga.h>
#include <kernel/tty.h>
#include <kernel/ksh.h>
#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/interrupts.h>
#include <kernel/cpu-detect.h>

//devices
#include <kernel/devices/timer.h>
#include <kernel/devices/keyboard.h>

//memory manager
#include <kernel/mm/phys_memory.h>



int kernel_init(struct multiboot_info *mboot_info)
{
	//init terminal subsystem
	tty_init();

	//tty_printf("                  ");tty_printf(EOS_VERSION_STRING);tty_printf("\n\n");
	tty_printf("\n                  ");tty_putstring_color(EOS_VERSION_STRING, VGA_COLOR_GREEN);tty_printf("\n\n");

	tty_printf("Terminal is ready.\n");

	//install Global Descriptor Table
	tty_printf("Installing GDT...\n");
    gdt_install();

    //install Interrupt Descriptor Table
    tty_printf("Installing IDT...\n");
    idt_install();

    //tty_printf("Some multiboot info:\nMagic number = %x\n", magic_number);
    //tty_printf("\nRAM available = %d MB\n", (mboot_info->mem_lower + mboot_info->mem_upper)/1024);
	tty_printf("\n");
	
    //parse_memory_map((memory_map_entry*)mboot_info->mmap_addr, mboot_info->mmap_length);
    phys_memory_init(mboot_info);

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

void kernel_main(int magic_number, struct multiboot_info *mboot_info) //Arguments are passed by _start in boot.s
{
	//initilize the kernel
	kernel_init(mboot_info);

	// init the kernel debug shell (ksh)
	ksh_init();

	// run ksh main function
	ksh_main();

	for(;;) asm("hlt");
	//kernel_idle();
	
}

