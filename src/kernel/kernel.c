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
#include <kernel/devices/mouse.h>

//memory manager
#include <kernel/mm/phys_memory.h>
#include <kernel/mm/virt_memory.h>
#include <kernel/mm/kheap.h>

//file system
#include <kernel/fs/vfs.h>
#include <kernel/fs/initrd.h>

//graphics
#include <kernel/graphics/vesafb.h>



//struct multiboot_mod_list {
  /* the memory used goes from bytes 'mod_start' to 'mod_end-1' inclusive */
   // uint32_t type;
   // uint32_t size;
 // uint32_t mod_start;
 // uint32_t mod_end;

  /* Module command line */
 // uint32_t cmdline;

  /* padding to take it to 16 bytes (must be zero) */
  //uint32_t pad;
//};
//typedef struct multiboot_mod_list multiboot_module_t;

/*struct mboot_mod_list_struct
{
    uint32_t mod_start;
    uint32_t mod_end;
    uint32_t cmdline;
    uint32_t pad;
};*/



int kernel_init(struct multiboot_info *mboot_info)
{

    tty_init();
    svga_mode_info_t *svga_mode = (svga_mode_info_t*)mboot_info->vbe_mode_info;
    framebuffer_addr = svga_mode->physbase;// vmm_temp_map_page(svga_mode->physbase);
    framebuffer_pitch = svga_mode->pitch;
    framebuffer_bpp = svga_mode->bpp;
    framebuffer_width = svga_mode->screen_width;
    framebuffer_height = svga_mode->screen_height;
    //framebuffer_size = framebuffer_width*framebuffer_height*(framebuffer_bpp/8);
    framebuffer_size = framebuffer_height*framebuffer_pitch;
    back_framebuffer_addr = framebuffer_addr;

    //tty_printf("Hello ");

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

    //uint32_t initrd_beg = *(uint32_t*)(mboot_info->mods_addr);
    //uint32_t initrd_end = *(uint32_t*)(mboot_info->mods_addr + 4);

    //tty_printf("Some multiboot info:\nMagic number = %x\n", magic_number);
    //tty_printf("\nRAM available = %d MB\n", (mboot_info->mem_lower + mboot_info->mem_upper)/1024);
	//tty_printf("\n");
	
    //parse_memory_map((memory_map_entry*)mboot_info->mmap_addr, mboot_info->mmap_length);
    pmm_init(mboot_info);
    //pmm_test();

    uint32_t initrd_beg = *(uint32_t*)(mboot_info->mods_addr);
    uint32_t initrd_end = *(uint32_t*)(mboot_info->mods_addr + 4);

    //tty_printf("pde0 = %u\n", *(page_dir_entry*)(0xFFFFF000));
    //tty_printf("pde_%d = %u\n", (0xC0000000 >> 22), *(page_dir_entry*)(0xFFFFF000 + (0xC0000000 >> 22)*4));
    
    //uint32_t x = *(uint32_t*)(0xF0FFF000); //doenst cause page fault if in boot.s was identity mapped all the ram :D

    vmm_init();
    //vmm_test();

    kheap_init();
    //kheap_test();


    //tty_init();
    init_vbe(mboot_info);
    //uint32_t x = *(uint32_t*)(0xF0FFF000); //page fault example

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

    vfs_init();

    //struct mboot_mod_list_struct *mod = (struct mboot_mod_list_struct*)mboot_info->mods_addr;
    //tty_printf("mods_count = %x\n", mboot_info->mods_count);
    //tty_printf("mods_addr = %x\n", mboot_info->mods_addr);
    //tty_printf("cmdline = %s\n", (char*)(mod->cmdline));
    //tty_printf("mod_size = %x\n", *(uint32_t*)(mboot_info->mods_addr + 4));
    
    //if we read initrd_beg and end here oni 0 tk ih kto to zater!

    initrd_init(initrd_beg, initrd_end);
    //initrd_init(mod->mod_start, mod->mod_end);


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

    mouse_install();

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

