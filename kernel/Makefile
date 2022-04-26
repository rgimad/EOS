# Makefile for building EOS kernel
TARGET_ELF=kernel.elf
TARGET_DIR=../bin/kernel/

INCLUDE_DIR=../

AS = i686-elf-as
CC = i686-elf-gcc

# if we're on Windows use WSL to run bash commands
ifeq ($(OS),Windows_NT) 
    BASH = wsl
else
    BASH =
endif

LDFLAGS=-T linker.ld -ffreestanding -nostdlib -lgcc
ifeq ($(DEBUG), 1)
	CCFLAGS=-std=gnu99 -g -ffreestanding -Wall -Wextra -I $(INCLUDE_DIR)
else
	CCFLAGS=-std=gnu99 -ffreestanding -Wall -Wextra -I $(INCLUDE_DIR)
endif

OBJ=boot.o kernel.o \
	libk/string.o libk/math.o libk/list.o \
	gdt.o gdt_asm.o idt.o idt_asm.o tss.o tss_asm.o tty.o cpu_detect.o \
	interrupts.o interrupts_asm.o paging.o syscall.o ksh.o \
	graphics/vesafb.o graphics/vgafnt.o \
	io/ports.o io/qemu_log.o \
	devices/keyboard.o devices/timer.o devices/mouse.o \
	mm/phys_memory.o mm/virt_memory.o mm/kheap.o mm/uheap.o \
	fs/vfs.o fs/initrd.o \
	pm/elf.o pm/thread.o pm/process.o pm/scheduler.o pm/context_switch_asm.o \
	gui/consolewindow.o

all: $(TARGET_ELF)

$(TARGET_ELF): $(OBJ)
	$(BASH) mkdir -p $(TARGET_DIR)
	$(CC) $(LDFLAGS) -o $(TARGET_DIR)$(TARGET_ELF) $(OBJ)

%.o: %.c
	$(CC) $(CCFLAGS) -c -o $@ $<

%.o: %.s
	$(AS) -o $@ $<

clean:
	$(BASH) rm -rf *.o
	$(BASH) rm -rf io/*.o
	$(BASH) rm -rf devices/*.o
	$(BASH) rm -rf mm/*.o
	$(BASH) rm -rf graphics/*.o
	$(BASH) rm -rf fs/*.o
	$(BASH) rm -rf pm/*.o
	$(BASH) rm -rf ../libk/*.o
	$(BASH) rm -rf gui/*.o
	$(BASH) rm -rf $(TARGET_DIR)kernel.elf

