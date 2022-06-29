/*
 * EOS - Experimental Operating System
 * Kernel mode shell header
 */

#ifndef _KSH_TTY_H
#define _KSH_TTY_H

void ksh_init();
void ksh_main();

void ksh_cmd_cpuid();
void ksh_cmd_ticks();

void ksh_cmd_about();
void ksh_cmd_help();
void ksh_kheap_test();
void ksh_draw_demo();
void ksh_gui_test();
void ksh_syscall_test();

void kthread_grafdemo();

void ksh_cmd_pwd();
void ksh_cmd_cat();
void ksh_cmd_cd();
void ksh_cmd_ls();

void ksh_cmd_elf_info(char *fname);
void ksh_cmd_run(char *fname);
void ksh_cmd_img(const char *fname);
//void ksh_cmd_regdump();

void ksh_cmd_unknown();

#endif // _KSH_TTY_H
