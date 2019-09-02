/*
*    EOS - Experimental Operating System
*    Terminal subsystem
*/
#include <kernel/tty.h>
#include <kernel/graphics/vesafb.h>
#include <kernel/graphics/vgafnt.h>

#include <libk/string.h>

volatile uint8_t tty_feedback = 1;

size_t tty_line_fill[1024];
int tty_pos_x;
int tty_pos_y;
uint32_t tty_text_color;

/*void update_cursor(size_t row, size_t col) {}*/

void tty_init() {
    tty_pos_y = 0;
    tty_pos_x = 0;

    tty_text_color = VESA_LIGHT_CYAN;
}


void draw_vga_character(uint8_t c, int x, int y, int fg, int bg, bool bgon)
{
    //if (tty_pos_x + 7 > 1024 /*|| tty_pos_y + 15 > 768*/) draw_fill(50, 50, 100, 100, 0x0000AA);
    int cx,cy;
    int mask[8]={128,64,32,16,8,4,2,1};
    unsigned char *glyph=(uint8_t*)vgafnt+(int)c*16;
    for(cy=0;cy<16;cy++){
        for(cx=0;cx<8;cx++){
            if(glyph[cy]&mask[cx]) set_pixel(x+cx,y+cy,fg);
            else if(bgon == true) set_pixel(x+cx,y+cy,bg);
        }
    }
}

void tty_backspace() {//!!!!!!!!!!!
    if (tty_pos_x < 8) { //old: == 0
        if (tty_pos_y >= 17) {
            tty_pos_y -= 17;
        }
        tty_pos_x = tty_line_fill[tty_pos_y];
    } else {
        tty_pos_x -= 8;
    }
    draw_vga_character(' ', tty_pos_x, tty_pos_y, tty_text_color, 0x000000, 1);
    //update_cursor(tty_pos_y, tty_pos_x);
}

void tty_setcolor(uint32_t color) { tty_text_color = color; }

void tty_putchar(char c) {
    //draw_fill(50, 50, 100, 100, 0x0000AA);
    if ((tty_pos_x + 8) >= VESA_WIDTH || c == '\n') { // old == ==
        tty_line_fill[tty_pos_y] = tty_pos_x;
        tty_pos_x = 0;
        if ((tty_pos_y + 17) >= VESA_HEIGHT) { //old ==
            //draw_fill(50, 50, 100, 100, 0x0000AA);
            tty_scroll();
        } else
        {
            tty_pos_y += 17;
        }
    } else
    {
        //if (tty_pos_x + 7 > 1024 || tty_pos_y + 15 > 768) draw_fill(50, 50, 100, 100, 0x0000AA);
        if ((tty_pos_y + 17) >= VESA_HEIGHT) {
            tty_scroll();
        }
        draw_vga_character(c, tty_pos_x, tty_pos_y, tty_text_color, 0x000000, 0);
        tty_pos_x += 8;
    }
    //update_cursor(tty_pos_y, tty_pos_x);
}

/*
void tty_putchar(char c) {
    draw_fill(50, 50, 100, 100, 0x0000AA);
    if (c != '\n') {
        draw_vga_character(c, tty_pos_x, tty_pos_y, tty_text_color, 0x000000, 0);
    }

    if ((tty_pos_x += 8) >= VESA_WIDTH || c == '\n') { // old == ==
        tty_line_fill[tty_pos_y] = tty_pos_x -= 8;
        tty_pos_x = 0;
        if ((tty_pos_y += 17) >= VESA_HEIGHT) { //old ==
            tty_scroll();
        }
    }
    //update_cursor(tty_pos_y, tty_pos_x);
}

*/

/*void tty_scroll() {
  tty_pos_y--;
  for (size_t y = 0; y < VESA_HEIGHT - 1; y++) {
    for (size_t x = 0; x < VESA_WIDTH; x++) {
      const size_t src_index = y * VESA_WIDTH + x;
      const size_t dstty_index = (y + 1) * VESA_WIDTH + x;
      tty_buffer[src_index] = tty_buffer[dstty_index];
    }
    tty_line_fill[y] = tty_line_fill[y + 1];
  }

  for (size_t x = 0; x < VESA_WIDTH; x++) {
    const size_t index = (VESA_HEIGHT - 1) * VESA_WIDTH + x;
    tty_buffer[index] = vga_entry(' ', tty_color);
  }
}*/


//Scrolls the display up number of rows
void tty_scroll() {//??????
    // charheight = 16???
    unsigned int num_rows = 1;
    tty_pos_y -= 17*num_rows;
    // Copy rows upwards
    uint8_t *read_ptr = (uint8_t *) back_framebuffer_addr + ((num_rows * 17) * framebuffer_pitch);
    uint8_t *write_ptr = (uint8_t *) back_framebuffer_addr;
    uint32_t num_bytes = (framebuffer_pitch * VESA_HEIGHT) - (framebuffer_pitch * (num_rows * 17));//old: unsigned old
    memcpy(write_ptr, read_ptr, num_bytes);

    // Clear the rows at the end
    write_ptr = (uint8_t *) back_framebuffer_addr + (framebuffer_pitch * VESA_HEIGHT) - (framebuffer_pitch * (num_rows * 17));
    memset(write_ptr, 0, framebuffer_pitch * (num_rows * 17));

    //swap buffers
    memcpy(framebuffer_addr, back_framebuffer_addr, framebuffer_size);
}


void tty_write(const char* data, size_t size) {
  for (size_t i = 0; i < size; i++) tty_putchar(data[i]);
}

void tty_putstring(const char* data) { tty_write(data, strlen(data)); }

void tty_putuint(int i)
{
    unsigned int n, d = 1000000000;
    char str[255];
    unsigned int dec_index = 0;
    while( ( i/d == 0 ) && ( d >= 10 ) ) d /= 10;
    n = i;
    while(d >= 10)
    {
        str[dec_index++] = ((char)((int)'0' + n/d));
        n = n % d;
        d /= 10;
    }
    str[dec_index++] = ((char)((int)'0' + n));
    str[dec_index] = 0;
    tty_putstring(str);
}

void tty_putint(int i)
{
    if(i >= 0)
    {
        tty_putuint(i);
    } else {
        tty_putchar('-');
        tty_putuint(-i);
    }
}


void tty_puthex(uint32_t i)
{
    const unsigned char hex[16]  =  { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
    unsigned int n, d = 0x10000000;

    tty_putstring("0x");
    while((i / d == 0) && (d >= 0x10)) d /= 0x10;
    n = i;
    while( d >= 0xF )
    {
        tty_putchar(hex[n/d]);
        n = n % d;
        d /= 0x10;
    }
    tty_putchar(hex[n]);
}


void tty_print(char *format, va_list args)
{
    int i = 0;
    char *string;

    while (format[i])
    {
        if (format[i] == '%')
        {
            i++;
            switch (format[i])
            {
                case 's':
                    string = va_arg(args, char*);
                    tty_putstring(string);
                    break;
                case 'c':
                    // To-Do: fix this! "warning: cast to pointer from integer of different size"
                    tty_putchar(va_arg(args, int));
                    break;
                case 'd':
                    tty_putint(va_arg(args, int));
                    break;
                case 'i':
                    tty_putint(va_arg(args, int));
                    break;
                case 'u':
                    tty_putuint(va_arg(args, unsigned int));
                    break;
                case 'x':
                    tty_puthex(va_arg(args, uint32_t));
                    break;
                default:
                    tty_putchar(format[i]);
            }
        } else {
            tty_putchar(format[i]);
        }
        i++;
    }//endwhile
}

void tty_printf(char *text, ... )
{
    va_list args;
    // find the first argument
    va_start(args, text);
    // pass print the output handle the format text and the first argument
    tty_print(text, args);
}

void tty_putstring_color(const char* data, uint32_t text_color)
{
    uint32_t tty_text_color_old = tty_text_color;
    tty_setcolor(text_color);
    tty_putstring(data);
    tty_setcolor(tty_text_color_old);
}