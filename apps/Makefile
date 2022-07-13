# Makefile for building apps for EOS

# if we're on Windows use WSL to run bash commands
ifeq ($(OS),Windows_NT) 
    BASH = wsl
else
    BASH =
endif

all: all_init app_kex_hw app_pe_test

all_init:
	$(BASH) mkdir -p ../bin/apps

app_kex_hw:
	make -C kex_hw

app_pe_test:
	make -C pe_test

clean:
	make -C kex_hw clean
	make -C pe_test clean
