/*
 * EOS - Experimental Operating System
 * Console window implementation
 */

#include <kernel/gui/consolewindow.h>
#include <kernel/graphics/vesafb.h>
#include <kernel/graphics/vgafnt.h>

#include <kernel/libk/string.h>

#include <kernel/mm/kheap.h>

consolewindow_t *consolewindow_create(const char *caption, uint32_t x, uint32_t y, uint32_t rows, uint32_t cols) {
    consolewindow_t *cwnd = kheap_malloc(sizeof(consolewindow_t));
    cwnd->x = x;
    cwnd->y = y;
    cwnd->rows = rows;
    cwnd->cols = cols;
    cwnd->width = cols * 8 + 2 * CONSOLEWINDOW_BORDER_SIZE; // 8 is width of the font
    cwnd->height = rows * 17 + CONSOLEWINDOW_HDR_HEIGHT + CONSOLEWINDOW_BORDER_SIZE; // 16 is width of the font but we use 17 because of space between lines
    cwnd->cursor_x = 0;
    cwnd->cursor_y = 0;
    cwnd->frame_color = VESA_LIGHT_BLUE;
    cwnd->background_color = VESA_DARK_GREY;
    cwnd->text_color = VESA_WHITE;

    memset(cwnd->line_fill, 0, 1024);
    strcpy(cwnd->caption, caption);

    return cwnd;
}

void consolewindow_draw(consolewindow_t *cwnd) {
    draw_fill(cwnd->x, cwnd->y, cwnd->width, cwnd->height, cwnd->frame_color);
    //draw_square(wnd_x, wnd_y, wnd_width, wnd_height, VESA_WHITE);
    draw_fill(cwnd->x + CONSOLEWINDOW_BORDER_SIZE, cwnd->y + CONSOLEWINDOW_HDR_HEIGHT, cwnd->width - 2 * CONSOLEWINDOW_BORDER_SIZE, cwnd->height - CONSOLEWINDOW_HDR_HEIGHT - CONSOLEWINDOW_BORDER_SIZE, cwnd->background_color);
    draw_text_string(cwnd->caption, cwnd->x + CONSOLEWINDOW_BORDER_SIZE, cwnd->y + CONSOLEWINDOW_BORDER_SIZE, cwnd->text_color, cwnd->frame_color, 1);
    //draw_fill(wnd_x + wnd_width - wnd_hdr_height, wnd_y + wnd_border, wnd_hdr_height - 2*wnd_border, wnd_hdr_height - 2*wnd_border, VESA_RED);
    //draw_square(wnd_x + wnd_width - wnd_hdr_height, wnd_y + wnd_border, wnd_hdr_height - 2*wnd_border, wnd_hdr_height - 2*wnd_border, VESA_WHITE);
}

void consolewindow_putchar(consolewindow_t *cwnd, char c) {
    uint32_t client_width = cwnd->cols * 8;
    uint32_t client_height = cwnd->rows * 17;

    if ((cwnd->cursor_x + 8) >= client_width || c == '\n') {
        cwnd->line_fill[cwnd->cursor_y] = cwnd->cursor_x;
        cwnd->cursor_x = 0;

        if ((cwnd->cursor_y + 17) >= client_height) {
            consolewindow_scroll(cwnd);
        } else {
            cwnd->cursor_y += 17;
        }

        if (c == '\n') {
            return;
        }

        draw_vga_character(c, cwnd->x + CONSOLEWINDOW_BORDER_SIZE + cwnd->cursor_x, cwnd->y + CONSOLEWINDOW_HDR_HEIGHT + cwnd->cursor_y, cwnd->text_color, cwnd->background_color, 0);
        cwnd->cursor_x += 8;
    } else {
        if ((cwnd->cursor_y + 17) >= client_height) {
            consolewindow_scroll(cwnd);
        }

        draw_vga_character(c, cwnd->x + CONSOLEWINDOW_BORDER_SIZE + cwnd->cursor_x, cwnd->y + CONSOLEWINDOW_HDR_HEIGHT + cwnd->cursor_y, cwnd->text_color, cwnd->background_color, 0);
        cwnd->cursor_x += 8;
    }
}

void consolewindow_scroll(consolewindow_t *cwnd) {
    unsigned int num_rows = 1; // na skolko vverh
    cwnd->cursor_y -= 17 * num_rows;

    //TODO: rewrite following code for console window scrolling:

    /*
    // Copy rows upwards
    uint8_t *read_ptr = (uint8_t *) back_framebuffer_addr + ((num_rows * 17) * framebuffer_pitch);
    uint8_t *write_ptr = (uint8_t *) back_framebuffer_addr;
    uint32_t num_bytes = (framebuffer_pitch * VESA_HEIGHT) - (framebuffer_pitch * (num_rows * 17));//old: unsigned old
    memcpy(write_ptr, read_ptr, num_bytes);

    // Clear the rows at the end
    write_ptr = (uint8_t *) back_framebuffer_addr + (framebuffer_pitch * VESA_HEIGHT) - (framebuffer_pitch * (num_rows * 17));
    memset(write_ptr, 0, framebuffer_pitch * (num_rows * 17));

    //swap buffers
    memcpy(framebuffer_addr, back_framebuffer_addr, framebuffer_size);*/
}

void consolewindow_backspace(consolewindow_t *cwnd) {
    if (cwnd->cursor_x < 8) {
        if (cwnd->cursor_y >= 17) {
            cwnd->cursor_y -= 17;
        }
        cwnd->cursor_x = cwnd->line_fill[cwnd->cursor_y];
    } else {
        cwnd->cursor_x -= 8;
    }

    draw_vga_character(' ', cwnd->x + CONSOLEWINDOW_BORDER_SIZE + cwnd->cursor_x, cwnd->y + CONSOLEWINDOW_HDR_HEIGHT + cwnd->cursor_y, cwnd->text_color, cwnd->background_color, 1);
}

void consolewindow_set_text_color(consolewindow_t *cwnd, uint32_t color) {
    cwnd->text_color = color;
}

void consolewindow_write(consolewindow_t *cwnd, const char *data, size_t size) {
    for (size_t i = 0; i < size; i++) {
          consolewindow_putchar(cwnd, data[i]);
    }
}

void consolewindow_putstring(consolewindow_t *cwnd, const char *data) {
    consolewindow_write(cwnd, data, strlen(data));
}

void consolewindow_putuint(consolewindow_t *cwnd, int i) {
    unsigned int n, d = 1000000000;
    char str[255];
    unsigned int dec_index = 0;

    while ((i / d == 0 ) && (d >= 10)) {
        d /= 10;
    }
    n = i;

    while (d >= 10) {
        str[dec_index++] = ((char) ((int)'0' + n/d));
        n = n % d;
        d /= 10;
    }

    str[dec_index++] = ((char) ((int)'0' + n));
    str[dec_index] = 0;
    consolewindow_putstring(cwnd, str);
}

void consolewindow_putint(consolewindow_t *cwnd, int i) {
    if(i >= 0) {
        consolewindow_putuint(cwnd, i);
    } else {
        consolewindow_putchar(cwnd, '-');
        consolewindow_putuint(cwnd, -i);
    }
}

void consolewindow_puthex(consolewindow_t *cwnd, uint32_t i) {
    const unsigned char hex[16]  =  { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
    unsigned int n, d = 0x10000000;

    consolewindow_putstring(cwnd, "0x");

    while ((i / d == 0) && (d >= 0x10)) {
        d /= 0x10;
    }
    n = i;

    while (d >= 0xF) {
        consolewindow_putchar(cwnd, hex[n/d]);
        n = n % d;
        d /= 0x10;
    }
    consolewindow_putchar(cwnd, hex[n]);
}

void consolewindow_print(consolewindow_t *cwnd, char *format, va_list args) {
    int i = 0;
    char *string;

    while (format[i]) {
        if (format[i] == '%') {
            i++;
            switch (format[i]) {
            case 's':
                string = va_arg(args, char*);
                consolewindow_putstring(cwnd, string);
                break;
            case 'c':
                // To-Do: fix this! "warning: cast to pointer from integer of different size"
                consolewindow_putchar(cwnd, va_arg(args, int));
                break;
            case 'd':
                consolewindow_putint(cwnd, va_arg(args, int));
                break;
            case 'i':
                consolewindow_putint(cwnd, va_arg(args, int));
                break;
            case 'u':
                consolewindow_putuint(cwnd, va_arg(args, unsigned int));
                break;
            case 'x':
                consolewindow_puthex(cwnd, va_arg(args, uint32_t));
                break;
            default:
                consolewindow_putchar(cwnd, format[i]);
            }
        } else {
            consolewindow_putchar(cwnd, format[i]);
        }

        i++;
    }
}

void consolewindow_printf(consolewindow_t *cwnd, char *text, ... ) { //TODO check the code
    va_list args;
    // find the first implicit argument
    va_start(args, text);
    // pass print the output handle the format text and the first argument
    consolewindow_print(cwnd, text, args);
    va_end(args);
}

void consolewindow_putstring_color(consolewindow_t *cwnd, const char *data, uint32_t text_color) {
    uint32_t cwnd_text_color_old = cwnd->text_color;
    consolewindow_set_text_color(cwnd, text_color);
    consolewindow_putstring(cwnd, data);
    consolewindow_set_text_color(cwnd, cwnd_text_color_old);
}
