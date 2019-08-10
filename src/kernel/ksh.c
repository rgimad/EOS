/*
*    EOS - Experimental Operating System
*    Kernel mode shell
*/
#include <kernel/ksh.h>
#include <kernel/tty.h>
#include <kernel/vga.h>
#include <kernel/devices/keyboard.h>
#include <kernel/cpu-detect.h>
#include <kernel/devices/timer.h>

#include <kernel/kernel.h>

#include <libk/string.h>


void ksh_init()
{
	tty_putstring_color("                  EOS KSH (Kernel SHell):\n\n", VGA_COLOR_LIGHT_RED);
}

void ksh_main()
{
	char cmd[256];
	int i;

	while(1)
	{
		tty_putstring_color("kernel> ", VGA_COLOR_LIGHT_BLUE);

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
		} else if (strcmp(cmd, "regdump") == 0)
		{
			//ksh_cmd_regdump();
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

void ksh_cmd_regdump()
{
	//uint32_t eax, ebx, ecx, edx, esi, edi, esp, ebp, cr0, cr2, cr3;
	
}

void ksh_cmd_help()
{
	tty_printf("Available commands:\n cpuid - information about processor\n ticks - get number of ticks\n about\n help\n");
}