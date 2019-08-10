/*
*    EOS - Experimental Operating System
*    GDT header
*/
#ifndef _KERNEL_GDT_H_
#define _KERNEL_GDT_H_

#include <stdint.h>

// Sets up the GDT, should be called on early initialization
void gdt_install();

#endif  // _KERNEL_GDT_H_