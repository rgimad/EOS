/*
*    EOS - Experimental Operating System
*    GDT header
*/
#ifndef _KERNEL_GDT_H_
#define _KERNEL_GDT_H_

#include <stdint.h>

#define GDT_MAX_DESCRIPTORS 6

void gdt_set_gate(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);

// Sets up the GDT, should be called on early initialization
void gdt_install();

#endif  // _KERNEL_GDT_H_