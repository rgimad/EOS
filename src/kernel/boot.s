# EOS - Experimental Operating System
# Kernel startup asm code

# Declare constants for the multiboot header
.set ALIGN,		1<<0				# align loaded modules on page boundaries
.set MEMINFO,	1<<1				# please provide us a memory map
.set FLAGS,		ALIGN | MEMINFO		# multiboot "flag" field
.set MAGIC,		0x1BADB002			# magic number to let the booloader find the header
.set CHECKSUM,	-(MAGIC + FLAGS)	# Checksum of the above

# Declare a multiboot header that marks the program as a kernel.  These
# are magic values that are documented in the multiboot standard.  The
# bootloader will search for this signature in the first 8KiB of the 
# kernel file, aligned at a 32-bit boundary.  The signature is in its
# own section so the header can be forced to be in the first 8 KiB of 
# the kernel file.
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

# The multiboot standard does not define the value of the stack pointer
# register esp, and it's up to the kernel to provide a stack.  The following
# lines allocate room for a small stack by creating a symbol at the bottom,
# then allocating 16KiB for it, then putting a symbol at the top.  The
# stack will grow downwards.  It's apparently in its own section so it
# can be marked nobits, but there doesn't seem to actually be anything
# that does that.  Odd.  In theory doing that allows us to avoid having
# 16KiB of dead bytes in the binary.
# The stack needs to be 16-byte alsigned for x86 according to the System
# V ABI standard, so we do that here.
.section .bss
.align 16
stack_bottom:
.skip 65535 # 64 KiB for the stack             old: 16384  # 16KiB
stack_top:

# The linker script will specify _start as the entry point to the kernel
# and so that bootloader will jump to this position when the kernel has
# been loaded.  We won't return from the function, as there's no point
# in returning from a kernel because there's nothing to return to.
.section .text
.global _start
.type _start, @function
_start:
	# set up the stack
	mov $stack_top, %esp

	# This is where we would set up the GDT if we were setting one
	# up -- but there's nothing here for now.

	push %ebx # Multiboot structure
	push %eax # Multiboot magic number

	# Now, call our C code.  The stack is untouched and thus 16-byte 
	# aligned, just like it was originally.  So we can safely call a 
	# C function that is expecting the System V ABI.

	call kernel_main

	# As we have nothing left to do, we can go into an infinite loop.
	# We disable interrupts (probably unnecessary, but if the kernel
	# was buggy and enabled them then returned, we it's safest to fix 
	# that).
	cli

	# Then we halt and wait for the next interrupt.  Having disabled
	# them, we should never get them -- though there might be a non-
	# maskable one
1:	hlt
	
	# Just in case we get a non-maskable one...
	jmp 1b

# We set the size of the _start symbol -- which presumably means it's
# some kind of object with attributes, not just a simple label -- 
# to the difference between our current point and the symbol's location
.size _start, . - _start
