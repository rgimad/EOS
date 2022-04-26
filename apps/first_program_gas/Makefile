TARGET_ELF = first_program_gas.elf
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

LDFLAGS=-T ../examples/app_gas.ld -ffreestanding -nostdlib -lgcc

OBJ = first_program_gas.o
	
all: $(TARGET_ELF)

$(TARGET_ELF): $(OBJ)
	$(BASH) mkdir -p $(TARGET_DIR)
	$(CC) $(LDFLAGS) -o $(TARGET_DIR)$(TARGET_ELF) $(OBJ)
	
%.o: %.s
	$(AS) -o $@ $<

clean:
	$(BASH) rm -rf *.o
	$(BASH) rm -rf $(TARGET_DIR)$(TARGET_ELF)
