/*
 * EOS - Experimental Operating System
 * Header for Task State Segment methods and structures
 */

#ifndef TSS_H
#define TSS_H

#include <stdint.h>
#include <stdbool.h>

// tss entry structure
typedef struct tss_entry {
    uint32_t prev_tss;

    uint32_t esp0;
    uint32_t ss0;
    uint32_t esp1;
    uint32_t ss1;
    uint32_t esp2;
    uint32_t ss2;
    uint32_t cr3;

    uint32_t eip;
    uint32_t eflags;

    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;

    uint32_t esi;
    uint32_t edi;

    uint32_t es;
    uint32_t cs;
    uint32_t ss;
    uint32_t ds;
    uint32_t fs;
    uint32_t gs;

    uint32_t ldt;
    uint16_t trap;
    uint16_t iomap_base;
}  __attribute__((packed)) tss_entry_t;

/*
typedef struct tss_descriptor {
    u16int   limit_15_0;      // Биты 15-0 лимита 
    u16int   base_15_0;       // Биты 15-0 базы 
    u8int    base_23_16;      // Биты 23-16 базы 
    u8int    type:4;          // Тип сегмента 
    u8int    sys:1;           // Системный сегмент 
    u8int    DPL:2;           // Уровень привилегий сегмента 
    u8int    present:1;       // Бит присутствия 
    u8int    limit_19_16:4;   // Биты 19-16 лимита 
    u8int    AVL:1;           // Зарезервирован 
    u8int    allways_zero:2;  // Всегда нулевые 
    u8int    gran:1;          // Бит гранулярности 
    u8int    base_31_24;      // Биты 31-24 базы 
} __attribute__((packed)) tss_descriptor_t;
*/

extern void tss_flush(); // Implemented in tss_asm.s
void tss_init(uint32_t idx, uint32_t kss, uint32_t kesp);
void tss_set_stack(uint32_t kss, uint32_t kesp);

#endif // TSS_H
