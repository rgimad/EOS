/*
*    EOS - Experimental Operating System
*    Kernel mode shell
*/
#include <kernel/ksh.h>
#include <kernel/tty.h>
#include <kernel/vga.h>
#include <kernel/graphics/vesafb.h>
#include <kernel/devices/keyboard.h>
#include <kernel/cpu_detect.h>
#include <kernel/devices/timer.h>

#include <kernel/mm/kheap.h>

#include <kernel/fs/vfs.h>
#include <kernel/fs/initrd.h>

#include <kernel/kernel.h>

#include <libk/string.h>


void ksh_init()
{
	tty_putstring_color("                  EOS KSH (Kernel SHell):\n\n", VESA_LIGHT_RED);
}

void ksh_main()
{
	char cmd[256];
	int i;

	while(1)
	{
		tty_putstring_color("kernel> ", VESA_LIGHT_BLUE);

		//tty_setcolor(vga_entry_color(VGA_COLOR_BLUE, VGA_COLOR_BLACK));
		//tty_printf("kernel> ");
		//tty_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));

		keyboard_gets(cmd, 256);
		//tty_printf("(%s)\n", cmd);//
		if (strlen(cmd) > 0) if (strcmp(cmd, "cpuid") == 0)
		{
			ksh_cmd_cpuid();
		} else if (strcmp(cmd, "about") == 0)
		{
			ksh_cmd_about();
		} else if (strcmp(cmd, "help") == 0)
		{
			ksh_cmd_help();
		} else if (strcmp(cmd, "ticks") == 0)
		{
			ksh_cmd_ticks();
		} else if (strcmp(cmd, "kheap_test") == 0)
		{
			ksh_kheap_test();
		} else if (strcmp(cmd, "draw_demo") == 0)
		{
			ksh_draw_demo();
		} else if (strcmp(cmd, "ls") == 0)
		{
			//initrd test
			//tty_printf("x = %x%x%x%x", );
			initrd_list(0, 0);
			char *arr = (char*)kheap_malloc(1000);
			initrd_read("about_eos.txt", 0, initrd_get_filesize("about_eos.txt"), 0, arr);
			tty_printf("\n%s\n", arr);
		} else {//if...
			ksh_cmd_unknown();
		}
	}
}

//command handlers implementation

void ksh_cmd_cpuid()
{
	detect_cpu();
	tty_printf("\n");
}

void ksh_cmd_unknown()
{
	tty_printf("Error: unknown command.\n");
}

void ksh_cmd_about()
{
	tty_printf("%s   Rustem Gimadutdinov\n", EOS_VERSION_STRING);
}

void ksh_cmd_ticks()
{
	tty_printf("Timer ticks = %d\n", timer_get_ticks());
}

void ksh_kheap_test()
{
	kheap_test();
}

void ksh_draw_demo()
{
	draw_fill(0, 500, framebuffer_width, framebuffer_height - 500, 0x0000AA);
    int arr[10] = {0x00AA00, 0x00AAAA, 0xAA0000, 0xAA00AA, 0xAA5500, 0xAAAAAA, 0x555555, 0x5555FF, 0x55FF55, 0x55FFFF};
    int i;
    for (i = 0; i < 30; i++)
    	draw_square(30 + 7*i, 30 + 7*i, 200, 300, arr[i % 10]);

    char chr;
    for (chr = 32; chr <= '~'; chr++)
    {
    	draw_vga_character(chr, 500 + ((chr - 32) % 10)*20, 50 + ((chr - 32)/10)*20, 0x00AA00, 0x0000AA, 0);
    }
}

void ksh_cmd_regdump()
{
	//uint32_t eax, ebx, ecx, edx, esi, edi, esp, ebp, cr0, cr2, cr3;
	
}

void ksh_cmd_help()
{
	tty_printf("Available commands:\n cpuid - information about processor\n ticks - get number of ticks\n kheap_test - test kernel heap\n draw_demo - demo super effects\n about\n help\n");
}