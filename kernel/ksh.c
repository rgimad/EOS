/*
 * EOS - Experimental Operating System
 * Kernel mode shell
 */

#include <kernel/ksh.h>
#include <kernel/tty.h>
#include <kernel/vga.h>
#include <kernel/graphics/vesafb.h>
#include <kernel/devices/keyboard.h>
#include <kernel/cpu_detect.h>
#include <kernel/devices/timer.h>

#include <kernel/mm/kheap.h>

#include <kernel/fs/vfs.h>
#include <kernel/fs/initrd.h>

#include <kernel/pm/elf.h>
#include <kernel/pm/pe.h>

#include <kernel/kernel.h>

#include <kernel/io/qemu_log.h>
#include <kernel/libk/string.h>
#include <kernel/libk/assert.h>

#include <kernel/gui/consolewindow.h>

#include <stdint.h>

#define STBI_NO_STDIO 1
#define STBI_NO_GIF   1
#define STB_IMAGE_IMPLEMENTATION 1
#define STBI_NO_LINEAR 1
#define STBI_NO_HDR 1
// #define STBI_NO_FAILURE_STRINGS 1
#define STBI_NO_THREAD_LOCALS 1 // !
#define STBI_ASSERT assert
#define STBI_MALLOC(sz) kheap_malloc(sz)
#define STBI_REALLOC(p,newsz) kheap_realloc(p,newsz)
#define STBI_FREE(p) kheap_free(p)
#include <kernel/utils/stb_image.h>

char ksh_working_directory[256];

void ksh_init() {
    tty_putstring_color("                  EOS KSH (Kernel SHell):\n\n", VESA_LIGHT_RED);
    strcpy(ksh_working_directory, "/initrd/");
}

void ksh_main() {
    char cmd[256];

    while (1) {
        tty_setcolor(VESA_LIGHT_GREEN);
        tty_printf("kernel ");
        tty_setcolor(VESA_LIGHT_BLUE);
        tty_printf("%s $ ", ksh_working_directory);
        tty_setcolor(VESA_LIGHT_CYAN);

        keyboard_gets(cmd, 256);
        if (strlen(cmd) == 0) {
            continue;
        }

        if (strcmp(cmd, "cpuid") == 0) {
            ksh_cmd_cpuid();
        } else if (strcmp(cmd, "about") == 0) {
            ksh_cmd_about();
        } else if (strcmp(cmd, "help") == 0) {
            ksh_cmd_help();
        } else if (strcmp(cmd, "ticks") == 0) {
            ksh_cmd_ticks();
        } else if (strcmp(cmd, "kheap_test") == 0) {
            ksh_kheap_test();
        } else if (strcmp(cmd, "draw_demo") == 0) {
            ksh_draw_demo();
        } else if (strcmp(cmd, "gui_test") == 0) {
            ksh_gui_test();
        }  else if (strcmp(cmd, "syscall_test") == 0) {
            ksh_syscall_test();
        } else if (strcmp(cmd, "pwd") == 0) {
            ksh_cmd_pwd();
        } else if (strlen(cmd) > 4 && strncmp(cmd, "cat ", 4) == 0) {
            char fname[100];
            char *tok = strtok(cmd, " ");
            tok = strtok(0, " "); // tok - now is filename

            if (fname != 0) {
                ksh_cmd_cat(tok);
            } else {
                tty_printf("cat: incorrect argument\n");
            }
        } else if (strlen(cmd) > 3 && strncmp(cmd, "cd ", 3) == 0) {
            char dname[100];
            char *tok = strtok(cmd, " ");
            tok = strtok(0, " "); // tok - now is dirname

            if (dname != 0) {
                ksh_cmd_cd(tok);
            } else {
                tty_printf("cd: incorrect argument\n");
            }
        } else if (strcmp(cmd, "ls") == 0) {
            ksh_cmd_ls();
        } else if (strlen(cmd) > 9 && strncmp(cmd, "elf_info ", 9) == 0) {
            char fname[100];
            char *tok = strtok(cmd, " ");
            tok = strtok(0, " "); // tok - now is filename

            if (fname != 0) {
                ksh_cmd_elf_info(tok);
            } else {
                tty_printf("elf_info: incorrect argument\n");
            }
        } else if (strlen(cmd) > 4 && strncmp(cmd, "run ", 4) == 0) {
            char fname[100];
            char *tok = strtok(cmd, " ");
            tok = strtok(0, " "); // tok - now is filename

            if (fname != 0) {
                ksh_cmd_run(tok);
            } else {
                tty_printf("run: incorrect argument\n");
            }
        } else if (strcmp(cmd, "cwnd_test") == 0) {
            consolewindow_t *cwnd1 = consolewindow_create("Test window 1", 450, 200, 7, 45);
            consolewindow_draw(cwnd1);
            consolewindow_printf(cwnd1, "Hello world = %d", 1337);
            consolewindow_backspace(cwnd1);
            consolewindow_printf(cwnd1, "\nPrivet mir!Privet mir!Privet mir!Privet mir!Privet mir!Privet mir!Privet mir!");

            consolewindow_t *cwnd2 = consolewindow_create("Window 2", 400, 450, 5, 35);
            consolewindow_draw(cwnd2);

            int a = 0, b = 1, c;
            for (int i = 0; i < 19; i++) {
                consolewindow_printf(cwnd2, "%d, ", a);
                c = a + b;
                a = b;
                b = c;
            }
        } else if (strcmp(cmd, "qemu_log_test") == 0) {
            qemu_printf("Hello world = %x + %s", 0x779, "privet");
        } else if (strcmp(cmd, "reg_modif") == 0) {
            int val = 13372;
            asm volatile("mov %0, %%eax" :: "r"(val));
            asm volatile("int $32;");
        } else if (strlen(cmd) > 4 && strncmp(cmd, "img ", 4) == 0) {
            char fname[100];
            char *tok = strtok(cmd, " ");
            tok = strtok(0, " "); // tok - now is filename

            if (fname != 0) {
                ksh_cmd_img(tok);
            } else {
                tty_printf("img: incorrect argument\n");
            }
        } else {
            ksh_cmd_unknown();
        }
    }
}

// Command handlers implementation

void kthread_grafdemo() { // TODO why invalid opcode on 0x4C happens ????
    int i;
    while (1) {
        //qemu_printf("iteration\n");
        /*asm volatile ("cli");
        for (i = 0; i < 1000; i++) draw_square(700, 250, 300 - i % 300, 300 - i % 300, 0x00AAAA);
        for (i = 0; i < 1000; i++) draw_square(700, 250, 300 - i % 300, 300 - i % 300, 0xAA0000);
        asm volatile ("sti");*/

        // TODO understand : why if we dont wrap these code in cli ... sti i see a garbage on the screen? maybe context is corrupting?
        asm volatile ("cli");
        for (i = 0; i < 50; i++) {
            unsigned int arguments[5];
            arguments[0] = 700;
            arguments[1] = 250;
            arguments[2] = 300 - i % 300;
            arguments[3] = 300 - i % 300;
            arguments[4] = 0x00AAAA;
            unsigned int res = 0;
            asm volatile("mov %%eax, %0;" : "=a"(res) : "a"(2), "b"(arguments));
            asm volatile("int $0x80;");
        }

        for (i = 0; i < 50; i++) {
            unsigned int arguments[5];
            arguments[0] = 700;
            arguments[1] = 250;
            arguments[2] = 300 - i % 300;
            arguments[3] = 300 - i % 300;
            arguments[4] = 0xAA0000;
            unsigned int res = 0;
            asm volatile("mov %%eax, %0;" : "=a"(res) : "a"(2), "b"(arguments));
            asm volatile("int $0x80;");
        }

        asm volatile("sti");
    }
}

void ksh_cmd_cpuid() {
    detect_cpu();
    tty_printf("\n");
}

void ksh_cmd_unknown() {
    tty_printf("Error: unknown command.\n");
}

void ksh_cmd_about() {
    tty_printf("EOS v0.1\n");
}

void ksh_cmd_ticks() {
    tty_printf("Timer ticks = %d\n", timer_get_ticks());
}

void ksh_kheap_test() {
    kheap_test();
}

void ksh_draw_demo() {
    draw_fill(0, 500, framebuffer_width, framebuffer_height - 500, 0x0000AA);

    int arr[10] = {0x00AA00, 0x00AAAA, 0xAA0000, 0xAA00AA, 0xAA5500, 0xAAAAAA, 0x555555, 0x5555FF, 0x55FF55, 0x55FFFF};
    int i;
    for (i = 0; i < 30; i++) {
        draw_square(30 + 7 * i, 30 + 7 * i, 200, 300, arr[i % 10]);
    }

    char chr;
    for (chr = 32; chr <= '~'; chr++) {
        draw_vga_character(chr, 500 + ((chr - 32) % 10) * 20, 50 + ((chr - 32) / 10) * 20, 0x00AA00, 0x0000AA, 0);
    }
}

void ksh_gui_test() {
    int wnd_x = 400, wnd_y = 200, wnd_width = 500, wnd_height = 300,  wnd_border = 4, wnd_hdr_height = 30;
    uint32_t wnd_frame_color = VESA_LIGHT_BLUE, wnd_background_color = VESA_LIGHT_GREY;
    draw_fill(wnd_x, wnd_y, wnd_width, wnd_height, wnd_frame_color);
    //draw_square(wnd_x, wnd_y, wnd_width, wnd_height, VESA_WHITE);
    draw_fill(wnd_x + wnd_border, wnd_y + wnd_hdr_height, wnd_width - 2*wnd_border, wnd_height - wnd_hdr_height - wnd_border, wnd_background_color);
    draw_fill(wnd_x + wnd_width - wnd_hdr_height, wnd_y + wnd_border, wnd_hdr_height - 2*wnd_border, wnd_hdr_height - 2*wnd_border, VESA_RED);
    draw_square(wnd_x + wnd_width - wnd_hdr_height, wnd_y + wnd_border, wnd_hdr_height - 2*wnd_border, wnd_hdr_height - 2*wnd_border, VESA_WHITE);
}

void ksh_cmd_pwd() {
    tty_printf("%s\n", ksh_working_directory);
}

void ksh_cmd_cat(char *fname) {
    if (fname[0] != '/') { //TODO: make function
        char temp[256];
        strcpy(temp, ksh_working_directory);
        strcat(temp, fname);
        strcpy(fname, temp);
    }

    char *buf = (char*) kheap_malloc(1000);

    if (!vfs_exists(fname)) {
        tty_printf("cat: error file not found\n");
    } else {
        uint32_t fsize = vfs_get_size(fname);
        int res = vfs_read(fname, 0, fsize, buf);
        (void)res;
        buf[fsize] = '\0';
        tty_printf("cat: file %s:\n\n%s\n", fname, buf);
        kheap_free(buf);
    }
}

void ksh_cmd_cd(char *dname) {
    if (dname[0] != '/') {
        char temp[256];
        strcpy(temp, ksh_working_directory);
        strcat(temp, dname);
        strcpy(dname, temp);
    }

    if (dname[strlen(dname) - 1] != '/') { //very important, otherwise vfs won't see the dir
        strcat(dname, "/");
    }

    if (vfs_exists(dname) && vfs_is_dir(dname)) {
        strcpy(ksh_working_directory, dname);
    } else {
        tty_printf("cd: no such directory\n");
    }
}

void ksh_cmd_ls() {
    initrd_list(0, 0);
    tty_printf("\n");
}

void ksh_cmd_elf_info(char *fname) {
    if (fname[0] != '/') { // TODO: make function
        char temp[256];
        strcpy(temp, ksh_working_directory);
        strcat(temp, fname);
        strcpy(fname, temp);
    }

    tty_printf("elf fname = %s\n", fname);
    elf_info_short(fname);
}

void ksh_cmd_img(const char *fname) {
    if (fname[0] != '/') { //TODO: make function
        char temp[256];
        strcpy(temp, ksh_working_directory);
        strcat(temp, fname);
        strcpy(fname, temp);
    }

    if (!vfs_exists(fname)) {
        tty_printf("img: error file not found\n");
    } else {
        uint32_t fsize = vfs_get_size(fname);
        uint8_t *buf = (char*) kheap_malloc(fsize);
        int res = vfs_read(fname, 0, fsize, buf);
        (void)res;
        int x, y, ch;
        tty_printf("decoding image..\n");
        uint8_t *res_img = stbi_load_from_memory(buf, fsize, &x, &y, &ch, 3);
        tty_printf("image decoded:\n  width = %d\n  height = %d\n  channels = %d\n", x, y, ch);
        for (int i = 0; i < y; i++) {
            for (int j = 0; j < x; j++) {
                uint32_t col = ((uint32_t)(res_img[(i*x + j)*ch]) << 16) + ((uint32_t)(res_img[(i*x + j)*ch + 1]) << 8) + ((uint32_t)(res_img[(i*x + j)*ch + 2]));
                set_pixel(400 + j, 150 + i, col);
            }
        }
        stbi_image_free(res_img);
        kheap_free(buf);
    }
}

void ksh_cmd_run(char *fname) {
    int status = 0;

    if (fname[0] != '/') { // TODO: make function
        char temp[256];
        strcpy(temp, ksh_working_directory);
        strcat(temp, fname);
        strcpy(fname, temp);
    }

    if(!vfs_exists(fname)) {
        tty_printf("File not found!\n");
        return;
    }

    uint32_t signature = 0;
    vfs_read(fname, 0, sizeof(signature), &signature);

    if (signature == ELF_SIGNATURE) { 
        status = run_elf_file(fname);
    } else if ((uint16_t)signature == PE_IMAGE_DOS_SIGNATURE) {
        status = run_pe_file(fname);
    } else {
        tty_printf("Unknown executable file format\n");
        return;
    }
    tty_printf("\nProgram exited with code %d\n", status);
}

void ksh_syscall_test() {
    uint32_t sc_code = 0; uint32_t res = 0;
    char *str = "Hello i'm system call !\n";

    asm volatile("mov %%eax, %0;" : "=a"(res) : "a"(sc_code), "b"(&str));
    asm volatile("int $0x80;");
}

void ksh_cmd_regdump() {
    //uint32_t eax, ebx, ecx, edx, esi, edi, esp, ebp, cr0, cr2, cr3;
}

void ksh_cmd_help() {
    tty_printf("Available commands:\n cpuid - information about processor\n ticks - get number of ticks\n kheap_test - test kernel heap\n draw_demo - demo super effects\n syscall_test - test system calls work\n ls - list of files and dirs\n cd - set current directory\n pwd - print working directory\n cat - print contents of specified file\n gui_test - draw test window\n elf_info - information about elf file\n img - open graphic image file\n run - run program (for example - run first_program_gas.elf)\n cwnd_test - console window system test\n qemu_log_test\n about - about EOS\n help\n");
}
