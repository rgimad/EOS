# EOS - Experimental Operating System
# Kernel startup asm code

# Declare constants for the multiboot header
.set ALIGN,		1<<0				# align loaded modules on page boundaries
.set MEMINFO,	1<<1				# please provide us a memory map
.set VBE_MODE, 1<<2       # VBE mode flag. GRUB will set it for us and provide info about it.
.set FLAGS,		ALIGN | MEMINFO	| VBE_MODE	# multiboot "flag" field
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
.long CHECKSUM #if you dont need vbe comment longs after this line
.long 0, 0, 0, 0, 0 # unused?
.long 0 # 0 = set graphics mode
.long 1024, 768, 32 # Width, height, depth


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

# Reserve a stack for the initial thread.
.section .bootstrap_stack, "aw", @nobits
stack_bottom:
.skip 65536 # 64 KiB for the stack             old: 16384  # 16KiB
stack_top:

# This is the virtual base address of kernel space. It must be used to convert virtual
# addresses into physical addresses until paging is enabled. Note that this is not
# the virtual address where the kernel image itself is loaded -- just the amount that must
# be subtracted from a virtual address to get a physical address.
.set KERNEL_VIRTUAL_BASE, 0xC0000000                  # 3GB
.set KERNEL_PAGE_NUMBER, (KERNEL_VIRTUAL_BASE >> 22)  # Page directory index of kernel's 4MB PTE.

# Declares the boot Paging directory to load a virtual higher half kernel
.section .data
.align 0x1000
.global _boot_page_directory
_boot_page_directory:
    .long 0x00000083
    .fill (KERNEL_PAGE_NUMBER - 1), 4, 0x00000000
    .long 0x00000083
    .fill (1024 - KERNEL_PAGE_NUMBER - 1), 4, 0x00000000
    #.fill (1024 - KERNEL_PAGE_NUMBER - 2), 4, 0x00000000
    #.long (_boot_page_directory | 0x00000003) #store the page dir as the last entry in itself (fractal mapping)

#Why 0x00000083
# the first entry identity maps the first 4MB of memory
# All bits are clear except the following:
# bit 7: PS The kernel page is 4MB.
# bit 1: RW The kernel page is read/write.
# bit 0: P  The kernel page is present.
# 0x00000083 in binary this is 10000011


#Text section
.section .text
.global _loader
_loader:
  mov $(_boot_page_directory - KERNEL_VIRTUAL_BASE), %ecx
  mov $(_boot_page_directory - KERNEL_VIRTUAL_BASE), %edx
  or $0x00000003, %ecx
  mov %ecx, 0xFFC(%edx) #bpd + 4092 i.e we write address of page_dir|0000003 to last pde

  # Load Page Directory Base Register. Until paging is set up, the code must
  # be position-independent and use physical addresses, not virtual ones
  mov $(_boot_page_directory - KERNEL_VIRTUAL_BASE), %ecx
  mov %ecx, %cr3

  # Set PSE bit in CR4 to enable 4MB pages.
  mov %cr4, %ecx
  or $0x00000010, %ecx
  mov %ecx, %cr4

  # Set PG bit in CR0 to enable paging.
  mov %cr0, %ecx
  or $0x80000000, %ecx
  mov %ecx, %cr0

  # Start fetching instructions in kernel space.
  # Since eip at this point holds the physical address of this command
  # (approximately 0x00100000) we need to do a long jump to the correct
  # virtual address of _start which is approximately 0xC0100000.
  movl $_start, %edx
  jmp *%edx

# The Higher-Half Kernel entry point.
.global _start
.type _start, @function
_start:
	# Unmap the identity-mapped first 4MB of physical address space.
  	# movl $0, (_boot_page_directory) #uncommenting this crashes all, figure why???
  	# invlpg (0)

  	# Enter protection mode
  	mov %cr0, %eax
  	or $1, %al
  	mov %eax, %cr0

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
