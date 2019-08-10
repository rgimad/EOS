/*
*    EOS - Experimental Operating System
*    Kernel mode shell header
*/
#ifndef _KSH_TTY_H
#define _KSH_TTY_H

void ksh_init();
void ksh_main();

void ksh_cmd_cpuid();
void ksh_cmd_ticks();

void ksh_cmd_about();
void ksh_cmd_help();
//void ksh_cmd_regdump();

void ksh_cmd_unknown();




#endif  // _KSH_TTY_H