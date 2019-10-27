/*ENTRY(main)
phys = 0x00000000;
virt = 0x10000000;
SECTIONS
{
  .text virt : AT( phys )
  {
    *(.text)
    . = ALIGN(4);
    *(.data)
    . = ALIGN(4);
    *(.rodata)
    . = ALIGN(4);
    *(COMMON)
    . = ALIGN(4);
    *(.bss)
    . = ALIGN(4);
  }
}


ENTRY(main)
phys = 0x00000000;
virt = 0x10000000;

SECTIONS {
  .text virt : AT( phys )
  {
    *(.text)
    . = ALIGN(4);
    *(.rodata)
    . = ALIGN(4);
    *(.data)
    . = ALIGN(4);
    *(COMMON)
    . = ALIGN(4);
    *(.bss)
    . = ALIGN(4);
  }
}*/

ENTRY(main)


SECTIONS {
  . = 0x10000000;

  .text : AT(ADDR(.text) - 0x00000000) {
    *(.multiboot)
    *(.text)
    . = ALIGN(0x1000);
  }

  .rodata : AT(ADDR(.rodata) - 0x00000000) {
    *(.rodata*)
    . = ALIGN(0x1000);
  }

  .data : AT(ADDR(.data) - 0x00000000) {
    *(.data)
    . = ALIGN(0x1000);
  }

  .bss : AT(ADDR(.bss) - 0x00000000) {
    *(COMMON)
    *(.bss)
    *(.bootstrap_stack)
    . = ALIGN(0x1000);
  }
}
