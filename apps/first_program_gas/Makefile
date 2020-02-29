#MAKEFILE=Makefile

#INCLUDE_DIR=../include/libc/

TARGET_ELF = first_program_gas.elf
TARGET_DIR = ../../bin/apps/

AS = "/opt/i686-elf-tools-linux/bin/i686-elf-as"
CC = "/opt/i686-elf-tools-linux/bin/i686-elf-gcc"
LD = "/opt/i686-elf-tools-linux/bin/i686-elf-ld"

LDFLAGS=-T ../examples/app_gas.ld -ffreestanding -nostdlib -lgcc

OBJ = first_program_gas.o
	
all: $(TARGET_ELF)

$(TARGET_ELF): $(OBJ)
	$(CC) $(LDFLAGS) -o $(TARGET_DIR)$(TARGET_ELF) $(OBJ)
	
%.o: %.s
	$(AS) -o $@ $<

clean:
	rm -rf *.o

