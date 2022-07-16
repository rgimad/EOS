# EOS (Experimental Operating System)

**EOS** is an educational operating system for x86 platform

![Screenshot](https://raw.githubusercontent.com/rgimad/EOS/master/screenshots/screenshot1.png "Screenshot")

## Current status

- GDT and IDT setup
- VESA framebuffer based terminal :D
- PS/2 keyboard and mouse support
- RAM detection and Physical Memory Manager
- Higher Half Kernel

  The kernel starts at virtual address 0xC0100000

  0xC0000000-0xC0400000 and 0x00000000-0x00400000 vitual addresses are mapped to one physical range - to 0x00000000-0x00400000
- Virtual Memory Manager
- Entry based kernel heap manager (kmalloc/kfree/krealloc)
- Very simple kernel command shell (KSH)
- VFS
- Initrd, uses TarFS
- MENUET01 and PE executables loaders
- System call mechanism

## TODO
- User-mode and multitasking
- Synchronization (mutex, spinlock)
- FAT32 driver
- HDD driver
- PCI driver
- ACPI driver
- etc.

## Building and debugging
Go to src/kernel

- Building *release* version:

```Shell
make build-iso
```

- Building and running *release* version:

```Shell
make build-iso-run
```

- Building *debug* version:

```Shell
make build-iso DEBUG=1
```

- Building and running *debug* version:

```Shell
make build-iso-run DEBUG=1
```

- Always do following command before switching between debug and release configurations:

```Shell
make clean
```

## Thanks to

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
