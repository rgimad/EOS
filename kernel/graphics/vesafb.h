/*
 * EOS - Experimental Operating System
 * VBE Linear framebuffer driver header
 */

#ifndef _VESAFB_H_
#define _VESAFB_H_

#include <kernel/multiboot.h>

#include <stdbool.h>
#include <stdint.h>

#include <kernel/graphics/vgafnt.h>

#define VESA_WIDTH  framebuffer_width
#define VESA_HEIGHT framebuffer_height

extern uint8_t *framebuffer_addr;
extern uint32_t framebuffer_pitch;
extern uint32_t framebuffer_bpp;
extern uint32_t framebuffer_width;
extern uint32_t framebuffer_height;
extern uint32_t framebuffer_size;

extern uint8_t *back_framebuffer_addr;

typedef struct rgba_struct {
    uint32_t r;
    uint32_t g;
    uint32_t b;
    uint32_t a;
} rgba_color;

void init_vbe(multiboot_info_t *mboot);

void set_pixel(int x, int y, uint32_t color);
void set_pixel_alpha(int x, int y, rgba_color color);

uint32_t get_pixel(int x, int y);

uint32_t rgb(uint8_t r, uint8_t g, uint8_t b);
uint32_t rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

void draw_horizontal_line(int x, int y,int length, uint32_t color);
void draw_vertical_line(int x, int y,int length, uint32_t color);
void draw_square(int x, int y, int width, int height, uint32_t color);

void draw_fill(int start_x, int start_y, int length_across, int length_down, uint32_t color);

void draw_vga_character(uint8_t c, int x, int y, int fg, int bg, bool bgon);
void draw_text_string(const char *text, int x, int y, int fg, int bg, bool bgon);

void create_back_framebuffer();

typedef struct svga_mode_info {
    uint16_t attributes;
    uint8_t windowA, windowB;
    uint16_t granularity;
    uint16_t windowSize;
    uint16_t segmentA, segmentB;
    uint32_t winFuncPtr; // Pointer to INT 0x10 Function 0x4F05
    uint16_t pitch; // Bytes per scan line

    uint16_t screen_width, screen_height; // Resolution
    uint8_t wChar, yChar, planes, bpp, banks; // Number of banks
    uint8_t memoryModel, bankSize, imagePages;
    uint8_t reserved0;

    // Color masks
    uint8_t readMask, redPosition;
    uint8_t greenMask, greenPosition; 
    uint8_t blueMask, bluePosition;
    uint8_t reservedMask, reservedPosition;
    uint8_t directColorAttributes;

    uint32_t physbase; // Pointer to LFB in LFB modes 
    uint32_t offScreenMemOff;
    uint16_t offScreenMemSize;
    uint8_t reserved1[206];
} __attribute__((packed)) svga_mode_info_t;


// DEPRECATED
enum colors  {
    VESA_BLACK = 0x000000,
    VESA_BLUE = 0x0000AA,
    VESA_GREEN = 0x00AA00,
    VESA_CYAN = 0x00AAAA,
    VESA_RED = 0xAA0000,
    VESA_MAGENTA = 0xAA00AA,
    VESA_BROWN = 0xAA5500,
    VESA_LIGHT_GREY = 0xAAAAAA,
    VESA_DARK_GREY = 0x555555,
    VESA_LIGHT_BLUE = 0x5555FF,
    VESA_LIGHT_GREEN = 0x55FF55,
    VESA_LIGHT_CYAN = 0x55FFFF,
    VESA_LIGHT_RED = 0xFF5555,
    VESA_LIGHT_MAGENTA = 0xFF55FF,
    VESA_LIGHT_BROWN = 0xffff55,
    VESA_WHITE = 0xFFFFFF,
};

#endif //_VESAFB_H_
