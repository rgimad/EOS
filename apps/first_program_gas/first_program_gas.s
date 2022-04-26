# Test program for eos

.text

    .global _main

_main:
    pushl %ebp
    movl %esp, %ebp

    movl $syscall_args, %ebx
    movl $hello, (%ebx)
    movl $0, %eax

    int $0x80
    int $0x80
    int $0x80
    int $0x80
    int $0x80
    int $0x80
    int $0x80

    movl %ebp, %esp
    popl %ebp
    ret


.data

hello:
    .ascii "This is a sample program for EOS\nthis program uses syscalls(0x80)!!!\n\n"
syscall_args:
    .long 0x00000000
