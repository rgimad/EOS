/*
 * EOS - Experimental Operating System
 * Terminal header file
 */

#ifndef _KERNEL_TTY_H
#define _KERNEL_TTY_H

#include <stdbool.h>
#include <kernel/libk/sprintf.h>

extern volatile uint8_t tty_feedback;

void tty_init(void);
void tty_setcolor(uint32_t color);

void draw_vga_character(uint8_t c, int x, int y, int fg, int bg, bool bgon);

void tty_putchar(char c);
void tty_write(const char *data, size_t size);
void tty_putstring(const char *data);
void tty_putstring_color(const char *data, uint32_t text_color);
void tty_backspace();

void tty_printf(const char *format, ...);
void tty_scroll();

#endif
