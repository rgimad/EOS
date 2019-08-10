ENTRY(_start)


SECTIONS
{
	. = 1M;
	kernel_phys_start = .;

	.text BLOCK(4K) : ALIGN(4K)
	{
		*(.multiboot)
		*(.text)
	}

	.rodata BLOCK(4K) : ALIGN(4K)
	{
		/* Read-only data */
		*(.rodata)
	}

	.data BLOCK(4K) : ALIGN(4K)
	{
		/* Read-write data */
		*(.rodata)
	}

	.bss BLOCK(4K) : ALIGN(4K)
	{
		/* "COMMON" is a specifier for all "uninitialized data" from all input files.
		   Not clear on exactly what that means */
		*(COMMON)
		/* stack section -- see boot.s */
		*(.bss)
	}
	kernel_phys_end = .;

}