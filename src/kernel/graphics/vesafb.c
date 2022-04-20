/*
 * EOS - Experimental Operating System
 * VBE Linear framebuffer driver
 */

#include <kernel/graphics/vesafb.h>
#include <kernel/mm/virt_memory.h>
#include <kernel/mm/kheap.h>
#include <kernel/tty.h>

#include <libk/string.h>

void init_vbe(multiboot_info *mboot) {
    //if (mboot->framebuffer_type != 1) {
        //panic
        //tty_printf("Invalid framebuffer type\n");
        //return;
    //}

    //framebuffer_addr = vmm_temp_map_page((uint8_t*) (*mboot).framebuffer_addr);
    //framebuffer_addr = (uint8_t*) (*mboot).framebuffer_addr;
    //framebuffer_pitch = mboot->framebuffer_pitch;
    //framebuffer_bpp = mboot->framebuffer_bpp;
    //framebuffer_width = mboot->framebuffer_width;
    //framebuffer_height = mboot->framebuffer_height;

    //tty_printf("lfb = %x\n", mboot->framebuffer_addr);
    //tty_printf("bpp = %x\n", mboot->framebuffer_bpp);

    svga_mode_info_t *svga_mode = (svga_mode_info_t*) mboot->vbe_mode_info;
    framebuffer_addr = (uint8_t *)svga_mode->physbase; //vmm_temp_map_page(svga_mode->physbase);
    framebuffer_pitch = svga_mode->pitch;
    framebuffer_bpp = svga_mode->bpp;
    framebuffer_width = svga_mode->screen_width;
    framebuffer_height = svga_mode->screen_height;
    //framebuffer_size = framebuffer_width * framebuffer_height * (framebuffer_bpp / 8);
    framebuffer_size = framebuffer_height * framebuffer_pitch;

    physical_addr frame;
    virtual_addr virt;
    for (frame = (physical_addr)framebuffer_addr, virt = (virtual_addr)framebuffer_addr;
         frame < ((physical_addr)framebuffer_addr + framebuffer_size/*0x00400000*//*0x002C0000 0x000F0000*/);
         frame += PAGE_SIZE, virt += PAGE_SIZE) {
        vmm_map_page(frame, virt);
    }

    create_back_framebuffer();

    //tty_printf("xxx = %x\n", *(uint8_t *)0xC0800000);

    /*
    size_t i;
    for (i = 0; i < framebuffer_size; ++i) {
        ((uint8_t*) (back_framebuffer_addr))[i] = 0;
    }
    */

    //back_framebuffer_addr = framebuffer_addr;

    //asm volatile( "movl %0, %%ecx" :: "r" (framebuffer_pitch));
    //for (;;) asm("hlt");
}

void create_back_framebuffer() {
    //flush_tlb_entry(back_framebuffer_addr);
    back_framebuffer_addr = kheap_malloc(framebuffer_size);
    tty_printf("back_framebuffer_addr = %x\n", back_framebuffer_addr);
    //tty_printf("init_vbe: [c0800000]->%x\n", page_table_entry_is_writable(GET_PTE(0xC0800000)));
    memset(back_framebuffer_addr, 0, framebuffer_size); //causes page fault at c0800000 when this line is placed in the end of init_vbe
}

void set_pixel(int x, int y, uint32_t color) {
    if (x < 0 || y < 0 || 
        x >= (int) VESA_WIDTH || 
        y >= (int) VESA_HEIGHT) {
        return;
    }

    unsigned where = x * (framebuffer_bpp / 8) + y * framebuffer_pitch;

    framebuffer_addr[where] = color;
    framebuffer_addr[where + 1] = color >> 8;
    framebuffer_addr[where + 2] = color >> 16;

    //asm("movl %0, %%ecx" : : "r"(where));
    //for(;;);
    //double buffering

    back_framebuffer_addr[where] = color & 255;
    back_framebuffer_addr[where + 1] = (color >> 8) & 255;
    back_framebuffer_addr[where + 2] = (color >> 16) & 255;

    //memcpy(framebuffer_addr + where, back_framebuffer_addr + where, 3);
    //framebuffer_addr[where] = back_framebuffer_addr[where];
    //framebuffer_addr[where + 1] = back_framebuffer_addr[where + 1];
    //framebuffer_addr[where + 2] = back_framebuffer_addr[where + 2];
}

void set_pixel_alpha(int x, int y, rgba_color color) {
    unsigned where = x * (framebuffer_bpp / 8) + y * framebuffer_pitch;

    if (color.a != 255) {
        if (color.a != 0) {
            framebuffer_addr[where] = (255.0f / color.b) * (255.0f / color.a);
            framebuffer_addr[where + 1] = (255.0f / color.g) * (255.0f / color.a);
            framebuffer_addr[where + 2] = (255.0f / color.r) * (255.0f / color.a);
        } else {
            return;
        }
    } else {
        framebuffer_addr[where] = color.b;
        framebuffer_addr[where + 1] = color.g;
        framebuffer_addr[where + 2] = color.r;
    }
}

// Return color of the pixel
uint32_t get_pixel(int x, int y) {
    if (x < 0 || y < 0 || 
        x >= (int) VESA_WIDTH || 
        y >= (int) VESA_HEIGHT) {
        return 0;
    }

    unsigned where = x * (framebuffer_bpp / 8) + y * framebuffer_pitch;

    uint32_t color = 0;
    color = framebuffer_addr[where + 0] + 
            (framebuffer_addr[where + 1] << 8) + 
            (framebuffer_addr[where + 2] << 16);
    return color;
}

uint32_t rgb(uint8_t r, uint8_t g, uint8_t b) {
    return 0xFF000000 + (r * 0x10000) + (g * 0x100) + (b * 0x1);
}

uint32_t rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    return (a * 0x1000000) + (r * 0x10000) + (g * 0x100) + (b * 0x1);
}

void draw_horizontal_line(int x, int y,int length, uint32_t color) {
    int i;
    for (i = 0; i <= length; i++) {
        set_pixel(x, y, color);
        x++;
    }
}

void draw_vertical_line(int x, int y, int length, uint32_t color) {
    int i;
    for (i = 0; i <= length; i++) {
        set_pixel(x, y, color);
        y++;
    }
}

void draw_square(int x, int y, int width, int height, uint32_t color) {
    draw_horizontal_line(x, y, width, color);
    draw_vertical_line(x, y, height, color);
    draw_horizontal_line(x, y + height, width, color);
    draw_vertical_line(x + width, y, height, color);
}

void draw_fill(int start_x, int start_y, int length_across, int length_down, uint32_t color) {
    int i, j;
    for (i = 0; i < length_down; i++) {
        for (j = 0; j < length_across; j++) {
            set_pixel(start_x + j, start_y + i, color);
        }
    }
}

void draw_vga_character(uint8_t c, int x, int y, int fg, int bg, bool bgon) {
    //if (tty_pos_x + 7 > 1024 /*|| tty_pos_y + 15 > 768*/) draw_fill(50, 50, 100, 100, 0x0000AA);

    int cx, cy;
    int mask[8] = { 128, 64, 32, 16, 8, 4, 2, 1 };
    unsigned char *glyph = (uint8_t*) vgafnt + (int) c * 16;

    for (cy = 0; cy < 16; cy++) {
        for (cx = 0; cx < 8; cx++) {
            if (glyph[cy] & mask[cx]) {
                set_pixel(x + cx, y + cy, fg);
            } else if (bgon == true) {
                set_pixel(x + cx, y + cy, bg);
            }
        }
    }
}

void draw_text_string(const char *text, int x, int y, int fg, int bg, bool bgon) {
    int len = strlen(text);
    for (int i = 0; i < len; i++) {
        if (x + 8 <= 1024) {
            draw_vga_character(text[i], x, y, fg, bg, bgon);
            x += 8;
        } else {
            break;
        }
    }
}
