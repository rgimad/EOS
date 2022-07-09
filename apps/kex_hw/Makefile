TARGET_BIN = kex_hw.kex
TARGET_DIR = ../../bin/apps/

# if we're on Windows use WSL to run bash commands
ifeq ($(OS),Windows_NT) 
    BASH = wsl
else
    BASH =
endif

all: $(TARGET_BIN)

$(TARGET_BIN): kex_hw.asm
	$(BASH) mkdir -p $(TARGET_DIR)
	fasm kex_hw.asm $(TARGET_DIR)$(TARGET_BIN)

clean:
	$(BASH) rm -rf $(TARGET_DIR)$(TARGET_BIN)
