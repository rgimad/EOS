# Makefile for building EOS kernel
TARGET_ELF=kernel.elf
TARGET_DIR=../bin/kernel/

INCLUDE_DIR=../

CC = clang
LD = ld.lld

# if we're on Windows use WSL to run bash commands
ifeq ($(OS),Windows_NT) 
    BASH = wsl
else
    BASH =
endif

TARGET_ARCH = -target i386-pc-none-elf
CCFLAGS= $(TARGET_ARCH) -Wall -Wextra -mno-sse -mno-avx -std=gnu99 -ffreestanding -I $(INCLUDE_DIR)
LDFLAGS=-T linker.ld -nostdlib

ifeq ($(DEBUG), 1)
	CCFLAGS += -g
endif

OBJ=	boot.o kernel.o \
	libk/string.o libk/math.o libk/assert.o\
	gdt.o gdt_asm.o idt.o idt_asm.o tss.o tss_asm.o tty.o cpu_detect.o \
	interrupts.o interrupts_asm.o paging.o syscall.o ksh.o \
	graphics/vesafb.o graphics/vgafnt.o \
	io/ports.o io/qemu_log.o \
	devices/keyboard.o devices/timer.o devices/mouse.o \
	mm/phys_memory.o mm/virt_memory.o mm/kheap.o \
	fs/vfs.o fs/initrd.o \
	pm/pe.o pm/kex.o \
	gui/consolewindow.o

all: $(TARGET_ELF)

$(TARGET_ELF): $(OBJ)
	$(BASH) mkdir -p $(TARGET_DIR)
	$(LD) $(LDFLAGS) -o $(TARGET_DIR)$(TARGET_ELF) $(OBJ)

%.o: %.c
	$(CC) $(CCFLAGS) -c -o $@ $<

%.o: %.s
	$(CC) $(TARGET_ARCH) -c -o $@ $<

clean:
	$(BASH) rm -rf *.o
	$(BASH) rm -rf io/*.o
	$(BASH) rm -rf devices/*.o
	$(BASH) rm -rf mm/*.o
	$(BASH) rm -rf graphics/*.o
	$(BASH) rm -rf fs/*.o
	$(BASH) rm -rf pm/*.o
	$(BASH) rm -rf libk/*.o
	$(BASH) rm -rf gui/*.o
	$(BASH) rm -rf $(TARGET_DIR)kernel.elf
