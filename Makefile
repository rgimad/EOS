# Makefile for building EOS
KERNEL_FILE=bin/kernel/kernel.elf
ISODIR=isodir/
TARGET_ISO=EOS.iso
LOGFILE=EOS_qemu_serial.log

# if we're on Windows use WSL to run bash commands
ifeq ($(OS),Windows_NT) 
    BASH = wsl
else
    BASH =
endif

all: build-iso-run

check-multiboot: $(KERNEL_FILE)
	$(BASH) grub-file --is-x86-multiboot $(KERNEL_FILE)

compile-kernel:
	make -C kernel -f Makefile DEBUG=$(DEBUG)

build-iso: compile-kernel check-multiboot
	$(BASH) rm -rf $(ISODIR)
	$(BASH) mkdir -p $(ISODIR)boot
	make -C apps -f Makefile
	$(BASH) cp -r bin/apps initrd/
	$(BASH) cd initrd; tar -cf ../isodir/boot/initrd.tar *; cd -
	$(BASH) mkdir -p $(ISODIR)boot/grub
	$(BASH) cp $(KERNEL_FILE) $(ISODIR)boot/kernel.elf
	$(BASH) cp grub/grub.cfg $(ISODIR)boot/grub/grub.cfg
	$(BASH) grub-mkrescue -o $(TARGET_ISO) $(ISODIR)

clean-logs:
	$(BASH) rm -rf $(LOGFILE)

run-iso: clean-logs
ifeq ($(DEBUG),1)
	qemu-system-i386 -s -S -m 512 -cdrom $(TARGET_ISO) -monitor stdio -serial file:$(LOGFILE) & gdb $(KERNEL_FILE) -ex "target remote localhost:1234" -tui
else
	qemu-system-i386 -m 2024 -cdrom $(TARGET_ISO) -monitor stdio -serial file:$(LOGFILE)
endif

build-iso-run: build-iso run-iso

clean:
	$(BASH) rm -rf $(ISODIR)
	$(BASH) rm -rf $(TARGET_ISO)
	$(BASH) rm -rf $(LOGFILE)
	make -C kernel -f Makefile clean
	make -C apps -f Makefile clean
	$(BASH) rm -rf initrd/apps/*



