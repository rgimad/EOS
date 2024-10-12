# Makefile for building EOS kernel
TARGET_ELF=kernel.elf
TARGET_DIR=../bin/kernel/

INCLUDE_DIR=../

CC = clang
LD = ld.lld

TARGET_ARCH = -target i386-pc-none-elf
CCFLAGS= $(TARGET_ARCH) -Wall -Wextra -mno-sse -mno-avx -std=gnu99 -ffreestanding -I $(INCLUDE_DIR)
LDFLAGS=-T linker.ld -nostdlib

ifeq ($(DEBUG), 1)
	CCFLAGS += -g
endif

OBJ=	boot.o kernel.o \
	libk/string.o libk/math.o libk/assert.o libk/sprintf.o libk/ctype.o\
	gdt.o gdt_asm.o idt.o idt_asm.o tss.o tss_asm.o tty.o cpu_detect.o \
	interrupts.o interrupts_asm.o paging.o syscall.o ksh.o \
	graphics/vesafb.o graphics/vgafnt.o \
	io/ports.o io/qemu_log.o \
	devices/keyboard.o devices/timer.o devices/mouse.o \
	mm/phys_memory.o mm/virt_memory.o mm/kheap.o \
	fs/vfs.o fs/initrd.o \
	pm/pe.o pm/kex.o

all: $(TARGET_ELF)

$(TARGET_ELF): $(OBJ)
	mkdir -p $(TARGET_DIR)
	$(LD) $(LDFLAGS) -o $(TARGET_DIR)$(TARGET_ELF) $(OBJ)

%.o: %.c
	$(CC) $(CCFLAGS) -c -o $@ $<

%.o: %.s
	$(CC) $(TARGET_ARCH) -c -o $@ $<

clean:
	rm -rf *.o
	rm -rf io/*.o
	rm -rf devices/*.o
	rm -rf mm/*.o
	rm -rf graphics/*.o
	rm -rf fs/*.o
	rm -rf pm/*.o
	rm -rf libk/*.o
	rm -rf $(TARGET_DIR)kernel.elf
