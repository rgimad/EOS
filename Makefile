# Makefile for building EOS
KERNEL_FILE=bin/kernel/kernel.elf
ISODIR=isodir/
TARGET_ISO=EOS.iso
LOGFILE=EOS_qemu_serial.log

all: build-iso-run

check-multiboot: $(KERNEL_FILE)
	grub-file --is-x86-multiboot $(KERNEL_FILE)

compile-kernel:
	make -C kernel -f Makefile DEBUG=$(DEBUG)

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

run-iso: clean-logs
ifeq ($(DEBUG),1)
	qemu-system-i386 -daemonize -s -S -m 512 -cdrom $(TARGET_ISO) -serial file:$(LOGFILE) & gdb -nx -ix ./qemu.gdbinit
else
	qemu-system-i386 -m 2024 -cdrom $(TARGET_ISO) -monitor stdio -serial file:$(LOGFILE)
endif

build-iso-run: build-iso run-iso

clean:
	rm -rf $(ISODIR)
	rm -rf $(TARGET_ISO)
	rm -rf $(LOGFILE)
	make -C kernel -f Makefile clean
	make -C apps -f Makefile clean
	rm -rf initrd/apps/*



