#
#    EOS - Experimental Operating System
#    low level functions for TSS
#
# 0x28 = 00101000 . bits 0 and 1 (two leftmost) are zeroes so selector is ring0.
# 00101000 = 40 in decimal - this is offset in bytes from GDT beginning.
# each gtd entry is 8 bit. 40/8 = 5. This means that 0x28 selector is ring0 and refers to gdt[5]

.global tss_flush     # Allows the C code to link to this. extern void tss_flush();
tss_flush:
    mov $0x2b, %ax  # or 0x28 ???
    ltr %ax
    ret