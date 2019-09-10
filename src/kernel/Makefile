MAKEFILE=Makefile

TARGET_ELF=kernel.elf

TARGET_DIR=../../bin/kernel/

ISODIR=../../isodir/

INCLUDE_DIR=../../include/

AS = "C:\i686-elf-tools\bin\i686-elf-as.exe"
CC = "C:\i686-elf-tools\bin\i686-elf-gcc.exe"
BASH = ubuntu1804 run

#LDFLAGS=-T linker.ld -ffreestanding -O2 -nostdlib -lgcc
#CCFLAGS=-std=gnu99 -ffreestanding -O2 -Wall -Wextra -I $(INCLUDE_DIR)
LDFLAGS=-T linker.ld -ffreestanding -nostdlib -lgcc
CCFLAGS=-std=gnu99 -ffreestanding -Wall -Wextra -I $(INCLUDE_DIR)

#LDFLAGS=-T linker.ld -ffreestanding -O1 -nostdlib -lgcc
#CCFLAGS=-std=gnu99 -ffreestanding -O1 -Wall -Wextra -I $(INCLUDE_DIR)

OBJ=boot.o kernel.o ../libk/string.o ../libk/math.o ksh.o gdt.o gdt_asm.o idt.o idt_asm.o tty.o graphics/vesafb.o graphics/vgafnt.o cpu_detect.o io/ports.o interrupts.o interrupts_asm.o devices/keyboard.o devices/timer.o devices/mouse.o mm/phys_memory.o paging.o mm/virt_memory.o mm/kheap.o fs/vfs.o fs/initrd.o
	
all: $(TARGET_ELF)

$(TARGET_ELF): $(OBJ)
	$(CC) $(LDFLAGS) -o $(TARGET_DIR)$(TARGET_ELF) $(OBJ)
	
%.o: %.c
	$(CC) $(CCFLAGS) -c -o $@ $<
	
#old was .asm
%.o: %.s
	$(AS) -o $@ $<

run:
	qemu-system-i386.exe -kernel $(TARGET_DIR)kernel.elf -M pc-i440fx-3.1

build-run: $(TARGET_ELF)
	qemu-system-i386.exe -kernel $(TARGET_DIR)/kernel.elf -M pc-i440fx-3.1

check-multiboot: $(TARGET_ELF)
	${BASH} grub-file --is-x86-multiboot $(TARGET_DIR)$(TARGET_ELF)

build-iso: check-multiboot
	${BASH} rm -rf $(ISODIR)
	${BASH} mkdir -p $(ISODIR)boot
	${BASH} "cd ../../initrd; tar -cf ../isodir/boot/initrd.tar *; cd -"
	${BASH} mkdir -p $(ISODIR)boot/grub
	${BASH} cp $(TARGET_DIR)kernel.elf $(ISODIR)boot/kernel.elf
	${BASH} cp ../../grub/grub.cfg $(ISODIR)boot/grub/grub.cfg
	${BASH} grub-mkrescue -o ../../EOS.iso $(ISODIR)

build-iso-run: build-iso
	qemu-system-i386.exe -cdrom ../../EOS.iso

run-iso:
	qemu-system-i386.exe -cdrom ../../EOS.iso

run-iso-monitor:
	qemu-system-i386.exe -cdrom ../../EOS.iso -monitor stdio

clean:
	${BASH} rm -rf $(ISODIR)
	${BASH} rm -rf *.o
	${BASH} rm -rf io/*.o
	${BASH} rm -rf devices/*.o
	${BASH} rm -rf mm/*.o
	${BASH} rm -rf graphics/*.o
	${BASH} rm -rf fs/*.o