TARGET_BIN = kex_hw.kex
TARGET_DIR = ../../bin/apps/

all: $(TARGET_BIN)

$(TARGET_BIN): kex_hw.asm
	mkdir -p $(TARGET_DIR)
	fasm kex_hw.asm $(TARGET_DIR)$(TARGET_BIN)

clean:
	rm -rf $(TARGET_DIR)$(TARGET_BIN)
