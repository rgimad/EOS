.global enable_paging

enable_paging:
    push %eax # save eax value before use

    # Move directory into CR3
    mov 8(%esp), %eax
    mov %eax, %cr3

    # Disable 4MB pages
    mov %cr4, %eax
    and $~0x00000010, %eax
    mov %eax, %cr4

    # Enable paging
    mov %cr0, %eax
    or $0x80000000, %eax
    mov %eax, %cr0

    pop %eax # restore eax value
    ret
