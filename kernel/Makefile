# Makefile for building EOS kernel

TARGET_ELF=kernel.elf
TARGET_DIR=../bin/kernel/

INCLUDE_DIR=../

CC = clang
LD = ld.lld

TARGET_ARCH = -target i386-pc-none-elf
CCFLAGS= $(TARGET_ARCH) -Wall -Wextra -mno-sse -mno-avx -std=gnu99 -ffreestanding -I $(INCLUDE_DIR)
LDFLAGS=-T linker.ld -nostdlib

# Enable debugging symbols (optional)
ifeq ($(DEBUG), 1)
	CCFLAGS += -g
endif

OBJS_PATH=../bin/kernel
OBJS= $(patsubst %, $(OBJS_PATH)/%.o, \
	boot kernel libk/string libk/math libk/assert libk/sprintf libk/ctype \
	gdt gdt_asm idt idt_asm tss tss_asm tty cpu_detect \
	interrupts interrupts_asm paging syscall ksh \
	graphics/vesafb graphics/vgafnt \
	io/ports io/qemu_log \
	devices/keyboard devices/timer devices/mouse \
	mm/phys_memory mm/virt_memory mm/kheap \
	fs/vfs fs/initrd \
	pm/pe pm/kex)

all: $(TARGET_ELF)

$(TARGET_DIR):
	mkdir -p $(TARGET_DIR)

$(TARGET_ELF): $(TARGET_DIR) $(OBJS)
	$(LD) $(LDFLAGS) -o $(TARGET_DIR)$(TARGET_ELF) $(OBJS)

$(OBJS_PATH)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CCFLAGS) -c -o $@ $<

$(OBJS_PATH)/%.o: %.s
	@mkdir -p $(dir $@)
	$(CC) $(TARGET_ARCH) -c -o $@ $<

clean:
	rm -f $(OBJS)
	rm -rf $(TARGET_DIR)

.PHONY: all clean
