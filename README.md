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
- VFS (in progress)
- InitRD (in progress)


TODO:
----

- ELF loader
- Multitasking
- GUI
- FAT32
- HDD reading
- PCI
- etc.

Thanks to:
---------

- Guilherme Samora (psamora)
- Nikita Ivanov, Dmitrii Kychanov, etc. from u365
- wiki.osdev.org
- Arjun Sreedharan
- Stephen Fewer (stephenfewer)
- etc.