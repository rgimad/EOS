# EOS (Experimental Operating System)

EOS is an educational operating system for x86 platform. More information on official page https://vk.com/eos_developer_notes

Current progress:
----------------

- GDT and IDT setup
- Text terminal (80x25)
- PS/2 keyboard support
- Timer support
- RAM detection and Physical Memory Manager
- Higher Half Kernel
- Virtual Memory Manager (now testing)

  The kernel starts at virtual address 0xC0100000

  0xC0000000-0xC0400000 and 0x00000000-0x00400000 vitual addresses are mapped to one physical range - to 0x00000000-0x00400000
- Very simple kernel command shell (KSH)

TODO:
----

- Heap manager
- InitRD
- File system
- Terminal with better fonts and higher resolution :D
- etc.

Thanks to:
---------

- Guilherme Samora (psamora)
- Nikita Ivanov, Dmitrii Kychanov, etc. from u365
- wiki.osdev.org
- Arjun Sreedharan
- Stephen Fewer (stephenfewer)
- etc.