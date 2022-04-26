# Makefile for building EOS
KERNEL_FILE=bin/kernel/kernel.elf
ISODIR=isodir/
TARGET_ISO=EOS.iso
LOGFILE=EOS_qemu_serial.log

all: build-iso-run

run:
	qemu-system-i386 -kernel $(KERNEL_FILE) -M pc-i440fx-3.1

build-run: $(TARGET_ELF)
	qemu-system-i386 -kernel $(KERNEL_FILE) -M pc-i440fx-3.1

check-multiboot: $(TARGET_ELF)
	grub-file --is-x86-multiboot $(KERNEL_FILE)

compile-kernel:
	make -C kernel -f Makefile

build-iso: compile-kernel check-multiboot
	rm -rf $(ISODIR)
	mkdir -p $(ISODIR)boot
	make -C apps -f Makefile
	cp -r bin/apps initrd/
	cd initrd; tar -cf ../isodir/boot/initrd.tar *; cd -
	mkdir -p $(ISODIR)boot/grub
	cp $(KERNEL_FILE) $(ISODIR)boot/kernel.elf
	cp grub/grub.cfg $(ISODIR)boot/grub/grub.cfg
	grub-mkrescue -o $(TARGET_ISO) $(ISODIR)

clean-logs:
	rm -rf $(LOGFILE)

build-iso-run: clean-logs build-iso
	qemu-system-i386 -cdrom $(TARGET_ISO) -monitor stdio -serial file:$(LOGFILE)

run-iso: clean-logs
	qemu-system-i386 -cdrom $(TARGET_ISO) -monitor stdio -serial file:$(LOGFILE)

run-iso-debug: clean-logs
	qemu-system-i386 -s -S -cdrom $(TARGET_ISO) -monitor stdio -serial file:$(LOGFILE) & gdb $(KERNEL_FILE) -ex "target remote localhost:1234" -tui

build-iso-run-debug: build-iso run-iso-debug

clean:
	rm -rf $(ISODIR)
	rm -rf $(TARGET_ISO)
	rm -rf $(LOGFILE)
	make -C kernel -f Makefile clean
	make -C apps -f Makefile clean
	rm -rf initrd/apps/*



