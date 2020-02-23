#ifndef _KERNEL_KERNEL_H_
#define _KERNEL_KERNEL_H_

//#include <sys/constants.h>
#include <kernel/multiboot.h>
#include <stdint.h>

#define EOS_VERSION_STRING "EOS v.0.0.0.4 (added hi-res vesa graphic terminal)"

//#define KERNEL_FILENAME				"kernel.elf"

//#define KERNEL_PID					0

//#define KERNEL_QUICKMAP_VADDRESS	(void *)0xC0000000

//#define KERNEL_CODE_VADDRESS		(void *)0xC0001000

//#define KERNEL_HEAP_VADDRESS		(void *)0xD0000000

//#define KERNEL_VGA_VADDRESS			(void *)0x000B8000

//#define KERNEL_VGA_PADDRESS			(void *)0x000B8000

int kernel_init(struct multiboot_info *mboot_info);
void higher_half_test();

void kernel_main(int magic_number, struct multiboot_info *mboot_info);

extern uint32_t kernel_stack_top_vaddr;

#endif

