# EOS (Experimental Operating System)

EOS is an educational operating system for x86 platform. More information on official page https://vk.com/eos_developer_notes

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
- ELF running (in progress)
- GUI (window system) (Console window system in progress)

TODO:
----

- Multitasking (processes and threads, user stack, user heap, user-mode)
- FAT32 driver
- HDD driver
- PCI driver
- ACPI driver
- etc.

Thanks to:
---------

- Guilherme Samora (psamora)
- Nikita Ivanov, Dmitrii Kychanov, etc. from u365
- wiki.osdev.org
- Arjun Sreedharan
- Stephen Fewer (stephenfewer)
- etc.