/*
 * EOS - Experimental Operating System
 * Constants and inline functions for VGA
 */

#ifndef _KERNEL_VGA_H
#define _KERNEL_VGA_H

//#include <asm.h>

#include <stdint.h>

#define VGA_WIDTH  80
#define VGA_HEIGHT 25

//static const size_t VGA_WIDTH = 80;
//static const size_t VGA_HEIGHT = 20;

enum vga_color {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_BROWN = 14,
    VGA_COLOR_WHITE = 15,
};

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg)
{
    return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color)
{
    return (uint16_t)uc | (uint16_t)color << 8;
}

// Maybe get from DiOS
//static uint16_t *const VGA_MEMORY = (uint16_t*) 0xB8000;

#endif // _KERNEL_VGA_H
