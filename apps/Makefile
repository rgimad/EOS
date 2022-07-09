# Makefile for building apps for EOS

# if we're on Windows use WSL to run bash commands
ifeq ($(OS),Windows_NT) 
    BASH = wsl
else
    BASH =
endif

all: all_init app_first_program_gas app_reverse_string app_grafdemo app_kex_hw

all_init:
	$(BASH) mkdir -p ../bin/apps

app_first_program_gas:
	make -C first_program_gas

app_reverse_string:
	make -C reverse_string

app_grafdemo:
	make -C grafdemo

app_kex_hw:
	make -C kex_hw

clean:
	make -C first_program_gas clean
	make -C reverse_string clean
	make -C grafdemo clean
	make -C kex_hw clean
