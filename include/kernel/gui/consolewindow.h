/*
 * EOS - Experimental Operating System
 * Console window header
 */

#ifndef _CONSOLEWINDOW_H
#define _CONSOLEWINDOW_H

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

#define CONSOLEWINDOW_BORDER_SIZE 4
#define CONSOLEWINDOW_HDR_HEIGHT  30

typedef struct consolewindow_t {
    char caption[128];

    uint32_t x, y, width, height; // Width and height are calculated automatically
    uint32_t rows, cols;
    uint32_t frame_color, background_color, text_color;
    uint32_t cursor_x, cursor_y; // In pixels, not in units

    size_t line_fill[1024]; // How many chars in each string was before line break

    //process_t *process;
} __attribute__((packed)) consolewindow_t;

consolewindow_t *consolewindow_create(const char *caption, uint32_t x, uint32_t y, uint32_t rows, uint32_t cols);

void consolewindow_draw(consolewindow_t *cwnd);
void consolewindow_putchar(consolewindow_t *cwnd, char c);
void consolewindow_backspace(consolewindow_t *cwnd);
void consolewindow_scroll(consolewindow_t *cwnd);

void consolewindow_set_text_color(consolewindow_t *cwnd, uint32_t color);
void consolewindow_write(consolewindow_t *cwnd, const char *data, size_t size);

void consolewindow_putstring(consolewindow_t *cwnd, const char* data);
void consolewindow_putuint(consolewindow_t *cwnd, int i);
void consolewindow_putint(consolewindow_t *cwnd, int i);
void consolewindow_puthex(consolewindow_t *cwnd, uint32_t i);

void consolewindow_print(consolewindow_t *cwnd, char *format, va_list args);
void consolewindow_printf(consolewindow_t *cwnd, char *text, ... );

void consolewindow_putstring_color(consolewindow_t *cwnd, const char *data, uint32_t text_color);

#endif //_CONSOLEWINDOW_H
