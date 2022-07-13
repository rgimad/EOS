CC = clang
LD = lld-link

CCFLAGS =  -target i386-pc-win32 -undef -ffreestanding -fno-builtin -c
LDFLAGS_EXE = /nodefaultlib /entry:main /subsystem:native
LDFLAGS_DLL = /nodefaultlib /entry:DllMain /dll /subsystem:native

TARGET_DIR = ../../bin/apps

TARGET_EXE = $(TARGET_DIR)/hello.exe
TARGET_DLL = $(TARGET_DIR)/printf.dll

TARGET_IMPLIB = ./printf.lib

# if we're on Windows use WSL to run bash commands
ifeq ($(OS),Windows_NT)
    BASH = wsl
else
    BASH =
endif

all: $(TARGET_DLL) $(TARGET_EXE)

$(TARGET_EXE): hello_exe.o
	$(BASH) mkdir -p $(TARGET_DIR)
	$(LD) $(LDFLAGS_EXE) $(TARGET_IMPLIB) $< /out:$@ 

$(TARGET_DLL): printf_dll.o
	$(BASH) mkdir -p $(TARGET_DIR) 
	$(LD) $(LDFLAGS_DLL) $< /out:$@ /implib:$(TARGET_IMPLIB)

%.o: %.c
	$(CC) $(CCFLAGS) -o $@ $<

clean:
	$(BASH) rm -rf $(TARGET_EXE) $(TARGET_DLL) $(TARGET_IMPLIB) *.o