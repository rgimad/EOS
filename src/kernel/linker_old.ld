/*
   	This file says where each of the sections defined in the
   	object files should go in order to make up the resulting
   	binary
*/

/*
	The bootloader will look at this image and start execution
	at the symbol we designate as the entry point:
*/
ENTRY(_start)

/*
	Now we specify where the various sections of the object files
	will be put in the final kernel image.  This bit basically
	defines the image layout; each block in here defines an output
	section in terms of the input sections that should be combined
	to make it up.
*/
SECTIONS
{
	/* We start at 1MiB, which is a conventional place for kernels
	   to be loaded to by the bootloader.  "." means "the place that
	   you're currently writing to, and is a variable that can be
	   read from or written to -- eg. you could do ". = . + 1000 to
	   leave 1000 empty bytes. */
	. = 1M;

	/* Define a block called ".text".  The "BLOCK(4K)" means "move forward to the
	   next 4 KiB boundary; of course, as this is at the start of the file, it's
	   a no-op -- but the ones further down are meaningful. I don't know what the
	   ALIGN is for; the docs at https://www.math.utah.edu/docs/info/ld_3.html#IDX226
	   don't mention it... aha!  It looks like it's a backward-compatibility thing
	   that dupes the BLOCK.  See https://stackoverflow.com/questions/43686352/block-and-align-in-linker-script */
	.text BLOCK(4K) : ALIGN(4K)
	{
		/* Now we put in our multiboot header, which needs to be near the
		   start of the image.  The line below means 'include here all sections
		   called ".multiboot" from all input files' */
		*(.multiboot)

		/* Likewise, this means 'include here all sections called ".text" from all
		   input files' */
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

}