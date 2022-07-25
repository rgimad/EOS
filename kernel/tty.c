/*
 * EOS - Experimental Operating System
 * Terminal subsystem
 */

#include <kernel/tty.h>
#include <kernel/graphics/vesafb.h>

#include <kernel/libk/string.h>

volatile uint8_t tty_feedback = 1;

size_t tty_line_fill[1024];

int tty_pos_x;
int tty_pos_y;

uint32_t tty_text_color;

void tty_init()
{
    tty_pos_y = 0;
    tty_pos_x = 0;

    tty_text_color = VESA_LIGHT_CYAN;
}

void tty_backspace()
{
    if (tty_pos_x < 8) {
        if (tty_pos_y >= 17) {
            tty_pos_y -= 17;
        }
        tty_pos_x = tty_line_fill[tty_pos_y];
    } else {
        tty_pos_x -= 8;
    }
    draw_vga_character(' ', tty_pos_x, tty_pos_y, tty_text_color, 0x000000, 1);
}

void tty_setcolor(uint32_t color)
{
    tty_text_color = color;
}

void tty_putchar(char c)
{
    if ((tty_pos_x + 8) >= (int)VESA_WIDTH || c == '\n') {
        tty_line_fill[tty_pos_y] = tty_pos_x;
        tty_pos_x = 0;
        if ((tty_pos_y + 17) >= (int)VESA_HEIGHT) {
            tty_scroll();
        } else {
            tty_pos_y += 17;
        }
    } else {
        if ((tty_pos_y + 17) >= (int)VESA_HEIGHT) {
            tty_scroll();
        }
        draw_vga_character(c, tty_pos_x, tty_pos_y, tty_text_color, 0x000000, 0);
        tty_pos_x += 8;
    }
}

// Scrolls the display up number of rows
void tty_scroll()
{
    // charheight = 16???
    unsigned int num_rows = 1;
    tty_pos_y -= 17 * num_rows;

    // Copy rows upwards
    uint8_t *read_ptr = (uint8_t *)back_framebuffer_addr + ((num_rows * 17) * framebuffer_pitch);
    uint8_t *write_ptr = (uint8_t *)back_framebuffer_addr;
    uint32_t num_bytes = (framebuffer_pitch * VESA_HEIGHT) - (framebuffer_pitch * (num_rows * 17)); //old: unsigned old
    memcpy(write_ptr, read_ptr, num_bytes);

    // Clear the rows at the end
    write_ptr = (uint8_t *)back_framebuffer_addr + (framebuffer_pitch * VESA_HEIGHT) - (framebuffer_pitch * (num_rows * 17));
    memset(write_ptr, 0, framebuffer_pitch * (num_rows * 17));

    // Swap buffers
    memcpy(framebuffer_addr, back_framebuffer_addr, framebuffer_size);
}

void tty_write(const char *data, size_t size)
{
    for (size_t i = 0; i < size; i++)
        tty_putchar(data[i]);
}

void tty_putstring(const char *data)
{
    tty_write(data, strlen(data));
}

void tty_putstring_color(const char *data, uint32_t text_color)
{
    uint32_t tty_text_color_old = tty_text_color;
    tty_setcolor(text_color);
    tty_putstring(data);
    tty_setcolor(tty_text_color_old);
}

void tty_printf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vsnprintf(sprintf_buffer, sizeof(sprintf_buffer), format, args);
    tty_putstring(sprintf_buffer);
}


