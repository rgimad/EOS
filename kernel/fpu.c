/**
 * @file fpu.c
 * @author Арен Елчинян (a2.dev@yandex.com)
 * @brief Функции для работы с FPU
 * @version 0.1.0
 * @date 01-1-2023
 * 
 * @copyright Арен Елчинян (c) 2023
 * 
 */

#include <stdbool.h>
#include <stdint.h>


/**
 * @brief Тестирование FPU
 * 
 */
bool fpu_test() {
    float a = 5;
    float x = 0;

    x = a / 2;

    if (x != 2.5f) {
        return false;
    }

    return true;
}


/**
 * @brief Установка контрольного слова fldcw
 * 
 * @param control_word Контрольное слово
 */
static void fpu_fldcw(uint16_t control_word) {
    asm volatile("fldcw %0"::"m"(control_word));
}



/**
 * @brief Инициализация FPU
 * 
 */
bool fpu_init() {
    uint32_t cpu;
    asm volatile("movl $0x1, %%eax\n cpuid\n"
                "movl %%edx, %0\n": "=g"(cpu)::"%eax", "%ebx", "%ecx", "%edx");
        
    if (cpu & 0x1) {
        asm volatile("finit");

        uint32_t cr4 = 0;
        
        asm volatile ("mov %%cr4, %0":"=r"(cr4));
        cr4 |= 0x200;
        asm volatile("mov %0, %%cr4"::"r"(cr4));
        fpu_fldcw(0x37F);

        return fpu_test();
    }

    // FPU для этой платформы не предусмотрен
    return false;
}