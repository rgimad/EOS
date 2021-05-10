# EOS (Experimental Operating System)

EOS is an educational operating system for x86 platform. 
![screenshot](https://github.com/rgimad/EOS/blob/master/screenshots/screen1.png "Screenshot")

Current progress: (v.0.0.0.4.x)
----------------

- GDT and IDT setup
- VESA framebuffer based terminal :D
- PS/2 keyboard and mouse support
- Timer support
- RAM detection and Physical Memory Manager
- Higher Half Kernel

  The kernel starts at virtual address 0xC0100000

  0xC0000000-0xC0400000 and 0x00000000-0x00400000 vitual addresses are mapped to one physical range - to 0x00000000-0x00400000
- Very simple kernel command shell (KSH)
- Virtual Memory Manager
- Entry based kernel heap manager
- VFS
- InitRD
- ELF loader (in progress)
- System calls (in progress)
- Kernel mode thread seems to work properly
- ELF running (in progress)
- GUI (window system) (Console window system in progress)

TODO:
----

- Correct exit from kernel threads (situation not checked yet), usermode processes and threads, user stack, user heap etc..
- Mutexes and spinlocks for threads synchronization
- FAT32 driver
- HDD driver
- PCI driver
- ACPI driver
- etc.

Building, debugging etc.
------------------------

Example: to build release "make -f Makefile_linux build-iso"
to build and run release "make -f Makefile_linux build-iso-run"
to build debug "make -f Makefile_linux build-iso-run DEBUG=1"
to build and run debug "make -f Makefile_linux build-iso-run-debug DEBUG=1"
p.s always do "make -f Makefile_linux clean" before switching between debug and release configurations
p.s Makefile_windows is currently incomplete (TODO complete and fix it)

Thanks to:
---------

- Guilherme Samora (psamora)
- Nikita Ivanov, Dmitrii Kychanov, etc. from u365
- wiki.osdev.org
- Arjun Sreedharan
- Stephen Fewer (stephenfewer)
- James Molloy
- levex named user from gihtub
- reddit /r/osdev
- Stackoverflow
- etc.
