TARGET_ELF = grafdemo.elf
TARGET_DIR = ../../bin/apps/

AS = i686-elf-as
CC = i686-elf-gcc
LD = i686-elf-ld

# if we're on Windows use WSL to run bash commands
ifeq ($(OS),Windows_NT) 
    BASH = wsl
else
    BASH =
endif

LDFLAGS=-T ../examples/app_c.ld -ffreestanding -nostdlib -lgcc
CCFLAGS=-std=gnu99 -ffreestanding -nostdlib -lgcc -Wall -Wextra

OBJ = grafdemo.o
  
all: $(TARGET_ELF)

$(TARGET_ELF): $(OBJ)
	$(BASH) mkdir -p $(TARGET_DIR)
	$(CC) $(LDFLAGS) -o $(TARGET_DIR)$(TARGET_ELF) $(OBJ)
  
%.o: %.c
	$(CC) $(CCFLAGS) -c -o $@ $<

clean:
	$(BASH) rm -rf *.o
	$(BASH) rm -rf $(TARGET_DIR)$(TARGET_ELF)
