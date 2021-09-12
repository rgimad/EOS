TARGET_ELF = reverse_string.elf
TARGET_DIR = ../../bin/apps/

AS = "/opt/i686-elf-tools-linux/bin/i686-elf-as"
CC = "/opt/i686-elf-tools-linux/bin/i686-elf-gcc"
LD = "/opt/i686-elf-tools-linux/bin/i686-elf-ld"

LDFLAGS=-T ../examples/app_c.ld -ffreestanding -nostdlib -lgcc
CCFLAGS=-std=gnu99 -ffreestanding -nostdlib -lgcc -Wall -Wextra

OBJ = reverse_string.o

all: $(TARGET_ELF)

$(TARGET_ELF): $(OBJ)
	$(CC) $(LDFLAGS) -o $(TARGET_DIR)$(TARGET_ELF) $(OBJ)
  
%.o: %.c
	$(CC) $(CCFLAGS) -c -o $@ $<

clean:
	rm -rf *.o
